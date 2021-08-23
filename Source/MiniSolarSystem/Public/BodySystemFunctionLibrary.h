// Copyright Soren Gilbertson

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BodySystemFunctionLibrary.generated.h"


USTRUCT(BlueprintType)
struct FBodySystem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SystemName;

	UPROPERTY()
	TArray<uint32> BodyIDs;

	FORCEINLINE bool operator==(const FBodySystem& other) const
	{
		return other.SystemName == SystemName;
	}

	FBodySystem()
	{
		SystemName = NAME_None;
	}

	FBodySystem(FName Name, TArray<uint32> IDs)
	{
		SystemName = Name;
		BodyIDs = IDs;
	}
};

/**
 * 
 */
UCLASS()
class MINISOLARSYSTEM_API UBodySystemFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void AddIDs(FBodySystem System, TArray<class ACelestialBody*> IDsToAdd);
	UFUNCTION(BlueprintCallable)
	static void RemoveID(FBodySystem System, class ACelestialBody* IDsToRemove);
	UFUNCTION(BlueprintCallable)
	static bool DoesSystemContainBody(FBodySystem System, class ACelestialBody* Body);
};
