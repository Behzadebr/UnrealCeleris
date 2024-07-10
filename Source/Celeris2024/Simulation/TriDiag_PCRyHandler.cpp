#include "TriDiag_PCRyHandler.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "RenderGraphUtils.h"
#include "RHICommandList.h"
#include "Engine/TextureRenderTarget2D.h"

class FTriDiagPCRyComputeShader : public FGlobalShader
{
    DECLARE_GLOBAL_SHADER(FTriDiagPCRyComputeShader)
    SHADER_USE_PARAMETER_STRUCT(FTriDiagPCRyComputeShader, FGlobalShader)

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, coefMaty)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, current_state)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, current_stateUVstar)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, newcoefy)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, txNewState)
        SHADER_PARAMETER(int, width)
        SHADER_PARAMETER(int, height)
        SHADER_PARAMETER(float, p)
        SHADER_PARAMETER(float, s)
    END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FTriDiagPCRyComputeShader, "/Celeris2024/TriDiag_PCRy.usf", "MainCS", SF_Compute)

static FTriDiagPCRyComputeShader::FParameters TriDiagPCRyParameters;

void TriDiag_PCRyHandler::Setup(int width, int height)
{
    TriDiagPCRyParameters.width = width;
    TriDiagPCRyParameters.height = height;
}

void TriDiag_PCRyHandler::Execute(UTextureRenderTarget2D* coefMaty, UTextureRenderTarget2D* newcoef, UTextureRenderTarget2D* txtemp, UTextureRenderTarget2D* txtemp2, UTextureRenderTarget2D* current_stateUVstar, UTextureRenderTarget2D* txNewState, int Py, int NLSW_or_Bous)
{
    if (NLSW_or_Bous == 0)
    {
        // Copy current_stateUVstar to txNewState
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
        for (int p = 0; p < Py; p++)
        {
            float s = 1 << p;

            TriDiagPCRyParameters.p = p;
            TriDiagPCRyParameters.s = s;

            ENQUEUE_RENDER_COMMAND(FTriDiagPCRyComputeShader)(
                [coefMaty, newcoef, txtemp, txtemp2, current_stateUVstar, txNewState, p, s](FRHICommandListImmediate& RHICmdList)
                {
                    FRDGBuilder GraphBuilder(RHICmdList);
                    FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

                    FRDGTextureRef coefMatyRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(coefMaty->GetRenderTargetResource()->TextureRHI, TEXT("coefMaty")));
                    FRDGTextureRef currentStateRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txNewState->GetRenderTargetResource()->TextureRHI, TEXT("current_state")));
                    FRDGTextureRef currentStateUVstarRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(current_stateUVstar->GetRenderTargetResource()->TextureRHI, TEXT("current_stateUVstar")));
                    FRDGTextureRef newcoefyRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(newcoef->GetRenderTargetResource()->TextureRHI, TEXT("newcoefy")));
                    FRDGTextureRef txNewStateRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txtemp2->GetRenderTargetResource()->TextureRHI, TEXT("txNewState")));

                    TriDiagPCRyParameters.coefMaty = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(coefMatyRDG));
                    TriDiagPCRyParameters.current_state = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(currentStateRDG));
                    TriDiagPCRyParameters.current_stateUVstar = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(currentStateUVstarRDG));
                    TriDiagPCRyParameters.newcoefy = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(newcoefyRDG));
                    TriDiagPCRyParameters.txNewState = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(txNewStateRDG));

                    TShaderMapRef<FTriDiagPCRyComputeShader> ComputeShader(GlobalShaderMap);
                    FComputeShaderUtils::AddPass(
                        GraphBuilder,
                        RDG_EVENT_NAME("TriDiag_PCRyComputeShader"),
                        ComputeShader,
                        &TriDiagPCRyParameters,
                        FIntVector(coefMaty->SizeX / 32, coefMaty->SizeY / 32, 1)
                    );

                    GraphBuilder.Execute();
                }
            );

            // Copy new textures to old ones only if the loop counter is less than Py - 1.
            if (p < Py - 1)
            {
                ENQUEUE_RENDER_COMMAND(CopyNewcoefToCoefMaty)(
                    [newcoef, coefMaty](FRHICommandListImmediate& RHICmdList)
                    {
                        if (newcoef->GetRenderTargetResource()->TextureRHI != coefMaty->GetRenderTargetResource()->TextureRHI)
                        {
                            FRHITexture* NewcoefTexture = newcoef->GetRenderTargetResource()->TextureRHI.GetReference();
                            FRHITexture* CoefMatyTexture = coefMaty->GetRenderTargetResource()->TextureRHI.GetReference();
                            RHICmdList.CopyTexture(NewcoefTexture, CoefMatyTexture, FRHICopyTextureInfo());
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
