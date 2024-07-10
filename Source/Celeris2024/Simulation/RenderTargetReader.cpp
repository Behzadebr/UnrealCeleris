#include "RenderTargetReader.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Rendering/Texture2DResource.h"

void RenderTargetReader::ReadAndLogValueAtTexel(UTextureRenderTarget2D* RenderTarget, int32 TexelX, int32 TexelY)
{
    if (!RenderTarget)
    {
        UE_LOG(LogTemp, Warning, TEXT("RenderTarget is null."));
        return;
    }

    FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
    if (RenderTargetResource)
    {
        TArray<FLinearColor> OutData;  // Use FLinearColor for 32-bit floats
        FReadSurfaceDataFlags ReadSurfaceDataFlags;
        ReadSurfaceDataFlags.SetLinearToGamma(false); // Keep the data in linear space

        // Read the pixel data into OutData array
        if (RenderTargetResource->ReadLinearColorPixels(OutData))
        {
            const int32 Index = TexelY * RenderTarget->SizeX + TexelX;
            if (OutData.IsValidIndex(Index))
            {
                const FLinearColor& PixelColor = OutData[Index];
                // Log the floating-point values
                FString LogMessage = FString::Printf(TEXT("Pixel (%d, %d) Floating-Point Values: R=%.10f, G=%.10f, B=%.10f, A=%.10f"),
                                     TexelX, TexelY,
                                     PixelColor.R, PixelColor.G, PixelColor.B, PixelColor.A);
                UE_LOG(LogTemp, Log, TEXT("%s"), *LogMessage);

            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Texel coordinates out of bounds."));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to read floating-point pixels from the render target."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to get render target resource."));
    }
}
