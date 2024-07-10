#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"

/**
 * RenderTargetFiller class for filling render targets with data.
 */
class RenderTargetFiller
{
public:
    // Static function to fill a render target with data from a TArray<FVector4>
    static void FillRenderTarget(UTextureRenderTarget2D* RenderTarget, const TArray<FVector4>& Data);
};
