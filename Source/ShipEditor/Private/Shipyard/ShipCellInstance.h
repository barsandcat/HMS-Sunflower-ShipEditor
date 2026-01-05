// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ShipData/ShipCellData.h"
#include "UObject/NoExportTypes.h"

#include "ShipCellInstance.generated.h"

class AShipPlanRender;

UENUM()
enum class ELoadBearing : uint8
{
	NONE = 0,
	ROOT = 1,
	CONNECTED = 2,
};

USTRUCT()
struct FShipCellInstance
{
	GENERATED_BODY()

	FShipCellInstance() = default;
	~FShipCellInstance() = default;
	FShipCellInstance(ECellType cell_type, ELoadBearing load_bearing, AShipPlanRender* target_render)
	    : CellType(cell_type), LoadBearing(load_bearing), TargetRender(target_render)
	{
	}

	UPROPERTY()
	ECellType CellType = ECellType::NONE;

	UPROPERTY()
	ELoadBearing LoadBearing = ELoadBearing::NONE;

	UPROPERTY()
	TObjectPtr<AShipPlanRender> TargetRender;
};
