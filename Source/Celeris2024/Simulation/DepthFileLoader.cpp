#include "DepthFileLoader.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

TArray<TArray<float>> DepthFileLoader::LoadDepth(const FString& FilePath, int32 width, int32 height)
{
    TArray<TArray<float>> DepthData;
    DepthData.SetNum(height);

    FString FullPath = FPaths::ProjectContentDir() / "Data" / FilePath;
    FString FileContent;
    
    if (FFileHelper::LoadFileToString(FileContent, *FullPath))
    {
        TArray<FString> Lines;
        FileContent.ParseIntoArrayLines(Lines);
        
        for (int32 i = 0; i < Lines.Num() && i < height; ++i)
        {
            TArray<FString> DepthValues;
            Lines[i].ParseIntoArray(DepthValues, TEXT(" "), true);
            
            DepthData[i].SetNum(width);
            for (int32 j = 0; j < DepthValues.Num() && j < width; ++j)
            {
                float Value = FCString::Atof(*DepthValues[j]);
                DepthData[i][j] = Value;
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Could not find depth file at %s"), *FullPath);
    }

    return DepthData;
}
