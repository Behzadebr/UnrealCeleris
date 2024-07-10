#include "Pass3_BousHandler.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "RenderGraphUtils.h"
#include "RHICommandList.h"
#include "Engine/TextureRenderTarget2D.h"

class FPass3_BousComputeShader : public FGlobalShader
{
    DECLARE_GLOBAL_SHADER(FPass3_BousComputeShader)
    SHADER_USE_PARAMETER_STRUCT(FPass3_BousComputeShader, FGlobalShader)

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txState)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txBottom)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txH)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txXFlux)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txYFlux)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, oldGradients)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, oldOldGradients)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, predictedGradients)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, F_G_star_oldOldGradients)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txstateUVstar)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txShipPressure)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, txNewState)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, dU_by_dt)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, F_G_star)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, current_stateUVstar)
        SHADER_PARAMETER(int, width)
        SHADER_PARAMETER(int, height)
        SHADER_PARAMETER(float, dt)
        SHADER_PARAMETER(float, dx)
        SHADER_PARAMETER(float, dy)
        SHADER_PARAMETER(float, one_over_dx)
        SHADER_PARAMETER(float, one_over_dy)
        SHADER_PARAMETER(float, g_over_dx)
        SHADER_PARAMETER(float, g_over_dy)
        SHADER_PARAMETER(int, timeScheme)
        SHADER_PARAMETER(float, epsilon)
        SHADER_PARAMETER(int, isManning)
        SHADER_PARAMETER(float, g)
        SHADER_PARAMETER(float, friction)
        SHADER_PARAMETER(int, pred_or_corrector)
        SHADER_PARAMETER(float, Bcoef)
        SHADER_PARAMETER(float, Bcoef_g)
        SHADER_PARAMETER(float, one_over_d2x)
        SHADER_PARAMETER(float, one_over_d3x)
        SHADER_PARAMETER(float, one_over_d2y)
        SHADER_PARAMETER(float, one_over_d3y)
        SHADER_PARAMETER(float, one_over_dxdy)
        SHADER_PARAMETER(float, seaLevel)
    END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FPass3_BousComputeShader, "/Celeris2024/Pass3_Bous.usf", "MainCS", SF_Compute)

static FPass3_BousComputeShader::FParameters Pass3_BousParameters;

void Pass3_BousHandler::Setup(int width, int height, float dt, float dx, float dy, float one_over_dx, float one_over_dy, float g_over_dx, float g_over_dy, int timeScheme, float epsilon, int isManning, float g, float friction, int pred_or_corrector, float Bcoef, float Bcoef_g, float one_over_d2x, float one_over_d3x, float one_over_d2y, float one_over_d3y, float one_over_dxdy, float seaLevel)
{
    Pass3_BousParameters.width = width;
    Pass3_BousParameters.height = height;
    Pass3_BousParameters.dt = dt;
    Pass3_BousParameters.dx = dx;
    Pass3_BousParameters.dy = dy;
    Pass3_BousParameters.one_over_dx = one_over_dx;
    Pass3_BousParameters.one_over_dy = one_over_dy;
    Pass3_BousParameters.g_over_dx = g_over_dx;
    Pass3_BousParameters.g_over_dy = g_over_dy;
    Pass3_BousParameters.timeScheme = timeScheme;
    Pass3_BousParameters.epsilon = epsilon;
    Pass3_BousParameters.isManning = isManning;
    Pass3_BousParameters.g = g;
    Pass3_BousParameters.friction = friction;
    Pass3_BousParameters.pred_or_corrector = pred_or_corrector;
    Pass3_BousParameters.Bcoef = Bcoef;
    Pass3_BousParameters.Bcoef_g = Bcoef_g;
    Pass3_BousParameters.one_over_d2x = one_over_d2x;
    Pass3_BousParameters.one_over_d3x = one_over_d3x;
    Pass3_BousParameters.one_over_d2y = one_over_d2y;
    Pass3_BousParameters.one_over_d3y = one_over_d3y;
    Pass3_BousParameters.one_over_dxdy = one_over_dxdy;
    Pass3_BousParameters.seaLevel = seaLevel;
}

