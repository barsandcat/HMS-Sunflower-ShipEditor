// Copyright (c) 2025, sillygilly. All rights reserved.

#include "ShipData/ShipPartAsset.h"

bool UShipPartAsset::RequiresWallConnection() const
{
	for (const FShipCellData& cell : Cells)
	{
		if (IsDeckRootCell(cell.CellType))
		{
			return true;
		}
	}
	return false;
}
