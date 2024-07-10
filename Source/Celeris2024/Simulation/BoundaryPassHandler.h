#pragma once

#include "CoreMinimal.h"
#include "RHI.h"
#include "RHIResources.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "RenderGraphUtils.h"
#include "TextureResource.h"

class BoundaryPassHandler
{
public:
    static void Setup(int width, int height, float dt, float dx, float dy, float PI_constant, float BoundaryWidth, float seaLevel, int boundary_nx, int boundary_ny, int numberOfWaves, int west_boundary_type, int east_boundary_type, int south_boundary_type, int north_boundary_type, float boundary_g);

    static void Execute(UTextureRenderTarget2D* txState, UTextureRenderTarget2D* txBottom, UTextureRenderTarget2D* txWaves, UTextureRenderTarget2D* txNewState, float total_time);
};
