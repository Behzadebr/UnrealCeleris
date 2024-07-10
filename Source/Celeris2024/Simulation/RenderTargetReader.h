#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"

/**
 * FloatingPointRenderTargetReader class for reading floating-point values from a render target.
 */
class RenderTargetReader
{
public:
    // Static function to read and log the floating-point value at a specific texel
    static void ReadAndLogValueAtTexel(UTextureRenderTarget2D* RenderTarget, int32 TexelX, int32 TexelY);
};
