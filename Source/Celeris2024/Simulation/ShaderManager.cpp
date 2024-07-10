#include "ShaderManager.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "RenderGraphUtils.h"
#include "RHICommandList.h"
#include "Engine/TextureRenderTarget2D.h"

class FMultiplyComputeShader : public FGlobalShader
{
    DECLARE_GLOBAL_SHADER(FMultiplyComputeShader)
    SHADER_USE_PARAMETER_STRUCT(FMultiplyComputeShader, FGlobalShader)

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, OutputTexture)
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, InputTexture)
        SHADER_PARAMETER(float, Theta)
    END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FMultiplyComputeShader, "/Celeris2024/MultiplyShader.usf", "MainCS", SF_Compute)

void ShaderManager::ExecuteComputeShader(UTextureRenderTarget2D* InputTexture, UTextureRenderTarget2D* OutputTexture, float Theta)
{
    ENQUEUE_RENDER_COMMAND(FMultiplyComputeShader)(
        [InputTexture, OutputTexture, Theta](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder GraphBuilder(RHICmdList);
            FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

            FRDGTextureRef InputTextureRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(InputTexture->GetRenderTargetResource()->TextureRHI, TEXT("InputTexture")));
            FRDGTextureRef OutputTextureRDG = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(OutputTexture->GetRenderTargetResource()->TextureRHI, TEXT("OutputTexture")));

            FMultiplyComputeShader::FParameters* PassParameters = GraphBuilder.AllocParameters<FMultiplyComputeShader::FParameters>();
            PassParameters->InputTexture = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(InputTextureRDG));
            PassParameters->OutputTexture = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(OutputTextureRDG));
            PassParameters->Theta = Theta;

            TShaderMapRef<FMultiplyComputeShader> ComputeShader(GlobalShaderMap);
            FComputeShaderUtils::AddPass(
                GraphBuilder,
                RDG_EVENT_NAME("MultiplyComputeShader"),
                ComputeShader,
                PassParameters,
                FIntVector(InputTexture->SizeX / 32, InputTexture->SizeY / 32, 1)
            );

            GraphBuilder.Execute();
        }
    );
}
