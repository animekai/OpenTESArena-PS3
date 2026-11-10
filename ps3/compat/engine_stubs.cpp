// Stubs para enlazar SoftwareRenderer sin Renderer.cpp ni ArenaRenderUtils.cpp (que arrastran Jolt y la UI)
#include "Rendering/Renderer.h"
#include "Rendering/ArenaRenderUtils.h"

bool ArenaRenderUtils::isLightLevelTexel(uint8_t texel)
{
    return (texel >= ArenaRenderUtils::PALETTE_INDEX_LIGHT_LEVEL_LOWEST) && (texel <= ArenaRenderUtils::PALETTE_INDEX_LIGHT_LEVEL_HIGHEST);
}

ObjectTextureID Renderer::createObjectTexture(int, int, int) { return -1; }
void Renderer::freeObjectTexture(ObjectTextureID) {}
std::optional<Int2> Renderer::tryGetObjectTextureDims(ObjectTextureID) const { return std::nullopt; }
LockedTexture Renderer::lockObjectTexture(ObjectTextureID) { return LockedTexture(); }
void Renderer::unlockObjectTexture(ObjectTextureID) {}
void Renderer::freeUiTexture(UiTextureID) {}
std::optional<Int2> Renderer::tryGetUiTextureDims(UiTextureID) const { return std::nullopt; }
LockedTexture Renderer::lockUiTexture(UiTextureID) { return LockedTexture(); }
void Renderer::unlockUiTexture(UiTextureID) {}
