#pragma once

#include "CoreMinimal.h"
#include "RHI.h"
#include "RHIResources.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "RenderGraphUtils.h"
#include "TextureResource.h"

class ShaderManager
{
public:
    static void ExecuteComputeShader(UTextureRenderTarget2D* InputTexture, UTextureRenderTarget2D* OutputTexture, float Theta);
};
