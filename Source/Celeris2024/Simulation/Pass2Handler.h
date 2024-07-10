#pragma once

#include "CoreMinimal.h"
#include "RHI.h"
#include "RHIResources.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "RenderGraphUtils.h"
#include "TextureResource.h"

class Pass2Handler
{
public:
    static void Setup(int width, int height, float g, float half_g, float dx, float dy);

    static void Execute(UTextureRenderTarget2D* txH, UTextureRenderTarget2D* txU, UTextureRenderTarget2D* txV, UTextureRenderTarget2D* txBottom, UTextureRenderTarget2D* txC, 
                        UTextureRenderTarget2D* txXFlux, UTextureRenderTarget2D* txYFlux);
};
