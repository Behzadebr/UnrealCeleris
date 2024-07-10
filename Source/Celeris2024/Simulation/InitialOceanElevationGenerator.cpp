#include "InitialOceanElevationGenerator.h"

TArray<FVector4> InitialOceanElevationGenerator::GenerateInitialOceanElevation(int32 width, int32 height, float CenterX, float CenterY, float amplitude, const TArray<FVector4>& BottomSurface)
{
    TArray<FVector4> Data;
    Data.Reserve(width * height);

    float Sigma = 24.0f; // Adjusts the spread of the initial elevation

    for (int32 y = 0; y < height; ++y)
    {
        for (int32 x = 0; x < width; ++x)
        {
            int32 idx = y * width + x;
            int32 idxRight = y * width + FMath::Min(x + 1, width - 1);
            int32 idxLeft = y * width + FMath::Max(x - 1, 0);
            int32 idxUp = FMath::Min(y + 1, height - 1) * width + x;
            int32 idxDown = FMath::Max(y - 1, 0) * width + x;

            float dx = x - CenterX;
            float dy = y - CenterY;

            float bottom = BottomSurface[idx].Z;
            float bottomRight = BottomSurface[idxRight].Z;
            float bottomLeft = BottomSurface[idxLeft].Z;
            float bottomUp = BottomSurface[idxUp].Z;
            float bottomDown = BottomSurface[idxDown].Z;

            float elevCorrection = 0.0f * (FMath::Max(bottomRight, 0.0f) + FMath::Max(bottomLeft, 0.0f) + FMath::Max(bottomUp, 0.0f) + FMath::Max(bottomDown, 0.0f));
            float elevation = elevCorrection + 0.0f * FMath::Exp(-(dx * dx + dy * dy) / (2 * Sigma * Sigma));

            if (elevation < bottom || bottom > 0)
            {
                elevation = bottom;
            }

            Data.Add(FVector4(elevation, 0.0f, 0.0f, 0.0f)); // Store elevation, x-velocity, y-velocity, scalar transport
        }
    }

    return Data;
}
