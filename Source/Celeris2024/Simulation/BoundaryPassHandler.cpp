#include "BoundaryPassHandler.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "RenderGraphUtils.h"
#include "RHICommandList.h"
#include "Engine/TextureRenderTarget2D.h"

class FBoundaryPassComputeShader : public FGlobalShader
{
    DECLARE_GLOBAL_SHADER(FBoundaryPassComputeShader)
    SHADER_USE_PARAMETER_STRUCT(FBoundaryPassComputeShader, FGlobalShader)

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txState)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txBottom)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txWaves)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, txNewState)
        SHADER_PARAMETER(int, width)
        SHADER_PARAMETER(int, height)
        SHADER_PARAMETER(float, dt)
        SHADER_PARAMETER(float, dx)
        SHADER_PARAMETER(float, dy)
        SHADER_PARAMETER(float, total_time)
        SHADER_PARAMETER(int, reflect_x)
        SHADER_PARAMETER(int, reflect_y)
        SHADER_PARAMETER(float, PI_constant)
        SHADER_PARAMETER(float, BoundaryWidth)
        SHADER_PARAMETER(float, seaLevel)
        SHADER_PARAMETER(int, boundary_nx)
        SHADER_PARAMETER(int, boundary_ny)
        SHADER_PARAMETER(int, numberOfWaves)
        SHADER_PARAMETER(int, west_boundary_type)
        SHADER_PARAMETER(int, east_boundary_type)
        SHADER_PARAMETER(int, south_boundary_type)
        SHADER_PARAMETER(int, north_boundary_type)
        SHADER_PARAMETER(float, boundary_g)
    END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FBoundaryPassComputeShader, "/Celeris2024/BoundaryPass.usf", "MainCS", SF_Compute)

static FBoundaryPassComputeShader::FParameters BoundaryPassParameters;

void BoundaryPassHandler::Setup(int width, int height, float dt, float dx, float dy, float PI_constant, float BoundaryWidth, float seaLevel, int boundary_nx, int boundary_ny, int numberOfWaves, int west_boundary_type, int east_boundary_type, int south_boundary_type, int north_boundary_type, float boundary_g)
{
    BoundaryPassParameters.width = width;
    BoundaryPassParameters.height = height;
    BoundaryPassParameters.dt = dt;
    BoundaryPassParameters.dx = dx;
    BoundaryPassParameters.dy = dy;
    BoundaryPassParameters.PI_constant = PI_constant;
    BoundaryPassParameters.BoundaryWidth = BoundaryWidth;
    BoundaryPassParameters.seaLevel = seaLevel;
    BoundaryPassParameters.boundary_nx = boundary_nx;
    BoundaryPassParameters.boundary_ny = boundary_ny;
    BoundaryPassParameters.numberOfWaves = numberOfWaves;
    BoundaryPassParameters.west_boundary_type = west_boundary_type;
    BoundaryPassParameters.east_boundary_type = east_boundary_type;
    BoundaryPassParameters.south_boundary_type = south_boundary_type;
    BoundaryPassParameters.north_boundary_type = north_boundary_type;
    BoundaryPassParameters.boundary_g = boundary_g;
}

void BoundaryPassHandler::Execute(UTextureRenderTarget2D* txState, UTextureRenderTarget2D* txBottom, UTextureRenderTarget2D* txWaves, UTextureRenderTarget2D* txNewState, float total_time)
{
    ENQUEUE_RENDER_COMMAND(FBoundaryPassComputeShader)(
        [txState, txBottom, txWaves, txNewState, total_time](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder GraphBuilder(RHICmdList);
            FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

            FRDGTextureRef txStateRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txState->GetRenderTargetResource()->TextureRHI, TEXT("txState")));
            FRDGTextureRef txBottomRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txBottom->GetRenderTargetResource()->TextureRHI, TEXT("txBottom")));
            FRDGTextureRef txWavesRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txWaves->GetRenderTargetResource()->TextureRHI, TEXT("txWaves")));
            FRDGTextureRef txNewStateRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txNewState->GetRenderTargetResource()->TextureRHI, TEXT("txNewState")));

            BoundaryPassParameters.txState = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txStateRDG));
            BoundaryPassParameters.txBottom = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txBottomRDG));
            BoundaryPassParameters.txWaves = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txWavesRDG));
            BoundaryPassParameters.txNewState = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(txNewStateRDG));
            BoundaryPassParameters.total_time = total_time;

            TShaderMapRef<FBoundaryPassComputeShader> ComputeShader(GlobalShaderMap);
            FComputeShaderUtils::AddPass(
                GraphBuilder,
                RDG_EVENT_NAME("BoundaryPassComputeShader"),
                ComputeShader,
                &BoundaryPassParameters,
                FIntVector(txState->SizeX / 32, txState->SizeY / 32, 1)
            );

            GraphBuilder.Execute();
        }
    );

    // Copy txNewState to txState
    ENQUEUE_RENDER_COMMAND(CopyTxNewStateToTxState)(
        [txNewState, txState](FRHICommandListImmediate& RHICmdList)
        {
            FRHITexture* NewStateTexture = txNewState->GetRenderTargetResource()->TextureRHI.GetReference();
            FRHITexture* StateTexture = txState->GetRenderTargetResource()->TextureRHI.GetReference();
            RHICmdList.CopyTexture(NewStateTexture, StateTexture, FRHICopyTextureInfo());
        }
    );
}
