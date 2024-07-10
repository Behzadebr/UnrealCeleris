#pragma once

#include "CoreMinimal.h"
#include "RHI.h"
#include "RHIResources.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "RenderGraphUtils.h"
#include "TextureResource.h"

class Pass3_BousHandler
{
public:
    static void Setup(int width, int height, float dt, float dx, float dy, float one_over_dx, float one_over_dy, float g_over_dx, float g_over_dy, int timeScheme, float epsilon, int isManning, float g, float friction, int pred_or_corrector, float Bcoef, float Bcoef_g, float one_over_d2x, float one_over_d3x, float one_over_d2y, float one_over_d3y, float one_over_dxdy, float seaLevel);

    static void Execute(UTextureRenderTarget2D* txState, UTextureRenderTarget2D* txBottom, UTextureRenderTarget2D* txH, UTextureRenderTarget2D* txXFlux, UTextureRenderTarget2D* txYFlux, UTextureRenderTarget2D* oldGradients, UTextureRenderTarget2D* oldOldGradients, UTextureRenderTarget2D* predictedGradients, UTextureRenderTarget2D* F_G_star_oldOldGradients, UTextureRenderTarget2D* txstateUVstar, UTextureRenderTarget2D* txShipPressure, UTextureRenderTarget2D* txNewState, UTextureRenderTarget2D* dU_by_dt, UTextureRenderTarget2D* F_G_star, UTextureRenderTarget2D* current_stateUVstar);
};