void Pass3_BousHandler::Execute(UTextureRenderTarget2D* txState, UTextureRenderTarget2D* txBottom, UTextureRenderTarget2D* txH, UTextureRenderTarget2D* txXFlux, UTextureRenderTarget2D* txYFlux, UTextureRenderTarget2D* oldGradients, UTextureRenderTarget2D* oldOldGradients, UTextureRenderTarget2D* predictedGradients, UTextureRenderTarget2D* F_G_star_oldOldGradients, UTextureRenderTarget2D* txstateUVstar, UTextureRenderTarget2D* txShipPressure, UTextureRenderTarget2D* txNewState, UTextureRenderTarget2D* dU_by_dt, UTextureRenderTarget2D* F_G_star, UTextureRenderTarget2D* current_stateUVstar)
{
    ENQUEUE_RENDER_COMMAND(FPass3_BousComputeShader)(
        [txState, txBottom, txH, txXFlux, txYFlux, oldGradients, oldOldGradients, predictedGradients, F_G_star_oldOldGradients, txstateUVstar, txShipPressure, txNewState, dU_by_dt, F_G_star, current_stateUVstar](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder GraphBuilder(RHICmdList);
            FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

            FRDGTextureRef txStateRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txState->GetRenderTargetResource()->TextureRHI, TEXT("txState")));
            FRDGTextureRef txBottomRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txBottom->GetRenderTargetResource()->TextureRHI, TEXT("txBottom")));
            FRDGTextureRef txHRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txH->GetRenderTargetResource()->TextureRHI, TEXT("txH")));
            FRDGTextureRef txXFluxRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txXFlux->GetRenderTargetResource()->TextureRHI, TEXT("txXFlux")));
            FRDGTextureRef txYFluxRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txYFlux->GetRenderTargetResource()->TextureRHI, TEXT("txYFlux")));
            FRDGTextureRef oldGradientsRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(oldGradients->GetRenderTargetResource()->TextureRHI, TEXT("oldGradients")));
            FRDGTextureRef oldOldGradientsRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(oldOldGradients->GetRenderTargetResource()->TextureRHI, TEXT("oldOldGradients")));
            FRDGTextureRef predictedGradientsRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(predictedGradients->GetRenderTargetResource()->TextureRHI, TEXT("predictedGradients")));
            FRDGTextureRef F_G_star_oldOldGradientsRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(F_G_star_oldOldGradients->GetRenderTargetResource()->TextureRHI, TEXT("F_G_star_oldOldGradients")));
            FRDGTextureRef txstateUVstarRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txstateUVstar->GetRenderTargetResource()->TextureRHI, TEXT("txstateUVstar")));
            FRDGTextureRef txShipPressureRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txShipPressure->GetRenderTargetResource()->TextureRHI, TEXT("txShipPressure")));
            FRDGTextureRef txNewStateRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txNewState->GetRenderTargetResource()->TextureRHI, TEXT("txNewState")));
            FRDGTextureRef dU_by_dtRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(dU_by_dt->GetRenderTargetResource()->TextureRHI, TEXT("dU_by_dt")));
            FRDGTextureRef F_G_starRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(F_G_star->GetRenderTargetResource()->TextureRHI, TEXT("F_G_star")));
            FRDGTextureRef current_stateUVstarRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(current_stateUVstar->GetRenderTargetResource()->TextureRHI, TEXT("current_stateUVstar")));

            Pass3_BousParameters.txState = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txStateRDG));
            Pass3_BousParameters.txBottom = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txBottomRDG));
            Pass3_BousParameters.txH = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txHRDG));
            Pass3_BousParameters.txXFlux = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txXFluxRDG));
            Pass3_BousParameters.txYFlux = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txYFluxRDG));
            Pass3_BousParameters.oldGradients = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(oldGradientsRDG));
            Pass3_BousParameters.oldOldGradients = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(oldOldGradientsRDG));
            Pass3_BousParameters.predictedGradients = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(predictedGradientsRDG));
            Pass3_BousParameters.F_G_star_oldOldGradients = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(F_G_star_oldOldGradientsRDG));
            Pass3_BousParameters.txstateUVstar = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txstateUVstarRDG));
            Pass3_BousParameters.txShipPressure = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txShipPressureRDG));
            Pass3_BousParameters.txNewState = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(txNewStateRDG));
            Pass3_BousParameters.dU_by_dt = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(dU_by_dtRDG));
            Pass3_BousParameters.F_G_star = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(F_G_starRDG));
            Pass3_BousParameters.current_stateUVstar = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(current_stateUVstarRDG));

            TShaderMapRef<FPass3_BousComputeShader> ComputeShader(GlobalShaderMap);
            FComputeShaderUtils::AddPass(
                GraphBuilder,
                RDG_EVENT_NAME("Pass3_BousComputeShader"),
                ComputeShader,
                &Pass3_BousParameters,
                FIntVector(txState->SizeX / 32, txState->SizeY / 32, 1)
            );

            GraphBuilder.Execute();
        }
    );

    // Copy dU_by_dt to predictedGradients
    ENQUEUE_RENDER_COMMAND(CopyPredictedGradients)(
        [dU_by_dt, predictedGradients](FRHICommandListImmediate& RHICmdList)
        {
            FRHITexture* dU_by_dtTexture = dU_by_dt->GetRenderTargetResource()->TextureRHI.GetReference();
            FRHITexture* PredictedGradientsTexture = predictedGradients->GetRenderTargetResource()->TextureRHI.GetReference();
            RHICmdList.CopyTexture(dU_by_dtTexture, PredictedGradientsTexture, FRHICopyTextureInfo());
        }
    );
}