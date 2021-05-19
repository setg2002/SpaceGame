// Copyright Soren Gilbertson

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EFilterType.h"
#include "NoiseSettings.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FSimpleNoiseSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
	float Strength = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
	float BaseRoughness = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
	float Roughness = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Center;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", ClampMax = "8"))
	int numLayers = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "2"))
	float Persistence = .5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
	float MinValue;
};

USTRUCT(BlueprintType)
struct FRidgidNoiseSettings : public FSimpleNoiseSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WeightMultiplier = .8f;
};

UCLASS(BlueprintType)
class CPPGAME_API UNoiseSettings : public UDataAsset
{
	GENERATED_BODY()

	DECLARE_DELEGATE(FNoiseSettingsChanged);

private:
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EFilterType> FilterType;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "FilterType == 0", EditConditionHides))
	FSimpleNoiseSettings SimpleNoiseSettings;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "FilterType == 1", EditConditionHides))
	FRidgidNoiseSettings RidgidNoiseSettings;

public:
	UNoiseSettings();
	~UNoiseSettings();

	FNoiseSettingsChanged OnNoiseSettingsChanged;

	UFUNCTION(BlueprintCallable)
	TEnumAsByte<EFilterType> GetFilterType() const { return FilterType; }
	UFUNCTION(BlueprintCallable)
	void SetFilterType(EFilterType NewFilterType);

	UFUNCTION(BlueprintCallable)
	FSimpleNoiseSettings GetSimpleNoiseSettings() const { return SimpleNoiseSettings; }
	UFUNCTION(BlueprintCallable)
	FRidgidNoiseSettings GetRidgidNoiseSettings() const { return RidgidNoiseSettings; }

	UFUNCTION(BlueprintCallable)
	void SetSimpleNoiseSettings(FSimpleNoiseSettings NewSettings);
	UFUNCTION(BlueprintCallable)
	void SetRidgidNoiseSettings(FRidgidNoiseSettings NewSettings);
};
