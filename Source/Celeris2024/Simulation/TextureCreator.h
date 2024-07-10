#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"

/**
 * TextureCreator class for creating and initializing render targets.
 */
class TextureCreator
{
public:
    // Static function to create and initialize a render target
    static UTextureRenderTarget2D* CreateInitializedRenderTarget(UObject* Outer, int32 width, int32 height, const TArray<FVector4>* Data = nullptr);
};
