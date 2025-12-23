// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "ShipPlanData.generated.h"

USTRUCT()
struct SHIPEDITOR_API FShipPlanData
{
	GENERATED_BODY()

	UPROPERTY()
	FString Name;
};
