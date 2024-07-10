#include "Pass1Handler.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "RenderGraphUtils.h"
#include "RHICommandList.h"
#include "Engine/TextureRenderTarget2D.h"

class FPass1ComputeShader : public FGlobalShader
{
    DECLARE_GLOBAL_SHADER(FPass1ComputeShader)
    SHADER_USE_PARAMETER_STRUCT(FPass1ComputeShader, FGlobalShader)

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txState)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txBottom)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txAuxiliary2)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, txH)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, txU)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, txV)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, txNormal)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, txAuxiliary2Out)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, txW)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, txC)
        SHADER_PARAMETER(int, width)
        SHADER_PARAMETER(int, height)
        SHADER_PARAMETER(float, one_over_dx)
        SHADER_PARAMETER(float, one_over_dy)
        SHADER_PARAMETER(float, dissipation_threshold)
        SHADER_PARAMETER(float, TWO_THETA)
        SHADER_PARAMETER(float, epsilon)
        SHADER_PARAMETER(float, whiteWaterDecayRate)
        SHADER_PARAMETER(float, dt)
        SHADER_PARAMETER(float, base_depth)
    END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FPass1ComputeShader, "/Celeris2024/Pass1.usf", "MainCS", SF_Compute)

static FPass1ComputeShader::FParameters Pass1Parameters;

void Pass1Handler::Setup(int width, int height, float one_over_dx, float one_over_dy, float dissipation_threshold,
                         float TWO_THETA, float epsilon, float whiteWaterDecayRate, float dt, float base_depth)
{
    Pass1Parameters.width = width;
    Pass1Parameters.height = height;
    Pass1Parameters.one_over_dx = one_over_dx;
    Pass1Parameters.one_over_dy = one_over_dy;
    Pass1Parameters.dissipation_threshold = dissipation_threshold;
    Pass1Parameters.TWO_THETA = TWO_THETA;
    Pass1Parameters.epsilon = epsilon;
    Pass1Parameters.whiteWaterDecayRate = whiteWaterDecayRate;
    Pass1Parameters.dt = dt;
    Pass1Parameters.base_depth = base_depth;
}

void Pass1Handler::Execute(UTextureRenderTarget2D* txState, UTextureRenderTarget2D* txBottom, UTextureRenderTarget2D* txAuxiliary2,
                           UTextureRenderTarget2D* txH, UTextureRenderTarget2D* txU, UTextureRenderTarget2D* txV,
                           UTextureRenderTarget2D* txNormal, UTextureRenderTarget2D* txAuxiliary2Out,
                           UTextureRenderTarget2D* txW, UTextureRenderTarget2D* txC)
{
    ENQUEUE_RENDER_COMMAND(FPass1ComputeShader)(
        [txState, txBottom, txAuxiliary2, txH, txU, txV, txNormal, txAuxiliary2Out, txW, txC](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder GraphBuilder(RHICmdList);
            FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

            FRDGTextureRef txStateRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txState->GetRenderTargetResource()->TextureRHI, TEXT("txState")));
            FRDGTextureRef txBottomRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txBottom->GetRenderTargetResource()->TextureRHI, TEXT("txBottom")));
            FRDGTextureRef txAuxiliary2RDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txAuxiliary2->GetRenderTargetResource()->TextureRHI, TEXT("txAuxiliary2")));

            FRDGTextureRef txHRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txH->GetRenderTargetResource()->TextureRHI, TEXT("txH")));
            FRDGTextureRef txURDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txU->GetRenderTargetResource()->TextureRHI, TEXT("txU")));
            FRDGTextureRef txVRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txV->GetRenderTargetResource()->TextureRHI, TEXT("txV")));
            FRDGTextureRef txNormalRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txNormal->GetRenderTargetResource()->TextureRHI, TEXT("txNormal")));
            FRDGTextureRef txAuxiliary2OutRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txAuxiliary2Out->GetRenderTargetResource()->TextureRHI, TEXT("txAuxiliary2Out")));
            FRDGTextureRef txWRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txW->GetRenderTargetResource()->TextureRHI, TEXT("txW")));
            FRDGTextureRef txCRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txC->GetRenderTargetResource()->TextureRHI, TEXT("txC")));

            Pass1Parameters.txState = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txStateRDG));
            Pass1Parameters.txBottom = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txBottomRDG));
            Pass1Parameters.txAuxiliary2 = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txAuxiliary2RDG));

            Pass1Parameters.txH = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(txHRDG));
            Pass1Parameters.txU = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(txURDG));
            Pass1Parameters.txV = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(txVRDG));
            Pass1Parameters.txNormal = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(txNormalRDG));
            Pass1Parameters.txAuxiliary2Out = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(txAuxiliary2OutRDG));
            Pass1Parameters.txW = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(txWRDG));
            Pass1Parameters.txC = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(txCRDG));

            TShaderMapRef<FPass1ComputeShader> ComputeShader(GlobalShaderMap);
            FComputeShaderUtils::AddPass(
                GraphBuilder,
                RDG_EVENT_NAME("Pass1ComputeShader"),
                ComputeShader,
                &Pass1Parameters,
                FIntVector(txState->SizeX / 32, txState->SizeY / 32, 1)
            );

            GraphBuilder.Execute();
        }
    );

    // Copy txAuxiliary2Out to txAuxiliary2
    ENQUEUE_RENDER_COMMAND(CopyAuxiliary2)(
        [txAuxiliary2, txAuxiliary2Out](FRHICommandListImmediate& RHICmdList)
        {
            FRHITexture* Auxiliary2Texture = txAuxiliary2->GetRenderTargetResource()->TextureRHI.GetReference();
            FRHITexture* Auxiliary2OutTexture = txAuxiliary2Out->GetRenderTargetResource()->TextureRHI.GetReference();
            RHICmdList.CopyTexture(Auxiliary2OutTexture, Auxiliary2Texture, FRHICopyTextureInfo());
        }
    );
}
