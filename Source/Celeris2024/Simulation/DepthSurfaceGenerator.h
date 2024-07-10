#pragma once

#include "CoreMinimal.h"

/**
 * Generates a depth surface based on depth data loaded from a file.
 */
class CELERIS2024_API DepthSurfaceGenerator
{
public:
    static TArray<FVector4> GenerateDepthSurface(const FString& FilePath, int32 width, int32 height, float base_depth);
};
