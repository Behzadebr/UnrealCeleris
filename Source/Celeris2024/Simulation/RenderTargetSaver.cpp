#include "RenderTargetSaver.h"
#include "Modules/ModuleManager.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Misc/FileHelper.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2D.h"
#include "RenderUtils.h"

RenderTargetSaver::RenderTargetSaver(const FString& InSaveDirectory)
    : SaveDirectory(InSaveDirectory)
{
    if (!FPaths::DirectoryExists(SaveDirectory))
    {
        FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*SaveDirectory);
    }
}

bool RenderTargetSaver::SaveRenderTargetToFile(UTextureRenderTarget2D* RenderTarget, const FString& TextureName) const
{
    if (!RenderTarget)
    {
        return false;
    }

    FString FilePath = SaveDirectory / (TextureName + TEXT(".png"));

    FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
    FReadSurfaceDataFlags ReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX);
    ReadSurfaceDataFlags.SetLinearToGamma(false);
    
    TArray<FColor> OutBMP;
    RenderTargetResource->ReadPixels(OutBMP, ReadSurfaceDataFlags);

    FIntPoint DestSize(RenderTarget->SizeX, RenderTarget->SizeY);

    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

    if (ImageWrapper.IsValid() && ImageWrapper->SetRaw(OutBMP.GetData(), OutBMP.GetAllocatedSize(), DestSize.X, DestSize.Y, ERGBFormat::BGRA, 8))
    {
        const TArray64<uint8>& PNGData = ImageWrapper->GetCompressed(100);
        return FFileHelper::SaveArrayToFile(PNGData, *FilePath);
    }

    return false;
}
