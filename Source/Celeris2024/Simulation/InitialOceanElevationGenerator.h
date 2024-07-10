#pragma once

#include "CoreMinimal.h"

/**
 * Designed to generate initial data for an ocean's elevation.
 */
class CELERIS2024_API InitialOceanElevationGenerator
{
public:
    static TArray<FVector4> GenerateInitialOceanElevation(int32 width, int32 height, float CenterX, float CenterY, float amplitude, const TArray<FVector4>& BottomSurface);
};
