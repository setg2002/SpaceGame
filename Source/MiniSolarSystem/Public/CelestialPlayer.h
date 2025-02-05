// Copyright Soren Gilbertson

#pragma once

#include "CoreMinimal.h"
#include "CelestialObject.h"
#include "GameFramework/Pawn.h"
#include "CelestialPlayer.generated.h"


class ACelestialGameMode;
class UCameraComponent;
class UBoxComponent;

UCLASS()
class MINISOLARSYSTEM_API ACelestialPlayer : public APawn, public ICelestialObject
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACelestialPlayer();

	UPROPERTY(SaveGame, BlueprintReadWrite)
	bool bFocusPlanet = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowChangePerspective;

	UFUNCTION(BlueprintCallable)
	virtual int GetMass() const override;

	UFUNCTION()
	virtual void UpdateVelocity(TArray<ACelestialBody*> allBodies, float timeStep) override;

	UFUNCTION()
	virtual void UpdatePosition(float timeStep) override;

	UFUNCTION(BlueprintCallable)
	virtual FVector GetCurrentVelocity() const override { return currentVelocity; }

	// Returns the throttle value
	UFUNCTION(BlueprintCallable)
	float GetThrottle()
	{
		return Throttle;
	}
	void SetThrottle(float NewThrottle) { Throttle = NewThrottle; }

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
	bool bIgnoreGravity;

	UFUNCTION(BlueprintCallable)
	ACelestialBody* GetLargestForce();

	UFUNCTION(BlueprintCallable)
	ACelestialBody* LookingAtPlanet();

	void SetCurrentVelocity(FVector NewVelocity);

	void SetWidget(UUserWidget* Highlight) { Widget = Highlight; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(SaveGame)
	FVector currentVelocity;

	//UPROPERTY(SaveGame)
	const int mass = 10;

	const int MaxSpeed = 9999;

	UUserWidget* Widget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBoxComponent* Collider;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCameraComponent* Camera; //NOTE: Making components into UPROPERTY() makes them NULL at runtime

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called when collider is overlapped with
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	// Called when collider overlap ends
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> PlanetHighlightClass;

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void MoveUp(float AxisValue);

	void RotationX(float AxisValue);
	void RotationY(float AxisValue);
	void RotationZ(float AxisValue);

	void SwitchPerspective();

	void SwitchIgnoreGravity()
	{
		bIgnoreGravity = !bIgnoreGravity;
	}

	void SwitchFocusPlanet();
	
	UPROPERTY(EditAnywhere)
	int RotationForce = 100;
	UPROPERTY(SaveGame)
	float Throttle = 1;

	void ChangeThrottle(float AxisValue)
	{
		if (Throttle + AxisValue / 10.f > 0.09f && Throttle + AxisValue / 10.f <= 5)
		{
			Throttle += AxisValue / 10.f;
		}
	}

	void LimitVelocity();

	ACelestialGameMode* gameMode;

	TMap<ACelestialBody*, float> ForcePerBody;

private:
	// The actor that is currently being overlapped with, nullptr if not overlapping
	AActor* OverlappedActor;
};
