// Copyright Soren Gilbertson

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "CelestialGameMode.generated.h"

/**
 * 
 */

//Forward Declarations
class ICelestialObject;
class ACelestialPlayer;
class AOverviewPlayer;
class ACelestialBody;
class UUserWidget;
class APlanet;
class AStar;

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPerspectiveChanged, uint8, Perspective);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLoadingDone);


UCLASS()
class MINISOLARSYSTEM_API ACelestialGameMode : public AGameModeBase
{
	GENERATED_BODY()

private:
	static float gravitationalConstant;

public:
	ACelestialGameMode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable)
	FPerspectiveChanged OnPerspectiveChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable)
	FLoadingDone OnLoadingComplete;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	static float GetGravitationalConstant() { return gravitationalConstant; }
	UFUNCTION(BlueprintCallable)
	void SetGravitationalConstant(float NewG);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> OverviewWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> CelestialWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> OrbitDebugWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> PauseWidgetClass;

	/* Sets the player perspective to the one desired
	* @param perspective 0 is overview, 1 is celestial */
	void SetPerspective(uint8 perspective);

	UFUNCTION(BlueprintCallable)
	uint8 GetCurrentPerspective() const { return currentPerspective; }
	
	void LoadGame();
	void SaveAsync(FAsyncSaveGameToSlotDelegate Out);

	// ======= Runtime Console Commands =======

	UFUNCTION(Exec, BlueprintCallable)
	void DeleteSave();

	UFUNCTION(Exec, BlueprintCallable)
	void SaveAndQuit();
	
	UFUNCTION(Exec, BlueprintCallable)
	void SaveAndQuitToMenu();

	UFUNCTION(Exec, BlueprintCallable)
	void Save() { SaveAsync(nullptr); }

	UFUNCTION(Exec, BlueprintCallable)
	void OrbitDebug();

	UFUNCTION(Exec, BlueprintCallable)
	void ReGenAll();

	UFUNCTION(Exec, BlueprintCallable)
	void ReGen(FString Planet);

	UFUNCTION(Exec, BlueprintCallable)
	void tp(FString toPlanet);

	UFUNCTION(Exec, BlueprintCallable)
	void RemoveBody(FString Body);

	UFUNCTION(Exec, BlueprintCallable)
	void SetTerrainResolution(FString PlanetName, int32 resolution);

	UFUNCTION(Exec, BlueprintCallable)
	void PauseGame();

	// ======= End ConsoleCommands =======

	UFUNCTION(BlueprintCallable)
	ACelestialBody* AddBody(TSubclassOf<ACelestialBody> Class, FName Name, FTransform Transform);

	ACelestialBody* GetBodyByName(FString Name);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadWrite)
	TArray<ACelestialBody*> bodies;

	TArray<ICelestialObject*> celestialObjects;

	UPROPERTY() // Prevents garbage collection
	UUserWidget* OverviewWidget;
	UPROPERTY(BlueprintReadOnly)
	UUserWidget* CelestialWidget;
	UPROPERTY()
	UUserWidget* PauseWidget;

	bool bGamePaused;

	uint8 currentPerspective;

	UPROPERTY()
	APlayerController* PC;

	UPROPERTY()
	ACelestialPlayer* CelestialPlayer;
	UPROPERTY()
	AOverviewPlayer* OverviewPlayer;

public:
	TArray<ACelestialBody*> GetBodies() { return bodies; }
	ACelestialPlayer* GetCelestialPlayer() const { return CelestialPlayer; }
	AOverviewPlayer* GetOverviewPlayer() const { return OverviewPlayer; }

private:
	UMaterialParameterCollectionInstance* PlanetIlluminationInst;

	uint8 NumStars;
};


namespace GeneratePlanetsOrdered
{
	static void DoGeneratePlanetsOrdered(TArray<FName> PlanetNames, ACelestialGameMode* GM);

	namespace // Anonymous namespace makes these variables "private"
	{
		static TArray<FName> TerrestrialPlanets;
		static TArray<FName> GeneratedPlanets;
		static ACelestialGameMode* GameMode;

		static bool bCurrentlyGenerating = false;
	}

	static void NewGeneratedPlanet(FName PlanetName);
};