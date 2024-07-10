#pragma once

#include "CoreMinimal.h"
#include "SimulationParameters.h" // Include the header where SimulationParameters is defined

/**
 * Utility class for loading wave data from a file.
 */
class CELERIS2024_API WaveLoader
{
public:
    static void LoadIrrWaves(USimulationParameters* SimParams, const FString& FilePath, float RotationAngle);
};
