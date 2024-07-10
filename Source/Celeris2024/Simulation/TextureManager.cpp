#include "TextureManager.h"
#include "TextureCreator.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/World.h"

TextureManager::TextureManager(UObject* InOuter, USimulationParameters* InSimulationParams)
    : Outer(InOuter), SimulationParams(InSimulationParams)
{
}

void TextureManager::InitializeTextures(const TArray<FVector4>& DepthData, const TArray<FVector4>& OceanElevationData, const TArray<FVector4>& waveVectors, const TArray<FVector4>& TridiagCoefXData, const TArray<FVector4>& TridiagCoefYData)
{
    int32 width = SimulationParams->width;
    int32 height = SimulationParams->height;

    if (width <= 0 || height <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid texture dimensions: width=%f, height=%f"), width, height);
        return;
    }

    txBottom = TextureCreator::CreateInitializedRenderTarget(Outer, width, height, &DepthData);
    txState = TextureCreator::CreateInitializedRenderTarget(Outer, width, height, &OceanElevationData);
    txNewState = TextureCreator::CreateInitializedRenderTarget(Outer, width, height, &OceanElevationData);
    txstateUVstar = TextureCreator::CreateInitializedRenderTarget(Outer, width, height, &OceanElevationData);
    txstateFGstar = TextureCreator::CreateInitializedRenderTarget(Outer, width, height, &OceanElevationData);
    current_stateUVstar = TextureCreator::CreateInitializedRenderTarget(Outer, width, height, &OceanElevationData);
    current_stateFGstar = TextureCreator::CreateInitializedRenderTarget(Outer, width, height, &OceanElevationData);
    txStateUVstar_pred = TextureCreator::CreateInitializedRenderTarget(Outer, width, height, &OceanElevationData);

    txWaves = TextureCreator::CreateInitializedRenderTarget(Outer, SimulationParams->numberOfWaves, 1, &waveVectors);

    txH = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    txU = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    txV = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    txW = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    txC = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    txXFlux = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    txYFlux = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    oldGradients = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    oldOldGradients = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    predictedGradients = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    F_G_star_oldGradients = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    F_G_star_oldOldGradients = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    F_G_star_predictedGradients = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    txNormal = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    txAuxiliary2 = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    txAuxiliary2Out = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    txtemp = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    txtemp2 = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    dU_by_dt = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    F_G_star = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    txShipPressure = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
    txDisplay = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);

    coefMatx = TextureCreator::CreateInitializedRenderTarget(Outer, width, height, &TridiagCoefXData);
    coefMaty = TextureCreator::CreateInitializedRenderTarget(Outer, width, height, &TridiagCoefYData);
    newcoef = TextureCreator::CreateInitializedRenderTarget(Outer, width, height);
}

void TextureManager::ReleaseTextures()
{
    if (txBottom) txBottom->ReleaseResource();
    if (txState) txState->ReleaseResource();
    if (txNewState) txNewState->ReleaseResource();
    if (txstateUVstar) txstateUVstar->ReleaseResource();
    if (txstateFGstar) txstateFGstar->ReleaseResource();
    if (current_stateUVstar) current_stateUVstar->ReleaseResource();
    if (current_stateFGstar) current_stateFGstar->ReleaseResource();
    if (txStateUVstar_pred) txStateUVstar_pred->ReleaseResource();
    if (txWaves) txWaves->ReleaseResource();
    if (txH) txH->ReleaseResource();
    if (txU) txU->ReleaseResource();
    if (txV) txV->ReleaseResource();
    if (txW) txW->ReleaseResource();
    if (txC) txC->ReleaseResource();
    if (txXFlux) txXFlux->ReleaseResource();
    if (txYFlux) txYFlux->ReleaseResource();
    if (oldGradients) oldGradients->ReleaseResource();
    if (oldOldGradients) oldOldGradients->ReleaseResource();
    if (predictedGradients) predictedGradients->ReleaseResource();
    if (F_G_star_oldGradients) F_G_star_oldGradients->ReleaseResource();
    if (F_G_star_oldOldGradients) F_G_star_oldOldGradients->ReleaseResource();
    if (F_G_star_predictedGradients) F_G_star_predictedGradients->ReleaseResource();
    if (txNormal) txNormal->ReleaseResource();
    if (txAuxiliary2) txAuxiliary2->ReleaseResource();
    if (txAuxiliary2Out) txAuxiliary2Out->ReleaseResource();
    if (txtemp) txtemp->ReleaseResource();
    if (txtemp2) txtemp2->ReleaseResource();
    if (dU_by_dt) dU_by_dt->ReleaseResource();
    if (F_G_star) F_G_star->ReleaseResource();
    if (txShipPressure) txShipPressure->ReleaseResource();
    if (txDisplay) txDisplay->ReleaseResource();
    if (coefMatx) coefMatx->ReleaseResource();
    if (coefMaty) coefMaty->ReleaseResource();
    if (newcoef) newcoef->ReleaseResource();
}

void TextureManager::CopyTexture(UTextureRenderTarget2D* SourceTexture, UTextureRenderTarget2D* DestinationTexture)
{
    if (SourceTexture && DestinationTexture)
    {
        ENQUEUE_RENDER_COMMAND(CopyTextureCommand)(
            [SourceTexture, DestinationTexture](FRHICommandListImmediate& RHICmdList)
            {
                FRHITexture* SrcTexture = SourceTexture->GetRenderTargetResource()->TextureRHI.GetReference();
                FRHITexture* DstTexture = DestinationTexture->GetRenderTargetResource()->TextureRHI.GetReference();
                RHICmdList.CopyTexture(SrcTexture, DstTexture, FRHICopyTextureInfo());
            }
        );
    }
}
