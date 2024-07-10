#pragma once

#include "CoreMinimal.h"

/**
 * Handles the values at the boundaries of the depth data.
 */
class CELERIS2024_API DepthDataBoundaryHandler
{
public:
    static void HandleBoundary(TArray<TArray<float>>& Bathy2D, int32 width, int32 height);
};
