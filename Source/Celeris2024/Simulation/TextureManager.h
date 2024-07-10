#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "SimulationParameters.h"

/**
 * TextureManager class for managing simulation textures.
 */
class TextureManager
{
public:
    TextureManager(UObject* InOuter, USimulationParameters* InSimulationParams);
    
    void InitializeTextures(const TArray<FVector4>& DepthData, const TArray<FVector4>& OceanElevationData, const TArray<FVector4>& waveVectors, const TArray<FVector4>& TridiagCoefXData, const TArray<FVector4>& TridiagCoefYData);
    void ReleaseTextures();

    void CopyTexture(UTextureRenderTarget2D* SourceTexture, UTextureRenderTarget2D* DestinationTexture);

    UTextureRenderTarget2D* txBottom;
    UTextureRenderTarget2D* txState;
    UTextureRenderTarget2D* txNewState;
    UTextureRenderTarget2D* txstateUVstar;
    UTextureRenderTarget2D* txstateFGstar;
    UTextureRenderTarget2D* current_stateUVstar;
    UTextureRenderTarget2D* current_stateFGstar;
    UTextureRenderTarget2D* txStateUVstar_pred;
    UTextureRenderTarget2D* txH;
    UTextureRenderTarget2D* txU;
    UTextureRenderTarget2D* txV;
    UTextureRenderTarget2D* txW;
    UTextureRenderTarget2D* txC;
    UTextureRenderTarget2D* txXFlux;
    UTextureRenderTarget2D* txYFlux;
    UTextureRenderTarget2D* oldGradients;
    UTextureRenderTarget2D* oldOldGradients;
    UTextureRenderTarget2D* predictedGradients;
    UTextureRenderTarget2D* F_G_star_oldGradients;
    UTextureRenderTarget2D* F_G_star_oldOldGradients;
    UTextureRenderTarget2D* F_G_star_predictedGradients;
    UTextureRenderTarget2D* txNormal;
    UTextureRenderTarget2D* txAuxiliary2;
    UTextureRenderTarget2D* txAuxiliary2Out;
    UTextureRenderTarget2D* txtemp;
    UTextureRenderTarget2D* txtemp2;
    UTextureRenderTarget2D* txWaves;
    UTextureRenderTarget2D* finalRenderState;
    UTextureRenderTarget2D* coefMatx;
    UTextureRenderTarget2D* coefMaty;
    UTextureRenderTarget2D* newcoef;
    UTextureRenderTarget2D* dU_by_dt;
    UTextureRenderTarget2D* F_G_star;
    UTextureRenderTarget2D* txShipPressure;
    UTextureRenderTarget2D* txDisplay;

private:
    UObject* Outer;
    USimulationParameters* SimulationParams;
};
