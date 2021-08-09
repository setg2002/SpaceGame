// Copyright Soren Gilbertson

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CelestialSaveGame.generated.h"

/**
 * 
 */

USTRUCT()
struct FAssetRecord
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	UClass* Class;

	UPROPERTY(SaveGame)
	FName Name;

	UPROPERTY(SaveGame)
	TArray<uint8> AssetData;

	// Default Values
	FAssetRecord()
	{
		Class = nullptr;
		Name = NAME_None;
	}
};

USTRUCT()
struct FActorRecord
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	UClass* Class;

	UPROPERTY(SaveGame)
	FTransform Transform;

	UPROPERTY(SaveGame)
	FName Name;

	UPROPERTY(SaveGame)
	TArray<uint8> ActorData;

	// Default Values
	FActorRecord()
	{
		Class = nullptr;
		Transform = FTransform();
		Name = NAME_None;
	}
};

USTRUCT()
struct FComponentRecord : public FActorRecord
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	FName ParentName;

	// Default Values
	FComponentRecord()
	{
		ParentName = NAME_None;
		Class = nullptr;
		Transform = FTransform();
		Name = NAME_None;
	}
};


UCLASS()
class MINISOLARSYSTEM_API UCelestialSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UCelestialSaveGame();

	UPROPERTY(VisibleAnywhere)
	TArray<FActorRecord> CelestialBodyData;

	UPROPERTY(VisibleAnywhere)
	TArray<FComponentRecord> CelestialComponentData;

	UPROPERTY(VisibleAnywhere)
	FActorRecord OrbitVisualizationData;

	UPROPERTY(VisibleAnywhere)
	FActorRecord CelestialPlayerData;

	UPROPERTY(VisibleAnywhere)
	FActorRecord OverviewPlayerData;
	UPROPERTY(VisibleAnywhere)
	float OverviewArmLength;
	UPROPERTY(VisibleAnywhere)
	FRotator OverviewCameraRotation;

	UPROPERTY(VisibleAnywhere)
	float GravConst;


	// Asset Saving \\

	UPROPERTY(VisibleAnywhere)
	TArray<FAssetRecord> GradientAssets;

	UPROPERTY(VisibleAnywhere)
	TArray<FAssetRecord> SettingsAssets;
};