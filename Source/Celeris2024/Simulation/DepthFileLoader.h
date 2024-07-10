#pragma once

#include "CoreMinimal.h"

/**
 * Responsible for loading depth data from a file.
 */
class CELERIS2024_API DepthFileLoader
{
public:
    static TArray<TArray<float>> LoadDepth(const FString& FilePath, int32 width, int32 height);
};
