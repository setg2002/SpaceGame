// Copyright Soren Gilbertson

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CelestialGameInstance.generated.h"


class UNoiseLayer;

/**
 * 
 */
UCLASS(Config=Game)
class MINISOLARSYSTEM_API UCelestialGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

public:
	UFUNCTION(BlueprintCallable)
	static void PlayLoadingScreen(bool bPlayUntilStopped, float PlayTime);
	
	UFUNCTION(BlueprintCallable)
	static void StopLoadingScreen();

//============== Max FPS ==============
private:
	UPROPERTY(Config)
	int32 MaxFPS = 0;
public:
	UFUNCTION(BlueprintCallable)
	int32 GetMaxFPS() const { return MaxFPS; }
	UFUNCTION(BlueprintNativeEvent, BLueprintCallable)
	void SetMaxFPS(int32 NewMaxFPS);

//============== Save Game Slot ==============
private:
	uint8 Slot;
public:
	UFUNCTION(BLueprintCallable)
	uint8 GetGameSlot() const { return Slot; }
	UFUNCTION(BLueprintCallable)
	void SetGameSlot(uint8 NewSlot) { Slot = NewSlot; }

//============== Volume Control ==============
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetVolume(float NewVolume);
	UFUNCTION(BlueprintCallable)
	float GetVolume() const { return Volume; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetUIVolume(float NewUIVolume);
	UFUNCTION(BlueprintCallable)
	float GetUIVolume() const { return UIVol; }
protected:
	UPROPERTY(Config, BlueprintReadWrite)
	float Volume = 1.f;
	UPROPERTY(Config)
	float UIVol = 1.f;

//============== Copy/Pasting Noise Layers ==============
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
	int32 ResMax = 2;
public:
	UFUNCTION(BlueprintCallable)
	int32 GetResMax() { return ResMax; }
	UFUNCTION(BlueprintCallable)
	void SetResMax(int32 NewResMax) { ResMax = NewResMax; SaveConfig(); }

//============== Showing Tutorial on First Play ==============
private:
	UPROPERTY(Config)
	bool bTutorialSeen = false;
public:
	UFUNCTION(BlueprintCallable)
	bool GetTutorialSeen() { return bTutorialSeen; }
	UFUNCTION(BlueprintCallable)
	void SetTutorialSeen(bool TutorialSeen);
};
