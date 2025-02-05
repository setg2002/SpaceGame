// Copyright Soren Gilbertson

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrbitDebugActor.generated.h"

// Forward Declarations
class ACelestialBody;
class USplineComponent;
class UNiagaraSystem;
class UNiagaraComponent;

class VirtualBody
{
public:
	FVector Position;
	FVector Velocity;
	int Mass;

	VirtualBody(ACelestialBody* Body);
};

UCLASS()
class MINISOLARSYSTEM_API AOrbitDebugActor : public AActor
{
	GENERATED_BODY()
	
// All ID tracking is used to keep orbit colors consistent between saves and when bodies are added/removed
private:
	UPROPERTY(SaveGame, EditAnywhere)
	TArray<uint32> IDs;

	TMap<ACelestialBody*, UNiagaraComponent*> BodyToParticleComp;
	TArray<TArray<FVector>> SavedPoints; // For UpdateWidthSpecificBody

public:
	void AddID(uint32 NewID);
	void RemoveID(uint32 IDToRemove);

private:
	UPROPERTY(SaveGame, EditAnywhere)
	int32 NumSteps = 1000;

	UPROPERTY(SaveGame, EditAnywhere)
	float TimeStep = 0.0833f;

	UPROPERTY(SaveGame, EditAnywhere)
	bool bRelativeToBody;

	UPROPERTY(SaveGame, EditAnywhere)
	bool bPhysicsTimeStep;

	UPROPERTY(SaveGame, EditAnywhere)
	ACelestialBody* CentralBody;

	// The width of the debug. If it is 0 (zero) then the planet's radius will be used
	UPROPERTY(SaveGame, EditAnywhere)
	float Width = 25;

	int32 RenderedSteps = 100;

public:	
	UFUNCTION(BlueprintCallable)
	static AOrbitDebugActor* Get();

	UFUNCTION(BlueprintCallable)
	int32 GetNumSteps() const { return NumSteps; }
	UFUNCTION(BlueprintCallable)
	void SetNumSteps(int32 NewNumSteps);

	UFUNCTION(BlueprintCallable)
	int32 GetRenderedSteps() const { return RenderedSteps; }
	UFUNCTION(BlueprintCallable)
	void SetRenderedSteps(int32 NewRenderedSteps);

	UFUNCTION(BlueprintCallable)
	float GetTimeStep() const { return TimeStep; }
	UFUNCTION(BlueprintCallable)
	void SetTimeStep(float NewTimeStep);

	UFUNCTION(BlueprintCallable)
	float GetWidth() const { return Width; }
	UFUNCTION(BlueprintCallable)
	void SetWidth(float NewWidth);

	UFUNCTION(BlueprintCallable)
	bool GetRelativeToBody() const { return bRelativeToBody; }
	UFUNCTION(BlueprintCallable)
	void SetRelativeToBody(bool NewRelativeToBody);

	UFUNCTION(BlueprintCallable)
	bool GetPhysicsTimeStep() const { return bPhysicsTimeStep; }
	UFUNCTION(BlueprintCallable)
	void SetPhysicsTimeStep(bool NewPhysicsTimeStep);

	UFUNCTION(BlueprintCallable)
	ACelestialBody* GetRelativeBody() const { return CentralBody; }
	UFUNCTION(BlueprintCallable)
	void SetRelativeBody(ACelestialBody* NewRelativeBody);

	UPROPERTY(EditAnywhere)
	bool bAutoDraw = true;

	UPROPERTY(SaveGame, EditAnywhere)
	bool bUsePhysicsTimeStep;

	UPROPERTY(EditAnywhere)
	TArray<USplineComponent*> Splines;

	UFUNCTION(BlueprintCallable, CallInEditor)
	void DrawOrbits();
	UFUNCTION(BlueprintCallable, CallInEditor)
	void ClearOrbits();

	UFUNCTION(BlueprintCallable)
	void UpdateWidthSpecificBody(ACelestialBody* Body);

	virtual void OnConstruction(const FTransform & Transform) override;

	// When true, orbits will not be drawn. This is for disabling orbit drawing when doing multiple actions that would otherwise cause unneccesary draw calls
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool ManualStop = false;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UNiagaraSystem* ParticleTemplate;

	TArray<UNiagaraComponent*> ParticleComponents;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	// Sets default values for this actor's properties
	AOrbitDebugActor();
	
	static AOrbitDebugActor* _instance;

	TArray<FColor> Colors = { FColor::Red, FColor::Blue, FColor::Green, FColor::White, FColor::Magenta, FColor::Cyan, FColor::Emerald, FColor::Orange, FColor::Purple, FColor::Silver, FColor::Yellow };

	FVector CalculateAcceleration(int i, TArray<VirtualBody*> VirtualBodies);
};
