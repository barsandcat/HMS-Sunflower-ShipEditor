// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ShipData/ShipCellData.h"
#include "UObject/NoExportTypes.h"

class FShipRenderUpdate;

enum class ELoadBearing : uint8
{
	NONE = 0,
	ROOT = 1,
	CONNECTED = 2,
};

struct FShipCellInstance
{
	FShipCellInstance() = default;
	~FShipCellInstance() = default;
	FShipCellInstance(ECellType cell_type, ELoadBearing load_bearing, FShipRenderUpdate& update)
	    : CellType(cell_type), LoadBearing(load_bearing), Update(update)
	{
	}

	ECellType CellType = ECellType::NONE;

	ELoadBearing LoadBearing = ELoadBearing::NONE;

	FShipRenderUpdate& Update;
};
