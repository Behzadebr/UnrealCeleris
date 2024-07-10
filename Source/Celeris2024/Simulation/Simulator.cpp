#include "Simulator.h"
#include "UObject/ConstructorHelpers.h"
#include "ParametersLoader.h"
#include "ConstantsCalculator.h"
#include "DepthSurfaceGenerator.h"
#include "InitialOceanElevationGenerator.h"
#include "WaveLoader.h"
#include "TridiagCoefGenerator.h"
#include "TextureManager.h"
#include "RenderTargetReader.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "RenderTargetSaver.h"
#include "ShaderManager.h"
#include "Pass1Handler.h"
#include "Pass2Handler.h"
#include "BoundaryPassHandler.h"
#include "Pass3_NLSWHandler.h"
#include "Pass3_BousHandler.h"
#include "TriDiag_PCRxHandler.h"
#include "TriDiag_PCRyHandler.h"
#include "WaveDisplayHandler.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASimulator::ASimulator()
{
    PrimaryActorTick.bCanEverTick = true;
    TextureMgr = nullptr;
    FrameCount = 0;
    TotalTime = 0.0f;
    Saver = nullptr;

    // Create the plane to display the texture
    DisplayPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayPlane"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane"));
    if (PlaneMesh.Succeeded())
    {
        DisplayPlane->SetStaticMesh(PlaneMesh.Object);
    }
    RootComponent = DisplayPlane;
}

