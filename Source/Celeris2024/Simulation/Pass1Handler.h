#pragma once

#include "CoreMinimal.h"
#include "RHI.h"
#include "RHIResources.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "RenderGraphUtils.h"
#include "TextureResource.h"

class Pass1Handler
{
public:
    static void Setup(int width, int height, float one_over_dx, float one_over_dy, float dissipation_threshold,
                      float TWO_THETA, float epsilon, float whiteWaterDecayRate, float dt, float base_depth);

    static void Execute(UTextureRenderTarget2D* txState, UTextureRenderTarget2D* txBottom, UTextureRenderTarget2D* txAuxiliary2,
                        UTextureRenderTarget2D* txH, UTextureRenderTarget2D* txU, UTextureRenderTarget2D* txV,
                        UTextureRenderTarget2D* txNormal, UTextureRenderTarget2D* txAuxiliary2Out,
                        UTextureRenderTarget2D* txW, UTextureRenderTarget2D* txC);
};
