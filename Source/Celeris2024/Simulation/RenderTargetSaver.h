#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"

class RenderTargetSaver
{
public:
    RenderTargetSaver(const FString& InSaveDirectory);

    // Method to save render target to a file with the given texture name
    bool SaveRenderTargetToFile(UTextureRenderTarget2D* RenderTarget, const FString& TextureName) const;

private:
    FString SaveDirectory;
};
