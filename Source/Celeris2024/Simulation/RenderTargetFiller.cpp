#include "RenderTargetFiller.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RHI.h"
#include "RenderGraphUtils.h"

void RenderTargetFiller::FillRenderTarget(UTextureRenderTarget2D* RenderTarget, const TArray<FVector4>& Data)
{
    if (!RenderTarget)
    {
        UE_LOG(LogTemp, Warning, TEXT("RenderTarget is null."));
        return;
    }

    int32 width = RenderTarget->SizeX;
    int32 height = RenderTarget->SizeY;

    if (Data.Num() != width * height)
    {
        UE_LOG(LogTemp, Warning, TEXT("Data size does not match the render target dimensions."));
        return;
    }

    FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
    TArray<FLinearColor> TextureData;
    TextureData.SetNumUninitialized(width * height);

    for (int32 i = 0; i < Data.Num(); ++i)
    {
        const FVector4& Vec = Data[i];
        TextureData[i] = FLinearColor(FLinearColor(Vec.X, Vec.Y, Vec.Z, Vec.W));
    }

    ENQUEUE_RENDER_COMMAND(UpdateRenderTargetCommand)(
        [RenderTargetResource, TextureData, width, height](FRHICommandListImmediate& RHICmdList)
        {
            FRHITexture2D* Texture2D = RenderTargetResource->GetRenderTargetTexture();
            if (!Texture2D)
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to get render target texture."));
                return;
            }

            FUpdateTextureRegion2D UpdateRegion(0, 0, 0, 0, width, height);
            RHICmdList.UpdateTexture2D(
                Texture2D,
                0,
                UpdateRegion,
                width * sizeof(FLinearColor),
                (uint8*)TextureData.GetData()
            );

            RHICmdList.ImmediateFlush(EImmediateFlushType::FlushRHIThread);
        });

    // Ensure the commands are executed
    FlushRenderingCommands();
}