// Called when the game starts or when spawned
void ASimulator::BeginPlay()
{
    Super::BeginPlay();

    // Initialize simulation parameters from JSON configuration
    SimulationParams = NewObject<USimulationParameters>(this, USimulationParameters::StaticClass());
    FString JsonFilePath = FPaths::ProjectContentDir() + TEXT("Data/config.json");
    UParametersLoader::LoadParameters(SimulationParams, JsonFilePath);
    ConstantsCalculator::CalculateConstants(SimulationParams);

    // Generate the depth surface based on the modified bathymetry data
    FString DepthFilePath = FPaths::ProjectContentDir() + TEXT("Data/modified_bathy.txt");
    TArray<FVector4> DepthData = DepthSurfaceGenerator::GenerateDepthSurface(DepthFilePath, SimulationParams->width, SimulationParams->height, SimulationParams->base_depth);

    // Log the values at index 1047552
    UE_LOG(LogTemp, Log, TEXT("DepthData[%d] = (%f, %f, %f, %f)"), 1047552, DepthData[1047552].X, DepthData[1047552].Y, DepthData[1047552].Z, DepthData[1047552].W);


    // Generate initial ocean elevation using the depth data
    TArray<FVector4> OceanElevationData = InitialOceanElevationGenerator::GenerateInitialOceanElevation(SimulationParams->width, SimulationParams->height, SimulationParams->width / 2.0f, SimulationParams->height / 2.0f, 1.0f, DepthData);

    // Load irregular wave data from file
    FString IrrWavesFilePath = FPaths::ProjectContentDir() + TEXT("Data/irrWaves.txt");
    WaveLoader::LoadIrrWaves(SimulationParams, IrrWavesFilePath, 0);

    // Generate tridiagonal coefficients for both x and y directions
    TArray<FVector4> TridiagCoefXData = TridiagCoefGenerator::GenerateTridiagCoefX(SimulationParams->width, SimulationParams->height, SimulationParams->Bcoef, SimulationParams->dx, DepthData);
    TArray<FVector4> TridiagCoefYData = TridiagCoefGenerator::GenerateTridiagCoefY(SimulationParams->width, SimulationParams->height, SimulationParams->Bcoef, SimulationParams->dy, DepthData);

    // Create and initialize the texture manager
    TextureMgr = new TextureManager(this, SimulationParams);
    TextureMgr->InitializeTextures(DepthData, OceanElevationData, SimulationParams->waveVectors, TridiagCoefXData, TridiagCoefYData);

    // Initialize the RenderTargetSaver with the desired save directory
    Saver = new RenderTargetSaver(FPaths::ProjectContentDir() + TEXT("SavedTextures"));

    // Create and set up the dynamic material instance
    UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/M_TextureDisplay.M_TextureDisplay"));
    if (BaseMaterial)
    {
        DynamicMaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);
        DisplayPlane->SetMaterial(0, DynamicMaterialInstance);
    }

    // Initial execution of the compute shaders
    Pass1Handler::Setup(SimulationParams->width, SimulationParams->height, SimulationParams->one_over_dx, SimulationParams->one_over_dy, SimulationParams->dissipation_threshold, SimulationParams->TWO_THETA, SimulationParams->epsilon, SimulationParams->WhiteWaterDecayRate, SimulationParams->dt, SimulationParams->base_depth);
    Pass2Handler::Setup(SimulationParams->width, SimulationParams->height, SimulationParams->g, SimulationParams->half_g, SimulationParams->dx, SimulationParams->dy);
    
    if (SimulationParams->NLSW_or_Bous == 0)
    {
        Pass3_NLSWHandler::Setup(SimulationParams->width, SimulationParams->height, SimulationParams->dt, SimulationParams->dx, SimulationParams->dy, SimulationParams->one_over_dx, SimulationParams->one_over_dy, SimulationParams->g_over_dx, SimulationParams->g_over_dy, SimulationParams->timeScheme, SimulationParams->epsilon, SimulationParams->isManning, SimulationParams->g, SimulationParams->friction, SimulationParams->pred_or_corrector, SimulationParams->Bcoef, SimulationParams->Bcoef_g, SimulationParams->one_over_d2x, SimulationParams->one_over_d3x, SimulationParams->one_over_d2y, SimulationParams->one_over_d3y, SimulationParams->one_over_dxdy, SimulationParams->seaLevel);
    }
    else if (SimulationParams->NLSW_or_Bous == 1)
    {
        Pass3_BousHandler::Setup(SimulationParams->width, SimulationParams->height, SimulationParams->dt, SimulationParams->dx, SimulationParams->dy, SimulationParams->one_over_dx, SimulationParams->one_over_dy, SimulationParams->g_over_dx, SimulationParams->g_over_dy, SimulationParams->timeScheme, SimulationParams->epsilon, SimulationParams->isManning, SimulationParams->g, SimulationParams->friction, SimulationParams->pred_or_corrector, SimulationParams->Bcoef, SimulationParams->Bcoef_g, SimulationParams->one_over_d2x, SimulationParams->one_over_d3x, SimulationParams->one_over_d2y, SimulationParams->one_over_d3y, SimulationParams->one_over_dxdy, SimulationParams->seaLevel);
    }

    BoundaryPassHandler::Setup(SimulationParams->width, SimulationParams->height, SimulationParams->dt, SimulationParams->dx, SimulationParams->dy, PI, SimulationParams->BoundaryWidth, SimulationParams->seaLevel, SimulationParams->boundary_nx, SimulationParams->boundary_ny, SimulationParams->numberOfWaves, SimulationParams->west_boundary_type, SimulationParams->east_boundary_type, SimulationParams->south_boundary_type, SimulationParams->north_boundary_type, SimulationParams->boundary_g);
    TriDiag_PCRxHandler::Setup(SimulationParams->width, SimulationParams->height);
    TriDiag_PCRyHandler::Setup(SimulationParams->width, SimulationParams->height);

    // Setup the WaveDisplayHandler
    WaveDisplayHandler::Setup(SimulationParams->width, SimulationParams->height, SimulationParams->dx, SimulationParams->dy, SimulationParams->epsilon);
}

