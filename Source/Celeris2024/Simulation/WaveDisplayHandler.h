#pragma once

#include "CoreMinimal.h"
#include "RHI.h"
#include "RHIResources.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "RenderGraphUtils.h"
#include "TextureResource.h"

class WaveDisplayHandler
{
public:
    static void Setup(int width, int height, float dx, float dy, float epsilon);

    static void Execute(UTextureRenderTarget2D* txState, UTextureRenderTarget2D* txBottom, UTextureRenderTarget2D* txAuxiliary2, UTextureRenderTarget2D* txShipPressure, UTextureRenderTarget2D* txDisplay);
};
