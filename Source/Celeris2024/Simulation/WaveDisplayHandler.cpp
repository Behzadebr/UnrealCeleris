#include "WaveDisplayHandler.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "RenderGraphUtils.h"
#include "RHICommandList.h"
#include "Engine/TextureRenderTarget2D.h"

class FWaveDisplayComputeShader : public FGlobalShader
{
    DECLARE_GLOBAL_SHADER(FWaveDisplayComputeShader)
    SHADER_USE_PARAMETER_STRUCT(FWaveDisplayComputeShader, FGlobalShader)

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txState)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txBottom)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txAuxiliary2)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txShipPressure)
        SHADER_PARAMETER(int, width)
        SHADER_PARAMETER(int, height)
        SHADER_PARAMETER(float, dx)
        SHADER_PARAMETER(float, dy)
        SHADER_PARAMETER(float, epsilon)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, txDisplay)
    END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FWaveDisplayComputeShader, "/Celeris2024/WaveDisplay.usf", "MainCS", SF_Compute)

static FWaveDisplayComputeShader::FParameters WaveDisplayParameters;

void WaveDisplayHandler::Setup(int width, int height, float dx, float dy, float epsilon)
{
    WaveDisplayParameters.width = width;
    WaveDisplayParameters.height = height;
    WaveDisplayParameters.dx = dx;
    WaveDisplayParameters.dy = dy;
    WaveDisplayParameters.epsilon = epsilon;
}

void WaveDisplayHandler::Execute(UTextureRenderTarget2D* txState, UTextureRenderTarget2D* txBottom, UTextureRenderTarget2D* txAuxiliary2, UTextureRenderTarget2D* txShipPressure, UTextureRenderTarget2D* txDisplay)
{
    ENQUEUE_RENDER_COMMAND(FWaveDisplayComputeShader)(
        [txState, txBottom, txAuxiliary2, txShipPressure, txDisplay](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder GraphBuilder(RHICmdList);
            FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

            FRDGTextureRef txStateRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txState->GetRenderTargetResource()->TextureRHI, TEXT("txState")));
            FRDGTextureRef txBottomRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txBottom->GetRenderTargetResource()->TextureRHI, TEXT("txBottom")));
            FRDGTextureRef txAuxiliary2RDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txAuxiliary2->GetRenderTargetResource()->TextureRHI, TEXT("txAuxiliary2")));
            FRDGTextureRef txShipPressureRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txShipPressure->GetRenderTargetResource()->TextureRHI, TEXT("txShipPressure")));
            FRDGTextureRef txDisplayRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txDisplay->GetRenderTargetResource()->TextureRHI, TEXT("txDisplay")));

            WaveDisplayParameters.txState = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txStateRDG));
            WaveDisplayParameters.txBottom = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txBottomRDG));
            WaveDisplayParameters.txAuxiliary2 = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txAuxiliary2RDG));
            WaveDisplayParameters.txShipPressure = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txShipPressureRDG));
            WaveDisplayParameters.txDisplay = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(txDisplayRDG));

            TShaderMapRef<FWaveDisplayComputeShader> ComputeShader(GlobalShaderMap);
            FComputeShaderUtils::AddPass(
                GraphBuilder,
                RDG_EVENT_NAME("WaveDisplayComputeShader"),
                ComputeShader,
                &WaveDisplayParameters,
                FIntVector(txDisplay->SizeX / 32, txDisplay->SizeY / 32, 1)
            );

            GraphBuilder.Execute();
        }
    );
}
