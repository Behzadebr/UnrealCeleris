#pragma once

#include "GameFramework/Actor.h"
#include "SimulationParameters.h"
#include "Engine/TextureRenderTarget2D.h"
#include "TextureManager.h"
#include "RenderTargetSaver.h"
#include "Simulator.generated.h"

UCLASS()
class CELERIS2024_API ASimulator : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ASimulator();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Property to hold the simulation parameters
    UPROPERTY(EditAnywhere, Category="Simulation")
    USimulationParameters* SimulationParams;

private:
    // Member variable to hold the TextureManager instance
    TextureManager* TextureMgr;

    // Frame count and total time
    int32 FrameCount;
    float TotalTime;

    // Initial time
    FDateTime StartTime;

    // RenderTargetSaver instance
    RenderTargetSaver* Saver;

    // Plane to display the texture
    UPROPERTY()
    UStaticMeshComponent* DisplayPlane;

    // Dynamic material instance
    UPROPERTY()
    UMaterialInstanceDynamic* DynamicMaterialInstance;
};
