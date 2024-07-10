#include "TridiagCoefGenerator.h"

TArray<FVector4> TridiagCoefGenerator::GenerateTridiagCoefX(int32 width, int32 height, float Bcoef, float Dx, const TArray<FVector4>& BottomSurface)
{
    TArray<FVector4> Data;
    Data.Reserve(width * height);

    for (int32 y = 0; y < height; ++y)
    {
        for (int32 x = 0; x < width; ++x)
        {
            float A, B, C;
            int32 idxHere = y * width + x;
            int32 idxPlus = y * width + FMath::Min(x + 1, width - 1);
            int32 idxMinus = y * width + FMath::Max(x - 1, 0);
            float depthHere = -BottomSurface[idxHere].Z;
            float depthPlus = -BottomSurface[idxPlus].Z;
            float depthMinus = -BottomSurface[idxMinus].Z;
            float Ddx = (depthPlus - depthMinus) / (2.0f * Dx);

            if (x <= 2 || x >= width - 3 || BottomSurface[idxHere].W < 0) // Also when near dry
            {
                A = 0.0f;
                B = 1.0f;
                C = 0.0f;
            }
            else
            {
                A = depthHere * Ddx / (6.0f * Dx) - (Bcoef + 1.0f / 3.0f) * depthHere * depthHere / (Dx * Dx);
                B = 1.0f + 2.0f * (Bcoef + 1.0f / 3.0f) * depthHere * depthHere / (Dx * Dx);
                C = -depthHere * Ddx / (6.0f * Dx) - (Bcoef + 1.0f / 3.0f) * depthHere * depthHere / (Dx * Dx);
            }

            Data.Add(FVector4(A, B, C, 0.0f));
        }
    }

    return Data;
}

TArray<FVector4> TridiagCoefGenerator::GenerateTridiagCoefY(int32 width, int32 height, float Bcoef, float Dy, const TArray<FVector4>& BottomSurface)
{
    TArray<FVector4> Data;
    Data.Reserve(width * height);

    for (int32 y = 0; y < height; ++y)
    {
        for (int32 x = 0; x < width; ++x)
        {
            float A, B, C;
            int32 idxHere = y * width + x;
            int32 idxPlus = (FMath::Min(y + 1, height - 1) * width) + x;
            int32 idxMinus = (FMath::Max(y - 1, 0) * width) + x;
            float depthHere = -BottomSurface[idxHere].Z;
            float depthPlus = -BottomSurface[idxPlus].Z;
            float depthMinus = -BottomSurface[idxMinus].Z;
            float Ddy = (depthPlus - depthMinus) / (2.0f * Dy);

            if (y <= 2 || y >= height - 3 || BottomSurface[idxHere].W < 0) // Also when near dry
            {
                A = 0.0f;
                B = 1.0f;
                C = 0.0f;
            }
            else
            {
                A = depthHere * Ddy / (6.0f * Dy) - (Bcoef + 1.0f / 3.0f) * depthHere * depthHere / (Dy * Dy);
                B = 1.0f + 2.0f * (Bcoef + 1.0f / 3.0f) * depthHere * depthHere / (Dy * Dy);
                C = -depthHere * Ddy / (6.0f * Dy) - (Bcoef + 1.0f / 3.0f) * depthHere * depthHere / (Dy * Dy);
            }

            Data.Add(FVector4(A, B, C, 0.0f));
        }
    }

    return Data;
}