// Called every frame
void ASimulator::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Increment the frame count
    FrameCount++;

    // Update total time
    TotalTime = FrameCount * SimulationParams->dt;

    if (FrameCount == 1)
    {
        // Start wall clock timer
        StartTime = FDateTime::UtcNow();

        // Execute BoundaryPass
        BoundaryPassHandler::Execute(TextureMgr->current_stateUVstar, TextureMgr->txBottom, TextureMgr->txWaves, TextureMgr->txtemp, TotalTime);
    }

    //!!PREDICTOR!!

    // Execute Pass1
    Pass1Handler::Execute(TextureMgr->txState, TextureMgr->txBottom, TextureMgr->txAuxiliary2, TextureMgr->txH, TextureMgr->txU, TextureMgr->txV, TextureMgr->txNormal, TextureMgr->txAuxiliary2Out, TextureMgr->txW, TextureMgr->txC);

    // Execute Pass2
    Pass2Handler::Execute(TextureMgr->txH, TextureMgr->txU, TextureMgr->txV, TextureMgr->txBottom, TextureMgr->txC, TextureMgr->txXFlux, TextureMgr->txYFlux);

    // Pass3's
    SimulationParams->pred_or_corrector = 1; // This is used inside PassX to determine the proper State update equation to use
    if (SimulationParams->NLSW_or_Bous == 0) // NLSW
    {
        Pass3_NLSWHandler::Execute(TextureMgr->txState, TextureMgr->txBottom, TextureMgr->txH, TextureMgr->txXFlux, TextureMgr->txYFlux, TextureMgr->oldGradients, TextureMgr->oldOldGradients, TextureMgr->predictedGradients, TextureMgr->F_G_star_oldOldGradients, TextureMgr->txstateUVstar, TextureMgr->txShipPressure, TextureMgr->txNewState, TextureMgr->dU_by_dt, TextureMgr->F_G_star, TextureMgr->current_stateUVstar);
    }
    else if (SimulationParams->NLSW_or_Bous == 1) // Bous
    {
        Pass3_BousHandler::Execute(TextureMgr->txState, TextureMgr->txBottom, TextureMgr->txH, TextureMgr->txXFlux, TextureMgr->txYFlux, TextureMgr->oldGradients, TextureMgr->oldOldGradients, TextureMgr->predictedGradients, TextureMgr->F_G_star_oldOldGradients, TextureMgr->txstateUVstar, TextureMgr->txShipPressure, TextureMgr->txNewState, TextureMgr->dU_by_dt, TextureMgr->F_G_star, TextureMgr->current_stateUVstar);
    }

    // Execute BoundaryPass
    BoundaryPassHandler::Execute(TextureMgr->current_stateUVstar, TextureMgr->txBottom, TextureMgr->txWaves, TextureMgr->txtemp, TotalTime);

    // Execute TriDiag_PCRx
    TriDiag_PCRxHandler::Execute(TextureMgr->coefMatx, TextureMgr->newcoef, TextureMgr->txtemp, TextureMgr->txtemp2, TextureMgr->current_stateUVstar, TextureMgr->txNewState, SimulationParams->Px, SimulationParams->NLSW_or_Bous);

    // Execute TriDiag_PCRy
    TriDiag_PCRyHandler::Execute(TextureMgr->coefMaty, TextureMgr->newcoef, TextureMgr->txtemp, TextureMgr->txtemp2, TextureMgr->current_stateUVstar, TextureMgr->txNewState, SimulationParams->Py, SimulationParams->NLSW_or_Bous);

    //!!END PREDICTOR!!

    // Step back values of F* and G*
    TextureMgr->CopyTexture(TextureMgr->F_G_star_oldGradients, TextureMgr->F_G_star_oldOldGradients);
    TextureMgr->CopyTexture(TextureMgr->F_G_star, TextureMgr->F_G_star_oldGradients);

    //!!CORRECTOR!!
    if (SimulationParams->timeScheme == 2) // only called when using Predictor+Corrector method
    {
        SimulationParams->pred_or_corrector = 2;

        // Copy txState to txState_pred
        TextureMgr->CopyTexture(TextureMgr->txstateUVstar, TextureMgr->txStateUVstar_pred);
        TextureMgr->CopyTexture(TextureMgr->txNewState, TextureMgr->txState);

        // Execute Pass1
        Pass1Handler::Execute(TextureMgr->txState, TextureMgr->txBottom, TextureMgr->txAuxiliary2, TextureMgr->txH, TextureMgr->txU, TextureMgr->txV, TextureMgr->txNormal, TextureMgr->txAuxiliary2Out, TextureMgr->txW, TextureMgr->txC);

        // Execute Pass2
        Pass2Handler::Execute(TextureMgr->txH, TextureMgr->txU, TextureMgr->txV, TextureMgr->txBottom, TextureMgr->txC, TextureMgr->txXFlux, TextureMgr->txYFlux);

        if (SimulationParams->NLSW_or_Bous == 0) // NLSW
        {
            Pass3_NLSWHandler::Execute(TextureMgr->txState, TextureMgr->txBottom, TextureMgr->txH, TextureMgr->txXFlux, TextureMgr->txYFlux, TextureMgr->oldGradients, TextureMgr->oldOldGradients, TextureMgr->predictedGradients, TextureMgr->F_G_star_oldOldGradients, TextureMgr->txstateUVstar, TextureMgr->txShipPressure, TextureMgr->txNewState, TextureMgr->dU_by_dt, TextureMgr->F_G_star, TextureMgr->current_stateUVstar);
        }
        else if (SimulationParams->NLSW_or_Bous == 1) // Bous
        {
            Pass3_BousHandler::Execute(TextureMgr->txState, TextureMgr->txBottom, TextureMgr->txH, TextureMgr->txXFlux, TextureMgr->txYFlux, TextureMgr->oldGradients, TextureMgr->oldOldGradients, TextureMgr->predictedGradients, TextureMgr->F_G_star_oldOldGradients, TextureMgr->txstateUVstar, TextureMgr->txShipPressure, TextureMgr->txNewState, TextureMgr->dU_by_dt, TextureMgr->F_G_star, TextureMgr->current_stateUVstar);
        }

        // Execute BoundaryPass
        BoundaryPassHandler::Execute(TextureMgr->current_stateUVstar, TextureMgr->txBottom, TextureMgr->txWaves, TextureMgr->txtemp, TotalTime);

        // Execute TriDiag_PCRx
        TriDiag_PCRxHandler::Execute(TextureMgr->coefMatx, TextureMgr->newcoef, TextureMgr->txtemp, TextureMgr->txtemp2, TextureMgr->current_stateUVstar, TextureMgr->txNewState, SimulationParams->Px, SimulationParams->NLSW_or_Bous);

        // Execute TriDiag_PCRy
        TriDiag_PCRyHandler::Execute(TextureMgr->coefMaty, TextureMgr->newcoef, TextureMgr->txtemp, TextureMgr->txtemp2, TextureMgr->current_stateUVstar, TextureMgr->txNewState, SimulationParams->Py, SimulationParams->NLSW_or_Bous);
    }
    //!!END CORRECTOR!!

    // Shift gradient textures
    TextureMgr->CopyTexture(TextureMgr->oldGradients, TextureMgr->oldOldGradients);
    TextureMgr->CopyTexture(TextureMgr->predictedGradients, TextureMgr->oldGradients);

    // Copy future_ocean_texture back to ocean_texture
    TextureMgr->CopyTexture(TextureMgr->txNewState, TextureMgr->txState);
    TextureMgr->CopyTexture(TextureMgr->current_stateUVstar, TextureMgr->txstateUVstar);

    // Log every second based on dt
    if (FrameCount % static_cast<int>(1.0f / SimulationParams->dt) == 0)
    {
        // UE_LOG(LogTemp, Log, TEXT("txState:"));
        // RenderTargetReader::ReadAndLogValueAtTexel(TextureMgr->txState, 923, 923);
        // // Save texture to file
        // float SimulationTime = FrameCount * SimulationParams->dt;
        // FString TextureName = FString::Printf(TEXT("txState_%.2f"), SimulationTime);
        // Saver->SaveRenderTargetToFile(TextureMgr->txState, TextureName);
    }

    // Execute the WaveDisplayHandler
    WaveDisplayHandler::Execute(TextureMgr->txState, TextureMgr->txBottom, TextureMgr->txAuxiliary2, TextureMgr->txShipPressure, TextureMgr->txDisplay);

    // Update the material with the combined render target texture
    if (DynamicMaterialInstance)
    {
        DynamicMaterialInstance->SetTextureParameterValue(FName("RenderTargetTexture"), TextureMgr->txDisplay);
    }

    // Flush rendering commands to ensure data is updated before reading
    FlushRenderingCommands();
}
