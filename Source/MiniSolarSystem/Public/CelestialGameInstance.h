// Copyright Soren Gilbertson

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CelestialGameInstance.generated.h"


class UNoiseLayer;

/**
 * 
 */
UCLASS(Config = Game)
class MINISOLARSYSTEM_API UCelestialGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

public:
	UFUNCTION(BlueprintCallable)
	float GetCurrentMaxFPS();

	UFUNCTION(BlueprintCallable)
	static void PlayLoadingScreen(bool bPlayUntilStopped, float PlayTime);
	
	UFUNCTION(BlueprintCallable)
	static void StopLoadingScreen();

//============== Volume Control ==============
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetVolume(float NewVolume);
	UFUNCTION(BlueprintCallable)
	float GetVolume() const { return Volume; }
protected:
	UPROPERTY(Config, BlueprintReadWrite)
	float Volume = 1.f;

//============== Coply/Pasting Noise Layers ==============
private:
	UNoiseLayer* CopiedNoiseLayer;
public:
	UFUNCTION(BLueprintCallable)
	void CopyNoiseLayer(UNoiseLayer* NoiseLayerToCopy);
	UFUNCTION(BLueprintCallable)
	void PasteNoiseLayer(UNoiseLayer* NoiseLayerToPasteTo);

//============== Terrestrial Planet Resolution Level ==============
private:
	UPROPERTY(Config)
	int32 ResMax;
public:
	UFUNCTION(BlueprintCallable)
	int32 GetResMax() { return ResMax; }
	UFUNCTION(BlueprintCallable)
	void SetResMax(int32 NewResMax) { ResMax = NewResMax; SaveConfig(); }
};
