#include "WaveLoader.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void WaveLoader::LoadIrrWaves(USimulationParameters* SimParams, const FString& FilePath, float RotationAngle)
{
    if (!SimParams)
    {
        UE_LOG(LogTemp, Error, TEXT("SimulationParameters is null."));
        return;
    }

    FString FullPath = FPaths::ProjectContentDir() + FilePath;
    TArray<FString> Lines;
    
    if (!FFileHelper::LoadFileToStringArray(Lines, *FullPath))
    {
        UE_LOG(LogTemp, Error, TEXT("Could not find IrrWaves file at %s"), *FullPath);
        return;
    }
    
    if (Lines.Num() < 2)
    {
        UE_LOG(LogTemp, Error, TEXT("IrrWaves file at %s is not in the correct format."), *FullPath);
        return;
    }

    // Parse the number of waves
    FString NwavesStr = Lines[1].Mid(16).TrimStartAndEnd(); // Correctly extracts the substring after "[numberOfWaves]"
    int32 numberOfWaves = FCString::Atoi(*NwavesStr); // Corrected usage

    // Update the number of waves in SimParams
    SimParams->numberOfWaves = numberOfWaves;

    SimParams->waveVectors.Empty();
    SimParams->waveVectors.Reserve(numberOfWaves);

    for (int32 i = 0; i < numberOfWaves; ++i)
    {
        TArray<FString> WaveDataStr;
        Lines[i + 3].ParseIntoArray(WaveDataStr, TEXT(" "), true);

        if (WaveDataStr.Num() < 4)
        {
            UE_LOG(LogTemp, Error, TEXT("IrrWaves file at %s is not in the correct format."), *FullPath);
            return;
        }

        FVector4 WaveVector(
            FCString::Atof(*WaveDataStr[0]), // amplitude
            FCString::Atof(*WaveDataStr[1]), // period
            FMath::Fmod(FCString::Atof(*WaveDataStr[2]) + RotationAngle, 360.0f), // Theta, adjusted for rotation
            FCString::Atof(*WaveDataStr[3])  // Phase
        );

        SimParams->waveVectors.Add(WaveVector);
    }
}
