#include "DepthSurfaceGenerator.h"
#include "DepthFileLoader.h"
#include "DepthDataBoundaryHandler.h"

TArray<FVector4> DepthSurfaceGenerator::GenerateDepthSurface(const FString& FilePath, int32 width, int32 height, float base_depth)
{
    TArray<FVector4> Data;
    Data.Reserve(width * height);

    // Assuming the depth file is stored in the Content/Data directory.
    TArray<TArray<float>> Bathy2D = DepthFileLoader::LoadDepth(FilePath, width, height);

    // Check if depth data is successfully loaded
    if (Bathy2D.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Depth data could not be loaded."));
        return Data;
    }

    // Handle the boundaries
    DepthDataBoundaryHandler::HandleBoundary(Bathy2D, width, height);

    // Populate the Data array
    for (int32 y = 0; y < height; ++y)
    {
        for (int32 x = 0; x < width; ++x)
        {
            float BN = 0.5f * Bathy2D[y][x] + 0.5f * Bathy2D[FMath::Min(height - 1, y + 1)][x];
            float BE = 0.5f * Bathy2D[y][x] + 0.5f * Bathy2D[y][FMath::Min(width - 1, x + 1)];
            float BS = 0.5f * Bathy2D[y][x] + 0.5f * Bathy2D[FMath::Max(0, y - 1)][x];
            float BW = 0.5f * Bathy2D[y][x] + 0.5f * Bathy2D[y][FMath::Max(0, x - 1)];

            FVector4 DataVec(BN, BE, Bathy2D[y][x], 99.0f); // w set to 99 as a placeholder

            Data.Add(DataVec);
        }
    }

    // Near-dry check
    int32 LengthCheck = 3;
    for (int32 y = 0; y < height; ++y)
    {
        for (int32 x = 0; x < width; ++x)
        {
            int32 idx = y * width + x;

            for (int32 yy = y - LengthCheck; yy <= y + LengthCheck; ++yy)
            {
                for (int32 xx = x - LengthCheck; xx <= x + LengthCheck; ++xx)
                {
                    int32 xC = FMath::Min(width - 1, FMath::Max(0, xx));
                    int32 yC = FMath::Min(height - 1, FMath::Max(0, yy));

                    if (Bathy2D[yC][xC] >= 0)
                    {
                        Data[idx].W = -99.0f;
                    }
                }
            }
        }
    }

    return Data;
}
