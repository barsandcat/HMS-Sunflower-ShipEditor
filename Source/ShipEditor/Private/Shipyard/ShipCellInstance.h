// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ShipData/ShipCellData.h"
#include "UObject/NoExportTypes.h"

class FShipRenderUpdate;

enum class EStructuralDeckStatus : uint8
{
	NONE = 0,
	ROOT = 1,
	CONNECTED = 2,
};

struct FShipCellInstance
{
	FShipCellInstance() = default;
	~FShipCellInstance() = default;
	FShipCellInstance(ECellType cell_type, FShipRenderUpdate* update)
	    : CellType(cell_type), Update(update)
	{
	}

	ECellType CellType = ECellType::NONE;
	EDeckType DeckType = EDeckType::NONE;

	FShipRenderUpdate* Update = nullptr;
	int32 Counter = 0;
};
