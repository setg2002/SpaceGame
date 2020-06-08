// This is a copyright notice

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ShapeSettings.generated.h"

/**
 * 
 */
UCLASS()
class CPPGAME_API UShapeSettings : public UDataAsset
{
	GENERATED_BODY()
	
public:
	//Radius in cm
	UPROPERTY(EditAnywhere)
	float PlanetRadius = 100;

};
