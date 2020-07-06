// This is a copyright notice


#include "Planet.h"

APlanet::APlanet() 
{
	meshes = { CreateDefaultSubobject<UProceduralMeshComponent>("Mesh"),
			   CreateDefaultSubobject<UProceduralMeshComponent>("Mesh1"),
			   CreateDefaultSubobject<UProceduralMeshComponent>("Mesh2"),
			   CreateDefaultSubobject<UProceduralMeshComponent>("Mesh3"),
			   CreateDefaultSubobject<UProceduralMeshComponent>("Mesh4"),
			   CreateDefaultSubobject<UProceduralMeshComponent>("Mesh5")
	};

	GeneratePlanet();
}

void APlanet::GeneratePlanet()
{
	if (ColorSettings)
	{
		for (auto& mesh : meshes)
		{
			mesh->SetRelativeLocation(FVector().ZeroVector);
		}

		if (ShapeSettings != nullptr && ShapeSettings->GetNoiseLayers())
		{
			Initialize();
			GenerateMesh();
			GenerateColors();
		}
	}
}

void APlanet::ReGenerate()
{
	GeneratePlanet();
}

void APlanet::Initialize()
{
	shapeGenerator = new ShapeGenerator(ShapeSettings);
	colorGenerator = new ColorGenerator(ColorSettings);

	for (int i = 0; i < 6; i++)
	{
		terrainFaces[i] = new TerrainFace(shapeGenerator, meshes[i], resolution, directions[i]);

		ColorSettings->DynamicMaterials[i] = meshes[i]->CreateAndSetMaterialInstanceDynamicFromMaterial(0, ColorSettings->PlanetMat);

		bool renderFace = FaceRenderMask == EFaceRenderMask::All || FaceRenderMask - 1 == i;
		meshes[i]->SetVisibility(renderFace);
	}
}

void APlanet::GenerateMesh()
{
	for (int i = 0; i < 6; i++)
	{
		if (meshes[i]->IsVisibleInEditor())
		{
			terrainFaces[i]->ConstructMesh();
		}
	}

	colorGenerator->UpdateElevation(shapeGenerator->ElevationMinMax);
}

void APlanet::OnShapeSettingsUpdated()
{
	Initialize();
	GenerateMesh();
}

void APlanet::OnColorSettingsUpdated()
{
	Initialize();
	GenerateColors();
}


void APlanet::GenerateColors()
{
	if (ColorSettings != nullptr)
	{
		for (int i = 0; i < 6; i++)
		{
			terrainFaces[i]->ColorMesh(ColorSettings);
		}
		colorGenerator->UpdateColors();
	}
}

void APlanet::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property != nullptr)
	{
		const FName PropertyName(PropertyChangedEvent.Property->GetName());
		if (PropertyName == GET_MEMBER_NAME_CHECKED(APlanet, resolution) && AutoGenerate)
		{
			GeneratePlanet();
		}
		if (PropertyName == GET_MEMBER_NAME_CHECKED(APlanet, ShapeSettings) && AutoGenerate)
		{
			GeneratePlanet();
		}
		if (PropertyName == GET_MEMBER_NAME_CHECKED(APlanet, ColorSettings) && AutoGenerate)
		{
			GeneratePlanet();
		}
		if (PropertyName == GET_MEMBER_NAME_CHECKED(APlanet, FaceRenderMask) && AutoGenerate)
		{
			GeneratePlanet();
		}
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}