#pragma once
#include <cstdint>
#include "Rendering/RenderShaderUtils.h"
#include "Rendering/RenderTextureUtils.h"
#include "Rendering/RenderMeshUtils.h"
#include "Rendering/RenderBuffer.h"
#include "Rendering/RenderMaterialUtils.h"
#include "Rendering/RenderInitSettings.h"
#include "Rendering/RenderDrawCall.h"
#include "Rendering/RenderDrawCommand.h"
#include "Rendering/RenderFrameSettings.h"
#include "Rendering/RenderCamera.h"
#include "Rendering/SoftwareRenderer.h"

struct TestScene
{
    RenderDrawCall drawCall;
    RenderFrameSettings frameSettings;
};

bool buildTestScene(SoftwareRenderer &renderer, TestScene &scene);
void renderTestFrame(SoftwareRenderer &renderer, const TestScene &scene, double yaw, uint32_t *out);
