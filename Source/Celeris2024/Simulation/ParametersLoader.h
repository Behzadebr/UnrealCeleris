#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SimulationParameters.h"
#include "ParametersLoader.generated.h"

UCLASS()
class CELERIS2024_API UParametersLoader : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    static bool LoadParameters(USimulationParameters* SimulationParametersObject, const FString& JsonFilePath);
};
