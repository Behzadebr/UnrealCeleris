#include "DepthDataBoundaryHandler.h"

void DepthDataBoundaryHandler::HandleBoundary(TArray<TArray<float>>& Bathy2D, int32 width, int32 height)
{
    // Handle boundary cells for the first two and last two columns by mirroring inner cells.
    for (int32 y = 0; y < height; ++y)
    {
        for (int32 x = 0; x < 2; ++x)
        {
            Bathy2D[y][x] = Bathy2D[y][4 - x];
        }

        for (int32 x = width - 2; x < width; ++x)
        {
            Bathy2D[y][x] = Bathy2D[y][2 * (width - 3) - x];
        }
    }

    // Handle boundary cells for the first two and last two rows by mirroring inner cells.
    for (int32 x = 0; x < width; ++x)
    {
        for (int32 y = 0; y < 2; ++y)
        {
            Bathy2D[y][x] = Bathy2D[4 - y][x];
        }

        for (int32 y = height - 2; y < height; ++y)
        {
            Bathy2D[y][x] = Bathy2D[2 * (height - 3) - y][x];
        }
    }
}
