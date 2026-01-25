// Copyright (c) 2025, sillygilly. All rights reserved.

#include "ShipData/ShipPartAsset.h"

bool UShipPartAsset::RequiresWallConnection() const
{
	for (const FShipCellData& cell : Cells)
	{
		if (cell.CellType == ECellType::ROOT)
		{
			return true;
		}
	}
	return false;
}
