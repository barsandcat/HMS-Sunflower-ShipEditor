// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ShipStructureData.h"
#include "UObject/NoExportTypes.h"

#include "ShipPartData.generated.h"

USTRUCT()
struct SHIPEDITOR_API FShipPartData
{
	GENERATED_BODY()

	UPROPERTY()
	FShipStructureData ShipStructure;
};
