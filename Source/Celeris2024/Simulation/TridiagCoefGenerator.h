#pragma once

#include "CoreMinimal.h"

/**
 * Class responsible for generating tridiagonal coefficients for simulation grids.
 */
class CELERIS2024_API TridiagCoefGenerator
{
public:
    // Generates tridiagonal coefficients in the x direction
    static TArray<FVector4> GenerateTridiagCoefX(int32 width, int32 height, float Bcoef, float Dx, const TArray<FVector4>& BottomSurface);

    // Generates tridiagonal coefficients in the y direction
    static TArray<FVector4> GenerateTridiagCoefY(int32 width, int32 height, float Bcoef, float Dy, const TArray<FVector4>& BottomSurface);
};
