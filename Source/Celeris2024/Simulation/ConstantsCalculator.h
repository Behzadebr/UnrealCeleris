#pragma once

#include "CoreMinimal.h"
#include "SimulationParameters.h"

/**
 * Utility class for calculating simulation constants that are not provided in the JSON file.
 */
class CELERIS2024_API ConstantsCalculator
{
public:
    // Calculates parameters not provided in the JSON file.
    static void CalculateConstants(USimulationParameters* CalcConstants);
};

