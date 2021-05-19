// Copyright Soren Gilbertson


#include "NoiseSettings.h"

UNoiseSettings::UNoiseSettings()
{
}

UNoiseSettings::~UNoiseSettings()
{
}

void UNoiseSettings::SetFilterType(EFilterType NewFilterType)
{
	FilterType = NewFilterType;
	OnNoiseSettingsChanged.ExecuteIfBound();
}

void UNoiseSettings::SetSimpleNoiseSettings(FSimpleNoiseSettings NewSettings)
{
	SimpleNoiseSettings = NewSettings;
	OnNoiseSettingsChanged.ExecuteIfBound();
}

void UNoiseSettings::SetRidgidNoiseSettings(FRidgidNoiseSettings NewSettings)
{
	RidgidNoiseSettings = NewSettings;
	OnNoiseSettingsChanged.ExecuteIfBound();
}
