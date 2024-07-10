#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ShipProperties.h"
#include "SimulationParameters.generated.h"

UCLASS(Blueprintable)
class CELERIS2024_API USimulationParameters : public UObject
{
	GENERATED_BODY()

public:
	// Default constructor
	USimulationParameters();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Simulation")
	int width;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Simulation")
	int height;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    float dx;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    float dy;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    float Courant_num;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    int NLSW_or_Bous;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    float base_depth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    float g;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    float Theta;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    float friction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    int isManning;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    float dissipation_threshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    float WhiteWaterDecayRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    int timeScheme;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    float seaLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    float Bcoef;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    int tridiag_solve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    int west_boundary_type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    int east_boundary_type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    int south_boundary_type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    int north_boundary_type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    float start_write_time;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    float end_write_time;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    float write_dt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    float n_disp_interval;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation")
    float significant_wave_height;

    // Additional properties not exposed to Blueprints or serialized
    float amplitude;
    float period;
    float direction;
    float rand_phase;
    float dt;
    float TWO_THETA;
    float half_g;
    float Bcoef_g;
    float g_over_dx;
    float g_over_dy;
    float one_over_dx;
    float one_over_dy;
    float one_over_d2x;
    float one_over_d3x;
    float one_over_d2y;
    float one_over_d3y;
    float one_over_dxdy;
    float epsilon;
    float boundary_epsilon;
    int boundary_nx;
    int boundary_ny;
    int reflect_x;
    int reflect_y;
    float BoundaryWidth;
    float boundary_g;
    int pred_or_corrector;
    int numberOfWaves;
    int Px;
    int Py;
    int n_writes;
    int n_write_interval;
    TArray<FVector4> waveVectors;
    TArray<FVector4> waves;

    int ThreadX;
    int ThreadY;
    int DispatchX;
    int DispatchY;

    int numberOfShips;
    int activeShipIdx;

    // Ship properties array
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship")
    TArray<FShipProperties> ShipProperties;

    TArray<FVector2D> ShipBoundaryPolygon;
};
