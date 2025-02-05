// Copyright Soren Gilbertson

#pragma once

#include "CoreMinimal.h"
#include "CelestialBody.h"
#include "Planet.generated.h"

/**
 * 
 */

//Forward Declarations Here
class TerrainFace;
class UProceduralMeshComponent;
class AOrbitDebugActor;
class ShapeGenerator;
class TerrestrialColorGenerator;
class UShapeSettings;
class UColorSettings;



UENUM()
enum EFaceRenderMask { 
	NoMask   UMETA(DisplayName = "All Faces"),
	Bottom   UMETA(DisplayName = "Bottom"),
	Top      UMETA(DisplayName = "Top"),
	Left     UMETA(DisplayName = "Left"),
	Right    UMETA(DisplayName = "Right"),
	Back     UMETA(DisplayName = "Back"),
	Front    UMETA(DisplayName = "Front")
};


UCLASS()
class MINISOLARSYSTEM_API APlanet : public ACelestialBody
{
	GENERATED_BODY()

	DECLARE_DELEGATE_OneParam(FPlanetGenerated, FName)
	
public:
	APlanet();
	~APlanet();

	FPlanetGenerated OnPlanetGenerated;
	
	// True when the planet is already generating
	UPROPERTY(BlueprintReadWrite)
	bool bGenerating;

	void ResetPosition();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Main mesh	
	//UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite)
	UProceduralMeshComponent* ProcMesh;

	TerrainFace* TerrainFaces[6];

	ShapeGenerator* shapeGenerator;
	TerrestrialColorGenerator* colorGenerator;

	// When true, the planet will call ReGenerate() every time a parameter is changed
	UPROPERTY(SaveGame, Category = "Settings", EditAnywhere)
	bool bAutoGenerate = true;

	// When true, planet terrain generation wil be multithreaded
	UPROPERTY(SaveGame, Category = "Settings", EditAnywhere)
	bool bMultithreadGeneration = true;

	// When true, tangents and normals will be generated for the planet mesh every time a parameter is changed (can only be true if bAutoGenerate is true)
	UPROPERTY(Category = "Settings", EditInstanceOnly, BlueprintReadWrite, meta = (EditCondition = "bAutoGenerate"))
	bool bAutoGenerateTangents;

	// Renders only the selected face
	UPROPERTY(Category = "Settings", EditInstanceOnly)
	TEnumAsByte<EFaceRenderMask> FaceRenderMask;

	UPROPERTY(Category = "Settings", EditAnywhere, BlueprintReadWrite)
	UColorSettings* ColorSettings;
	UPROPERTY(Category = "Settings", EditAnywhere, BlueprintReadWrite)
	UShapeSettings* ShapeSettings;

	// Initializes TerrainFaces
	void Initialize();

	// Makes new empty data asset for given data asset
	UFUNCTION(BlueprintCallable)
	UObject* CreateSettingsAssetBP(TSubclassOf<UObject> AssetClass);
	template< class T >
	T* CreateSettingsAsset(TSubclassOf<UObject> AssetClass);
	template< class T >
	T* CreateSettingsAssetEditor(TSubclassOf<UObject> AssetClass);
	static UObject* RestoreSettingsAsset(FName Name, TArray<uint8> Data, UClass* Class);

	// Sets color and shape settings to null for regenerating planets from save i guess
	void ClearSettingsAssets();

	// Makes new blank data asset for shape and color settings if either are unassigned
	UFUNCTION(Category = "Settings", BlueprintCallable, CallInEditor)
	void CreateSettingsAssets();

	//TODO All of these Generate, ReGenerate, and On___Updated functions need to be sorted out

	UFUNCTION(Category = "Settings", BlueprintCallable, CallInEditor)
	void ClearMeshSections();

	// Blueprint callable for GeneratePlanet()
	UFUNCTION(Category = "Settings", BlueprintCallable, CallInEditor)
	void ReGenerate();

	UFUNCTION(Category = "Settings", BlueprintCallable, CallInEditor)
	void ReGenerateColors();

	UFUNCTION(Category = "Settings", BlueprintCallable, CallInEditor)
	void ReGenerateTangents();

	void GeneratePlanet();

	void GenerateMesh();

	void GenerateColors();

	UFUNCTION(BlueprintCallable)
	void OnShapeSettingsUpdated();
	UFUNCTION(BlueprintCallable)
	void OnColorSettingsUpdated();

	virtual void OnConstruction(const FTransform & Transform) override;

	UFUNCTION(BlueprintCallable)
	void BindDelegates();
	UFUNCTION(BlueprintCallable)
	void UnBindDelegates();
	UFUNCTION(BlueprintCallable)
	void BindSettingsIDs();

	virtual float GetBodyRadius() const override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	virtual void PostEditMove(bool bFinished) override;
#endif

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void CreatePackageName(FString& OutAssetName, FString& OutPackagePath, UObject& OutOuter, TSubclassOf<UDataAsset> DataAssetClass);

private:
	const FVector directions[6] = { FVector().UpVector,
									FVector().DownVector,
									FVector().LeftVector,
									FVector().RightVector,
									FVector().ForwardVector,
									FVector().BackwardVector };

	const int32 Resolutions[5] = { 16, 32, 64, 128, 256 };
	int32 ResolutionLevel;
};
