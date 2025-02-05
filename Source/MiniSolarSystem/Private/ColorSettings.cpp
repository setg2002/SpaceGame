// Copyright Soren Gilbertson


#include "ColorSettings.h"
#include "Curves/CurveLinearColor.h"

UColorSettings::UColorSettings()
{
	PlanetMat = LoadObject<UMaterialInterface>(NULL, TEXT("MaterialInstanceConstant'/Game/Materials/Instances/M_Planet_Inst.M_Planet_Inst'"), NULL, LOAD_None, NULL);
	
	if (ColorSettings.OceanColor)
		ColorSettings.OceanColor->OnGradientUpdated.AddDynamic(this, &UColorSettings::ColorSettingsUpdated);
}

void UColorSettings::Init()
{
	if (ColorSettings.OceanColor && !ColorSettings.OceanColor->OnGradientUpdated.IsAlreadyBound(this, &UColorSettings::ColorSettingsUpdated))
		ColorSettings.OceanColor->OnGradientUpdated.AddDynamic(this, &UColorSettings::ColorSettingsUpdated);

	if (ColorSettings.BiomeColorSettings)
	{
		for (UBiome* Biome : ColorSettings.BiomeColorSettings->GetBiomes())
		{
			if (!Biome->OnBiomeHeightUpdated.IsAlreadyBound(ColorSettings.BiomeColorSettings, &UBiomeColorSettings::SortBiomesByHeight))
				Biome->OnBiomeHeightUpdated.AddDynamic(ColorSettings.BiomeColorSettings, &UBiomeColorSettings::SortBiomesByHeight);
			if (Biome->GetGradient() && !Biome->GetGradient()->OnGradientUpdated.IsAlreadyBound(Biome, &UBiome::BiomeUpdated))
				Biome->GetGradient()->OnGradientUpdated.AddDynamic(Biome, &UBiome::BiomeUpdated);
		}
	}
}

void UBiome::SetStartHeight(float NewHeight)
{
	Biome.StartHeight = NewHeight;
	OnBiomeHeightUpdated.Broadcast();
	OnSettingsAssetChanged.Broadcast();
}

void UBiome::SetGradient(UCurveLinearColor* NewGradient)
{
	if (Biome.Gradient)
		Biome.Gradient->OnGradientUpdated.RemoveDynamic(this, &UBiome::BiomeUpdated);
	Biome.Gradient = NewGradient;
	Biome.Gradient->OnGradientUpdated.AddDynamic(this, &UBiome::BiomeUpdated);
	OnSettingsAssetChanged.Broadcast();
}


UBiomeColorSettings::UBiomeColorSettings()
{
	for (UBiome* Biome : BiomeColorSettings.Biomes)
	{
		Biome->OnBiomeHeightUpdated.AddDynamic(this, &UBiomeColorSettings::SortBiomesByHeight);
		Biome->GetGradient()->OnGradientUpdated.AddDynamic(Biome, &UBiome::BiomeUpdated);
	}
}

void UBiomeColorSettings::SetUsingNoise(bool NewUsingNoise)
{
	BiomeColorSettings.bUsingNoise = NewUsingNoise;
	OnSettingsAssetChanged.Broadcast();
}

void UBiomeColorSettings::SetNoiseOffset(float NewNoiseOffset)
{
	BiomeColorSettings.NoiseOffset = NewNoiseOffset;
	OnSettingsAssetChanged.Broadcast();
}

void UBiomeColorSettings::SetNoiseStrength(float NewNoiseStrength)
{
	BiomeColorSettings.NoiseStrength = NewNoiseStrength;
	OnSettingsAssetChanged.Broadcast();
}

void UBiomeColorSettings::AddBiome(UBiome* NewBiome)
{
	BiomeColorSettings.Biomes.Add(NewBiome);
	NewBiome->OnBiomeHeightUpdated.AddDynamic(this, &UBiomeColorSettings::SortBiomesByHeight);
	OnSettingsAssetChanged.Broadcast();
}

void UBiomeColorSettings::RemoveBiome(int32 index)
{
	if (BiomeColorSettings.Biomes.IsValidIndex(index))
	{
		BiomeColorSettings.Biomes[index]->RemoveAppliedID(ID);
		BiomeColorSettings.Biomes.RemoveAt(index);
		OnSettingsAssetChanged.Broadcast();
	}
}

void UBiomeColorSettings::RemoveAllBiomes()
{
	for (UBiome* Biome : BiomeColorSettings.Biomes)
	{
		RemoveBiomeByRef(Biome);
	}
}

void UBiomeColorSettings::RemoveBiomeByRef(UBiome* ref)
{
	if (BiomeColorSettings.Biomes.Contains(ref))
	{
		ref->RemoveAppliedID(ID);
		BiomeColorSettings.Biomes.Remove(ref);
		OnSettingsAssetChanged.Broadcast();
	}
}

void UBiomeColorSettings::SortBiomesByHeight()
{
	Algo::Sort(BiomeColorSettings.Biomes, [](const UBiome* a, const UBiome* b) { return b->GetStartHeight() > a->GetStartHeight(); });
}

void UBiomeColorSettings::SetBlendAmount(float NewBlendAmount)
{
	BiomeColorSettings.blendAmount = NewBlendAmount;
	OnSettingsAssetChanged.Broadcast();
}

void UBiomeColorSettings::SetNoise(UNoiseSettings* NewNoise)
{
	BiomeColorSettings.Noise = NewNoise;
	OnSettingsAssetChanged.Broadcast();
}


void UColorSettings::SetBiomeColorSettings(UBiomeColorSettings* NewColorSettings)
{
	ColorSettings.BiomeColorSettings = NewColorSettings;
	OnSettingsAssetChanged.Broadcast();
}

void UColorSettings::SetTintPercent(float NewTintPercent)
{
	ColorSettings.BiomeTintPercent = NewTintPercent;
	OnSettingsAssetChanged.Broadcast();
}

void UColorSettings::SetOceanColor(UCurveLinearColor* NewOceanColor)
{
	if (ColorSettings.OceanColor)
		ColorSettings.OceanColor->OnGradientUpdated.RemoveDynamic(this, &UColorSettings::ColorSettingsUpdated);
	ColorSettings.OceanColor = NewOceanColor;
	ColorSettings.OceanColor->OnGradientUpdated.AddDynamic(this, &UColorSettings::ColorSettingsUpdated);
	OnSettingsAssetChanged.Broadcast();
}
