#include <cstdio>
#include <cstring>
#include <vector>
#include "frame_test.h"
#include "Assets/COLFile.h"
#include "Assets/LGTFile.h"
#include "Rendering/RenderDrawCommand.h"

static ObjectTextureID makeTexture(SoftwareRenderer &r, int w, int h, int bpt, const void *data)
{
    const ObjectTextureID id = r.createTexture(w, h, bpt);
    if (id < 0) { printf("createTexture fallo\n"); return -1; }
    LockedTexture lt = r.lockTexture(id);
    if (!lt.isValid()) { printf("lockTexture fallo\n"); return -1; }
    std::memcpy(lt.texels.begin(), data, static_cast<size_t>(w) * h * bpt);
    r.unlockTexture(id);
    return id;
}

bool buildTestScene(SoftwareRenderer &r, TestScene &scene)
{
    // Paleta de 32 bits (ARGB) desde PAL.COL.
    COLFile col;
    if (!col.init("PAL.COL")) { printf("PAL.COL fallo\n"); return false; }
    std::vector<uint32_t> pal(256);
    for (int i = 0; i < 256; i++)
    {
        const Color &c = col.getPalette()[i];
        pal[i] = (0xFFu << 24) | (c.r << 16) | (c.g << 8) | c.b;
    }
    const ObjectTextureID palTex = makeTexture(r, 256, 1, 4, pal.data());

    // Tabla de luz: 13 paletas de 256 bytes.
    LGTFile lgt;
    if (!lgt.init("NORMAL.LGT")) { printf("NORMAL.LGT fallo\n"); return false; }
    const Span2D<const uint8_t> lights = lgt.getAllLightPalettes();
    const ObjectTextureID lightTex = makeTexture(r, 256, LGTFile::PALETTE_COUNT, 1, lights.begin());

    const uint8_t zero = 0;
    const ObjectTextureID ditherTex = makeTexture(r, 1, 1, 1, &zero);
    const ObjectTextureID skyTex = makeTexture(r, 1, 1, 1, &zero);

    // Textura del cuadrado: 64x64 de degradado de indices de paleta.
    std::vector<uint8_t> tex(64 * 64);
    for (int y = 0; y < 64; y++)
        for (int x = 0; x < 64; x++)
            tex[x + y * 64] = static_cast<uint8_t>(16 + ((x / 8 + y / 8) % 2) * 100 + (y / 8));
    const ObjectTextureID quadTex = makeTexture(r, 64, 64, 1, tex.data());
    if (palTex < 0 || lightTex < 0 || ditherTex < 0 || skyTex < 0 || quadTex < 0) return false;

    // Malla: un cuadrado (pared) en el plano x = 0.
    const double pos[12] = { 0.0, 2.0, -1.0,  0.0, 0.0, -1.0,  0.0, 0.0, 1.0,  0.0, 2.0, 1.0 };
    const double nrm[12] = { -1,0,0, -1,0,0, -1,0,0, -1,0,0 };
    const double uv[8] = { 0,0, 0,1, 1,1, 1,0 };
    const int idx[6] = { 0, 1, 2, 0, 2, 3 };

    const VertexPositionBufferID posBuf = r.createVertexPositionBuffer(4, 3, sizeof(double));
    const VertexAttributeBufferID nrmBuf = r.createVertexAttributeBuffer(4, 3, sizeof(double));
    const VertexAttributeBufferID uvBuf = r.createVertexAttributeBuffer(4, 2, sizeof(double));
    const IndexBufferID idxBuf = r.createIndexBuffer(6, sizeof(int));
    LockedBuffer lb = r.lockVertexPositionBuffer(posBuf); std::memcpy(lb.bytes.begin(), pos, sizeof(pos)); r.unlockVertexPositionBuffer(posBuf);
    lb = r.lockVertexAttributeBuffer(nrmBuf); std::memcpy(lb.bytes.begin(), nrm, sizeof(nrm)); r.unlockVertexAttributeBuffer(nrmBuf);
    lb = r.lockVertexAttributeBuffer(uvBuf); std::memcpy(lb.bytes.begin(), uv, sizeof(uv)); r.unlockVertexAttributeBuffer(uvBuf);
    lb = r.lockIndexBuffer(idxBuf); std::memcpy(lb.bytes.begin(), idx, sizeof(idx)); r.unlockIndexBuffer(idxBuf);

    // Transformacion (identidad) y buffer de luces (vacio).
    constexpr int STRESS_N = 200;
    const UniformBufferID xfBuf = r.createUniformBuffer(STRESS_N, sizeof(Matrix4d), alignof(Matrix4d));
    for (int i = 0; i < STRESS_N; i++)
    {
        const Matrix4d m = Matrix4d::translation(i * 0.02, 0.0, 0.0);
        lb = r.lockUniformBufferIndex(xfBuf, i);
        std::memcpy(lb.bytes.begin(), &m, sizeof(Matrix4d));
        r.unlockUniformBufferIndex(xfBuf, i);
    }
    const UniformBufferID lightsBuf = r.createUniformBuffer(1, 64, 8);

    // Material.
    RenderMaterialKey key;
    key.init(VertexShaderType::Basic, FragmentShaderType::Opaque, Span<const ObjectTextureID>(&quadTex, 1),
        RenderLightingType::PerMesh, false, true, true);
    const RenderMaterialID mat = r.createMaterial(key);
    const RenderMaterialInstanceID matInst = r.createMaterialInstance();
    r.setMaterialInstanceMeshLightPercent(matInst, 1.0);

    scene.drawCalls.resize(STRESS_N);
    for (int i = 0; i < STRESS_N; i++)
    {
        RenderDrawCall &dc = scene.drawCalls[i];
        dc.transformBufferID = xfBuf;
        dc.transformIndex = i;
        dc.positionBufferID = posBuf;
        dc.normalBufferID = nrmBuf;
        dc.texCoordBufferID = uvBuf;
        dc.indexBufferID = idxBuf;
        dc.materialID = mat;
        dc.materialInstID = matInst;
        dc.multipassType = RenderMultipassType::None;
    }

    scene.frameSettings.init(Color(0, 0, 0), 1.0, lightsBuf, 0, 0.0, palTex, lightTex, ditherTex, skyTex, 1, DitheringMode::None);
    printf("escena lista: pos=%d idx=%d mat=%d\n", posBuf, idxBuf, mat);
    return true;
}

void renderTestFrame(SoftwareRenderer &r, const TestScene &scene, double yaw, uint32_t *out)
{
    RenderCamera cam;
    cam.init(WorldDouble3(-3.0, 1.0, 0.0), yaw, 0.0, 60.0, 320.0 / 200.0, 1.2);
    RenderDrawCommandList list;
    list.addDrawCalls(Span<const RenderDrawCall>(scene.drawCalls.data(), static_cast<int>(scene.drawCalls.size())));
    r.submitFrame(list, cam, scene.frameSettings, out);
}


