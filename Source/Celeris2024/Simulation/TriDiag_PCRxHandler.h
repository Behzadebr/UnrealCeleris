#pragma once

#include "CoreMinimal.h"
#include "RHI.h"
#include "RHIResources.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "RenderGraphUtils.h"
#include "TextureResource.h"

class TriDiag_PCRxHandler
{
public:
    static void Setup(int width, int height);
    static void Execute(UTextureRenderTarget2D* coefMatx, UTextureRenderTarget2D* newcoef, UTextureRenderTarget2D* txtemp, UTextureRenderTarget2D* txtemp2, UTextureRenderTarget2D* current_stateUVstar, UTextureRenderTarget2D* txNewState, int Px, int NLSW_or_Bous);
};
