#include "Pass2Handler.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "RenderGraphUtils.h"
#include "RHICommandList.h"
#include "Engine/TextureRenderTarget2D.h"

class FPass2ComputeShader : public FGlobalShader
{
    DECLARE_GLOBAL_SHADER(FPass2ComputeShader)
    SHADER_USE_PARAMETER_STRUCT(FPass2ComputeShader, FGlobalShader)

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txH)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txU)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txV)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txBottom)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, txC)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, txXFlux)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, txYFlux)
        SHADER_PARAMETER(int, width)
        SHADER_PARAMETER(int, height)
        SHADER_PARAMETER(float, g)
        SHADER_PARAMETER(float, half_g)
        SHADER_PARAMETER(float, dx)
        SHADER_PARAMETER(float, dy)
    END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FPass2ComputeShader, "/Celeris2024/Pass2.usf", "MainCS", SF_Compute)

static FPass2ComputeShader::FParameters Pass2Parameters;

void Pass2Handler::Setup(int width, int height, float g, float half_g, float dx, float dy)
{
    Pass2Parameters.width = width;
    Pass2Parameters.height = height;
    Pass2Parameters.g = g;
    Pass2Parameters.half_g = half_g;
    Pass2Parameters.dx = dx;
    Pass2Parameters.dy = dy;
}

void Pass2Handler::Execute(UTextureRenderTarget2D* txH, UTextureRenderTarget2D* txU, UTextureRenderTarget2D* txV, UTextureRenderTarget2D* txBottom, UTextureRenderTarget2D* txC, 
                           UTextureRenderTarget2D* txXFlux, UTextureRenderTarget2D* txYFlux)
{
    ENQUEUE_RENDER_COMMAND(FPass2ComputeShader)(
        [txH, txU, txV, txBottom, txC, txXFlux, txYFlux](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder GraphBuilder(RHICmdList);
            FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

            FRDGTextureRef txHRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txH->GetRenderTargetResource()->TextureRHI, TEXT("txH")));
            FRDGTextureRef txURDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txU->GetRenderTargetResource()->TextureRHI, TEXT("txU")));
            FRDGTextureRef txVRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txV->GetRenderTargetResource()->TextureRHI, TEXT("txV")));
            FRDGTextureRef txBottomRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txBottom->GetRenderTargetResource()->TextureRHI, TEXT("txBottom")));
            FRDGTextureRef txCRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txC->GetRenderTargetResource()->TextureRHI, TEXT("txC")));

            FRDGTextureRef txXFluxRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txXFlux->GetRenderTargetResource()->TextureRHI, TEXT("txXFlux")));
            FRDGTextureRef txYFluxRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(txYFlux->GetRenderTargetResource()->TextureRHI, TEXT("txYFlux")));

            Pass2Parameters.txH = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txHRDG));
            Pass2Parameters.txU = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txURDG));
            Pass2Parameters.txV = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txVRDG));
            Pass2Parameters.txBottom = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txBottomRDG));
            Pass2Parameters.txC = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(txCRDG));

            Pass2Parameters.txXFlux = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(txXFluxRDG));
            Pass2Parameters.txYFlux = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(txYFluxRDG));

            TShaderMapRef<FPass2ComputeShader> ComputeShader(GlobalShaderMap);
            FComputeShaderUtils::AddPass(
                GraphBuilder,
                RDG_EVENT_NAME("Pass2ComputeShader"),
                ComputeShader,
                &Pass2Parameters,
                FIntVector(txH->SizeX / 32, txH->SizeY / 32, 1)
            );

            GraphBuilder.Execute();
        }
    );
}
