#include "ConstantsCalculator.h"
#include <cmath> // For std::sqrt, std::pow, and std::log

void ConstantsCalculator::CalculateConstants(USimulationParameters* SimParams)
{
    if (!SimParams)
    {
        UE_LOG(LogTemp, Warning, TEXT("SimParams is null."));
        return;
    }

    // Time step is calculated based on Courant number, dx, gravity and base depth.
    SimParams->dt = SimParams->Courant_num * SimParams->dx / FMath::Sqrt(SimParams->g * SimParams->base_depth);
    SimParams->TWO_THETA = SimParams->Theta * 2.0f;
    SimParams->half_g = SimParams->g / 2.0f;
    SimParams->Bcoef_g = SimParams->Bcoef * SimParams->g;
    SimParams->g_over_dx = SimParams->g / SimParams->dx;
    SimParams->g_over_dy = SimParams->g / SimParams->dy;
    SimParams->one_over_dx = 1.0f / SimParams->dx;
    SimParams->one_over_dy = 1.0f / SimParams->dy;
    SimParams->one_over_d2x = SimParams->one_over_dx * SimParams->one_over_dx;
    SimParams->one_over_d3x = SimParams->one_over_d2x * SimParams->one_over_dx;
    SimParams->one_over_d2y = SimParams->one_over_dy * SimParams->one_over_dy;
    SimParams->one_over_d3y = SimParams->one_over_d2y * SimParams->one_over_dy;
    SimParams->one_over_dxdy = SimParams->one_over_dx * SimParams->one_over_dy;
    SimParams->epsilon = FMath::Pow(SimParams->base_depth / 1000.0f, 2.0f);
    SimParams->boundary_epsilon = SimParams->epsilon;
    SimParams->boundary_nx = SimParams->width - 1;
    SimParams->boundary_ny = SimParams->height - 1;
    SimParams->reflect_x = 2 * (SimParams->width - 3);
    SimParams->reflect_y = 2 * (SimParams->height - 3);
    SimParams->BoundaryWidth = 25.0f;
    SimParams->boundary_g = SimParams->g;
    SimParams->Px = FMath::CeilToInt(FMath::Log2(static_cast<float>(SimParams->width)));
    SimParams->Py = FMath::CeilToInt(FMath::Log2(static_cast<float>(SimParams->height)));
    SimParams->n_write_interval = FMath::CeilToInt(SimParams->write_dt / SimParams->dt);
    SimParams->write_dt = SimParams->n_write_interval * SimParams->dt;
    SimParams->n_writes = FMath::CeilToInt((SimParams->end_write_time - SimParams->start_write_time) / SimParams->write_dt) + 1;

    // Thread and dispatch size configuration
    SimParams->ThreadX = 16;
    SimParams->ThreadY = 16;
    SimParams->DispatchX = FMath::CeilToInt(static_cast<float>(SimParams->width) / SimParams->ThreadX);
    SimParams->DispatchY = FMath::CeilToInt(static_cast<float>(SimParams->height) / SimParams->ThreadY);

    // Initialize ship properties
    SimParams->numberOfShips = 10; // This could be set from JSON or another source
    SimParams->activeShipIdx = 0;
    // Ensure the ShipProperties array is of the correct size
    SimParams->ShipProperties.SetNum(SimParams->numberOfShips);
    
    for (int32 i = 0; i < SimParams->numberOfShips; ++i)
    {
        // Directly modifying the TArray elements
        SimParams->ShipProperties[i].ShipPosX += static_cast<float>(i * 50); // Separate ships for demonstration

        // Pre-calculate various coefficients for ship pressure distribution
        // Note: Unreal uses FMath for math operations, which includes functions like Pow, Sqrt, etc.
        SimParams->ShipProperties[i].ShipC1a = 1.0f / (2.0f * FMath::Pow(SimParams->ShipProperties[i].ShipLength / PI, 2.0f));
        SimParams->ShipProperties[i].ShipC1b = 1.0f / (2.0f * FMath::Pow(SimParams->ShipProperties[i].ShipWidth / PI, 2.0f));
        SimParams->ShipProperties[i].ShipC2 = -1.0f / (4.0f * FMath::Pow(SimParams->ShipProperties[i].ShipLength / PI, 2.0f)) 
                                                  + 1.0f / (4.0f * FMath::Pow(SimParams->ShipProperties[i].ShipWidth / PI, 2.0f));
        SimParams->ShipProperties[i].ShipC3a = 1.0f / (2.0f * FMath::Pow(SimParams->ShipProperties[i].ShipLength / PI, 2.0f));
        SimParams->ShipProperties[i].ShipC3b = 1.0f / (2.0f * FMath::Pow(SimParams->ShipProperties[i].ShipWidth / PI, 2.0f));
    }

    SimParams->ShipBoundaryPolygon = {
        FVector2D(550, 50),
        FVector2D(1250, 650),
        FVector2D(1600, 1300),
        FVector2D(2000, 1550),
        FVector2D(2200, 3000),
        FVector2D(3000, 3000),
        FVector2D(3000, 50)
    };
}
