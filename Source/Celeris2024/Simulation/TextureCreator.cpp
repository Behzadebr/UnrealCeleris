#include "TextureCreator.h"
#include "Engine/World.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "RenderTargetFiller.h"

UTextureRenderTarget2D* TextureCreator::CreateInitializedRenderTarget(UObject* Outer, int32 width, int32 height, const TArray<FVector4>* Data)
{
    // Ensure we're creating a render target with HDR support and UAV flag
    UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>(Outer);
    if (!RenderTarget)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create a render target."));
        return nullptr; // Early return if the render target couldn't be created
    }

    RenderTarget->RenderTargetFormat = RTF_RGBA32f;
    RenderTarget->InitAutoFormat(width, height);
    RenderTarget->bCanCreateUAV = true; // Allow UAV creation
    RenderTarget->ClearColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Initialize the render target with the clear color
    UWorld* World = Outer->GetWorld();
    if (World)
    {
        UKismetRenderingLibrary::ClearRenderTarget2D(World, RenderTarget, RenderTarget->ClearColor);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("World is null. Render target has not been cleared."));
        return RenderTarget;
    }

    // Update the render target resource immediately
    RenderTarget->UpdateResourceImmediate(true);

    // If data is provided, use RenderTargetFiller to fill the render target
    if (Data && Data->Num() == width * height)
    {
        RenderTargetFiller::FillRenderTarget(RenderTarget, *Data);
    }
    else if (Data)
    {
        UE_LOG(LogTemp, Warning, TEXT("Data size does not match the render target dimensions."));
    }

    UE_LOG(LogTemp, Log, TEXT("Render target created and initialized: %s"), *RenderTarget->GetName());

    return RenderTarget;
}
