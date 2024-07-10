#pragma once

#include "CoreMinimal.h"
#include "ShipProperties.generated.h"

USTRUCT(BlueprintType)
struct FShipProperties
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship")
    float ShipPosX = 2300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship")
    float ShipPosY = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship")
    float ShipWidth = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship")
    float ShipLength = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship")
    float ShipDraft = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship")
    float ShipHeading = 1.5708f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship")
    float ShipDx = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship")
    float ShipDy = 0.0f;

    // New properties added
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship")
    float ShipC1a;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship")
    float ShipC1b;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship")
    float ShipC2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship")
    float ShipC3a;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship")
    float ShipC3b;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship")
    float ShipVelX = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship")
    float ShipVelY = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship")
    float ShipSpeed = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship")
    float LastDx = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ship")
    float LastDy = 0.0f;
};