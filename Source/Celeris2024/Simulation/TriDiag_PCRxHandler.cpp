#include "TriDiag_PCRxHandler.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "RenderGraphUtils.h"
#include "RHICommandList.h"
#include "Engine/TextureRenderTarget2D.h"

class FTriDiagPCRxComputeShader : public FGlobalShader
{
    DECLARE_GLOBAL_SHADER(FTriDiagPCRxComputeShader)
    SHADER_USE_PARAMETER_STRUCT(FTriDiagPCRxComputeShader, FGlobalShader)

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, coefMatx)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, current_state)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, current_stateUVstar)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, newcoefx)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, txNewState)
        SHADER_PARAMETER(int, width)
        SHADER_PARAMETER(int, height)
        SHADER_PARAMETER(float, p)
        SHADER_PARAMETER(float, s)
    END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FTriDiagPCRxComputeShader, "/Celeris2024/TriDiag_PCRx.usf", "MainCS", SF_Compute)

static FTriDiagPCRxComputeShader::FParameters TriDiagPCRxParameters;

void TriDiag_PCRxHandler::Setup(int width, int height)
{
    TriDiagPCRxParameters.width = width;
    TriDiagPCRxParameters.height = height;
}

void TriDiag_PCRxHandler::Execute(UTextureRenderTarget2D* coefMatx, UTextureRenderTarget2D* newcoef, UTextureRenderTarget2D* txtemp, UTextureRenderTarget2D* txtemp2, UTextureRenderTarget2D* current_stateUVstar, UTextureRenderTarget2D* txNewState, int Px, int NLSW_or_Bous)
{
    if (NLSW_or_Bous == 0)
    {
        ENQUEUE_RENDER_COMMAND(CopyCurrentStateUVstarToTxNewState)(
            [current_stateUVstar, txNewState](FRHICommandListImmediate& RHICmdList)
            {
                FRHITexture* CurrentStateUVstarTexture = current_stateUVstar->GetRenderTargetResource()->TextureRHI.GetReference();
                FRHITexture* TxNewStateTexture = txNewState->GetRenderTargetResource()->TextureRHI.GetReference();
                RHICmdList.CopyTexture(CurrentStateUVstarTexture, TxNewStateTexture, FRHICopyTextureInfo());
            }
        );
    }
    else
    {
        for (int p = 0; p < Px; p++)
        {
            float s = 1 << p;

            TriDiagPCRxParameters.p = p;
            TriDiagPCRxParameters.s = s;

            ENQUEUE_RENDER_COMMAND(FTriDiagPCRxComputeShader)(
                [coefMatx, newcoef, txtemp, txtemp2, current_stateUVstar, txNewState, p, s](FRHICommandListImmediate& RHICmdList)
                {
                    FRDGBuilder GraphBuilder(RHICmdList);
                    FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

                    FRDGTextureRef coefMatxRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(coefMatx->GetRenderTargetResource()->TextureRHI, TEXT("coefMatx")));
                    FRDGTextureRef currentStateRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txNewState->GetRenderTargetResource()->TextureRHI, TEXT("current_state")));
                    FRDGTextureRef currentStateUVstarRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(current_stateUVstar->GetRenderTargetResource()->TextureRHI, TEXT("current_stateUVstar")));
                    FRDGTextureRef newcoefxRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(newcoef->GetRenderTargetResource()->TextureRHI, TEXT("newcoefx")));
                    FRDGTextureRef txNewStateRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txtemp2->GetRenderTargetResource()->TextureRHI, TEXT("txNewState")));

                    TriDiagPCRxParameters.coefMatx = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(coefMatxRDG));
                    TriDiagPCRxParameters.current_state = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(currentStateRDG));
                    TriDiagPCRxParameters.current_stateUVstar = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(currentStateUVstarRDG));
                    TriDiagPCRxParameters.newcoefx = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(newcoefxRDG));
                    TriDiagPCRxParameters.txNewState = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(txNewStateRDG));

                    TShaderMapRef<FTriDiagPCRxComputeShader> ComputeShader(GlobalShaderMap);
                    FComputeShaderUtils::AddPass(
                        GraphBuilder,
                        RDG_EVENT_NAME("TriDiag_PCRxComputeShader"),
                        ComputeShader,
                        &TriDiagPCRxParameters,
                        FIntVector(coefMatx->SizeX / 32, coefMatx->SizeY / 32, 1)
                    );

                    GraphBuilder.Execute();
                }
            );

            // Copy new textures to old ones only if the loop counter is less than Px - 1.
            if (p < Px - 1)
            {
                ENQUEUE_RENDER_COMMAND(CopyNewcoefToCoefMatx)(
                    [newcoef, coefMatx](FRHICommandListImmediate& RHICmdList)
                    {
                        if (newcoef->GetRenderTargetResource()->TextureRHI != coefMatx->GetRenderTargetResource()->TextureRHI)
                        {
                            FRHITexture* NewcoefTexture = newcoef->GetRenderTargetResource()->TextureRHI.GetReference();
                            FRHITexture* CoefMatxTexture = coefMatx->GetRenderTargetResource()->TextureRHI.GetReference();
                            RHICmdList.CopyTexture(NewcoefTexture, CoefMatxTexture, FRHICopyTextureInfo());
                        }
                    }
                );
            }
        }

        // After all the iterations, copy the new state into current state.
        ENQUEUE_RENDER_COMMAND(CopyTxtemp2ToTxNewState)(
            [txtemp2, txNewState](FRHICommandListImmediate& RHICmdList)
            {
                FRHITexture* Txtemp2Texture = txtemp2->GetRenderTargetResource()->TextureRHI.GetReference();
                FRHITexture* TxNewStateTexture = txNewState->GetRenderTargetResource()->TextureRHI.GetReference();
                if (Txtemp2Texture != TxNewStateTexture)
                {
                    RHICmdList.CopyTexture(Txtemp2Texture, TxNewStateTexture, FRHICopyTextureInfo());
                }
            }
        );
    }
}
