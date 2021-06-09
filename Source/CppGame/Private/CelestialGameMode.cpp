// Copyright Soren Gilbertson

//TODO Remove unneccesary includes
#include "CelestialGameMode.h"
#include "Serialization\ObjectAndNameAsStringProxyArchive.h"
#include "GameFramework/SpringArmComponent.h"
#include "SaveDataBlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "AssetRegistryModule.h"
#include "CelestialSaveGame.h"
#include "NiagaraComponent.h"
#include "CelestialObject.h"
#include "CelestialPlayer.h"
#include "OverviewPlayer.h"
#include "ShapeSettings.h"
#include "CelestialBody.h"
#include "NoiseSettings.h"
#include "NiagaraActor.h"
#include "EngineUtils.h"
#include "NoiseLayer.h"
#include "Planet.h"
#include "Star.h"

struct FCelestialSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
	FCelestialSaveGameArchive(FArchive& InInnerArchive)
		: FObjectAndNameAsStringProxyArchive(InInnerArchive, false)
	{
		ArIsSaveGame = true;
	}
};


ACelestialGameMode::ACelestialGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACelestialGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	b = true;

	PC = GetWorld()->GetFirstPlayerController();
	
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	// Make widgets
	CelestialWidget = CreateWidget<UUserWidget, APlayerController>(GetWorld()->GetFirstPlayerController(), CelestialWidgetClass);
	OverviewWidget = CreateWidget<UUserWidget, APlayerController>(GetWorld()->GetFirstPlayerController(), OverviewWidgetClass);

	CelestialPlayer = Cast<ACelestialPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), ACelestialPlayer::StaticClass()));
	OverviewPlayer = Cast<AOverviewPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), AOverviewPlayer::StaticClass()));

	CelestialPlayer->SetWidget(CelestialWidget);

	// Gets all ACelestialBodies and adds them to bodies
	for (TActorIterator<ACelestialBody> Itr(GetWorld()); Itr; ++Itr) {
		bodies.Add(*Itr);

		APlanet* Planet = Cast<APlanet>(*Itr);
		if (Planet)
		{
			TerrestrialPlanets.Add(Planet->Name);
			Planet->OnPlanetGenerated.BindUFunction(this, "NewGeneratedPlanet");
		}
	}
	TerrestrialPlanets.Sort([](const FName& a, const FName& b) { return b.FastLess(a); });

	// Gets all actors that implement ICelestialObject and adds them to celestialObjects
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithInterface(GWorld, UCelestialObject::StaticClass(), Actors);
	for (auto& actor : Actors)
	{
		const auto &Interface = Cast<ICelestialObject>(actor);
		celestialObjects.Add(Interface);
	}

	LoadGame();

	SetPerspective(1);
}

void ACelestialGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		return;
	}

	if (currentPerspective == 1)
	{
		for (int i = 0; i < celestialObjects.Num(); i++) {
			ICelestialObject* thisObject = celestialObjects[i];

			thisObject->UpdateVelocity(bodies, DeltaTime);
			thisObject->UpdatePosition(DeltaTime);
		}
	}

	// Set the gravity location of asteroid Niagara systems to the location of the most massive body
	ACelestialBody* LargestBody = nullptr;
	for (auto& Body : bodies)
	{
		if (LargestBody == nullptr)
		{
			LargestBody = Body;
		}
		else if (Body->GetMass() > LargestBody->GetMass())
		{
			LargestBody = Body;
		}
	}
	TArray<AActor*> NiagaraSystems;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANiagaraActor::StaticClass(), NiagaraSystems);
	for (auto& System : NiagaraSystems)
	{
		Cast<ANiagaraActor>(System)->GetNiagaraComponent()->SetNiagaraVariableVec3("GravityPos", LargestBody->GetActorLocation());
	}
}

void ACelestialGameMode::NewGeneratedPlanet(FName PlanetName)
{
	if (b)
	{
		GeneratedPlanets.AddUnique(PlanetName);

		GeneratedPlanets.Sort([](const FName& a, const FName& b) { return b.FastLess(a); });
		if (GeneratedPlanets == TerrestrialPlanets)
		{
			b = false;
			UGameplayStatics::SetGamePaused(GetWorld(), false);
			return;
		}
		else if (TerrestrialPlanets.Num() > TerrestrialPlanets.Find(PlanetName) + 1)
		{
			ReGen(TerrestrialPlanets[TerrestrialPlanets.Find(PlanetName) + 1].ToString());
		}
	}
}

void ACelestialGameMode::SetPerspective(uint8 perspective)
{
	ensure(PC);
	switch (perspective)
	{
	case 0: // Overview mode
	{
		PC->Possess(OverviewPlayer);
		CelestialWidget->RemoveFromViewport();
		OverviewWidget->AddToViewport(0);
		PC->SetInputMode(FInputModeGameAndUI());
		PC->SetShowMouseCursor(true);
		currentPerspective = perspective;
		AOrbitDebugActor::Get()->DrawOrbits();

		TArray<AActor*> NiagaraSystems;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANiagaraActor::StaticClass(), NiagaraSystems);
		for (auto& System : NiagaraSystems)
		{
			Cast<ANiagaraActor>(System)->GetNiagaraComponent()->SetPaused(true);
		}
		break;
	}
	case 1: // Celestial mode
	{
		PC->Possess(CelestialPlayer);
		OverviewWidget->RemoveFromViewport();
		CelestialWidget->AddToViewport(0);
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
		currentPerspective = perspective;
		AOrbitDebugActor::Get()->ClearOrbits();

		TArray<AActor*> NiagaraSystems;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANiagaraActor::StaticClass(), NiagaraSystems);
		for (auto& System : NiagaraSystems)
		{
			Cast<ANiagaraActor>(System)->GetNiagaraComponent()->SetPaused(false);
		}
		break;
	}
	default:
		break;
	}
}

