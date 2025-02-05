// Copyright Soren Gilbertson

#pragma once

#include "CoreMinimal.h"
#include "INoiseFilter.h"
#include "SimpleNoiseFilter.h"
#include "RidgidNoiseFilter.h"
#include "NoiseSettings.h"

/**
 * 
 */
class MINISOLARSYSTEM_API NoiseFilterFactory
{
public:
	static INoiseFilter* CreateNoiseFilter(UNoiseSettings* NoiseSettings);
};
