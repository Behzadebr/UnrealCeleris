#include "SimulationParameters.h"

USimulationParameters::USimulationParameters()
{
    width = 800;
    height = 600;
    dx = 1.0f;
    dy = 1.0f;
    Courant_num = 0.15f;
    NLSW_or_Bous = 0;
    base_depth = 1.0f;
    g = 9.80665f;
    Theta = 1.3f;
    friction = 0.001f;
    isManning = 1;
    dissipation_threshold = 1.0f;
    WhiteWaterDecayRate = 1.0f;
    timeScheme = 2;
    seaLevel = 0.0f;
    Bcoef = 1.0f / 15.0f;
    tridiag_solve = 1;
    west_boundary_type = 0;
    east_boundary_type = 0;
    south_boundary_type = 0;
    north_boundary_type = 0;
    start_write_time = 60.0f;
    end_write_time = 120.0f;
    write_dt = 1.0f;
    n_disp_interval = 100.0f;
    significant_wave_height = 0.0f;

	// Initialize hidden or non-UPROPERTY fields
    amplitude = 0.0f;
    period = 10.0f;
    direction = 0.0f;
    rand_phase = 0.0f;
	pred_or_corrector = 1;
    numberOfWaves = 0;
}