ACelestialBody* ACelestialGameMode::GetBodyByName(FString Name)
{
	for (auto& body : bodies)
	{
		if (body->GetName() == Name)
		{
			return body;
		}
	}
	return nullptr;
}
#pragma optimize("", off)
void ACelestialGameMode::LoadGame()
{
	// Retrieve and cast the USaveGame object to UMySaveGame.
	if (UCelestialSaveGame* LoadedGame = Cast<UCelestialSaveGame>(UGameplayStatics::LoadGameFromSlot("Save", 0)))
	{
		// The operation was successful, so LoadedGame now contains the data we saved earlier.
		UE_LOG(LogTemp, Warning, TEXT("LOADED"));

		for (auto& Object : celestialObjects)
		{
			if (Cast<ACelestialBody>(Object))
			{
				ACelestialBody* Body = Cast<ACelestialBody>(Object);
				UE_LOG(LogTemp, Warning, TEXT("Loading Data For: %s"), *Body->Name.ToString());
				for (auto& data : LoadedGame->CelestialBodyData)
				{
					if (data.Name == Body->Name)
					{
						Body->SetActorTransform(data.Transform);
						FMemoryReader MemoryReader(data.ActorData);
						FCelestialSaveGameArchive Ar(MemoryReader);
						Body->Serialize(Ar);
						UE_LOG(LogTemp, Warning, TEXT("Data Loaded For: %s"), *Body->Name.ToString());
						break;
					}
				}
			}
		}

		AOrbitDebugActor* ODA = AOrbitDebugActor::Get();
		FMemoryReader MemoryReader(LoadedGame->OrbitVisualizationData.ActorData);
		FCelestialSaveGameArchive Ar(MemoryReader);
		ODA->Serialize(Ar);

		ReGen(TerrestrialPlanets[0].ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NOT LOADED"));

		ReGen(TerrestrialPlanets[0].ToString());
	}
}


// ======= Runtime Console Commands ======================================================

void ACelestialGameMode::DeleteSave()
{
	UGameplayStatics::DeleteGameInSlot("Save", 0);
}

void ACelestialGameMode::SaveAndQuit()
{
	Save();
	FGenericPlatformMisc::RequestExit(false);
}

void ACelestialGameMode::SaveAndQuitToMenu()
{
	Save();
	UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");
}

void ACelestialGameMode::Save()
{
	if (UCelestialSaveGame* SaveGameInstance = Cast<UCelestialSaveGame>(UGameplayStatics::CreateSaveGameObject(UCelestialSaveGame::StaticClass())))
	{
		// Set data on the savegame object.

		SaveGameInstance->CelestialBodyData.SetNum(bodies.Num());
		for (int32 i = 0; i < bodies.Num(); i++)
		{
			ACelestialBody* Body = bodies[i];

			SaveGameInstance->CelestialBodyData[i].Class = Body->GetClass();
			SaveGameInstance->CelestialBodyData[i].Transform = Body->GetTransform();
			SaveGameInstance->CelestialBodyData[i].Name = Body->Name;

			FMemoryWriter MemoryWriter(SaveGameInstance->CelestialBodyData[i].ActorData);

			// Use a wrapper archive that converts FNames and UObject*'s to strings that can be read back in
			FCelestialSaveGameArchive Ar(MemoryWriter);

			// Serialize the object
			Body->Serialize(Ar);
		}

		AOrbitDebugActor* ODA = AOrbitDebugActor::Get();
		FMemoryWriter MemoryWriter(SaveGameInstance->OrbitVisualizationData.ActorData);
		FCelestialSaveGameArchive Ar(MemoryWriter);
		ODA->Serialize(Ar);

		// Save the data immediately.
		if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, "Save", 0))
		{
			// Save succeeded.
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Save Succedded")));
			}
		}
		else
		{
			// Save failed.
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("Save Failed")));
			}
		}
	}
}
#pragma optimize("", on)
void ACelestialGameMode::OrbitDebug()
{
	if (currentPerspective != 0)
	{
		SetPerspective(0);
	}

	CreateWidget<UUserWidget, APlayerController>(GetWorld()->GetFirstPlayerController(), OrbitDebugWidgetClass)->AddToViewport(0);
}

void ACelestialGameMode::ReGenAll()
{
	for (TActorIterator<APlanet> Itr(GetWorld()); Itr; ++Itr) {
		Cast<APlanet>(*Itr)->GeneratePlanet();
		Cast<APlanet>(*Itr)->ResetPosition();
	}
}

void ACelestialGameMode::ReGen(FString Planet)
{
	UE_LOG(LogTemp, Warning, TEXT("ReGen on: %s"), *Planet);
	Cast<APlanet>(GetBodyByName(Planet))->GeneratePlanet();
	Cast<APlanet>(GetBodyByName(Planet))->ResetPosition();
}

void ACelestialGameMode::tp(FString toPlanet)
{
	GetWorld()->GetFirstPlayerController()->GetPawn()->SetActorLocation(GetBodyByName(toPlanet)->GetActorLocation());
}

void ACelestialGameMode::SetTerrainResolution(FString Planet, int32 resolution)
{
	Cast<APlanet>(GetBodyByName(Planet))->resolution = resolution;
	Cast<APlanet>(GetBodyByName(Planet))->GeneratePlanet();
}
