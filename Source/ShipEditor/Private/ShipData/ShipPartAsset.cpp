// Copyright (c) 2025, sillygilly. All rights reserved.

#include "ShipData/ShipPartAsset.h"

bool UShipPartAsset::RequiresPhoneConnection() const
{
	for (const FShipCellData& cell : Cells)
	{
		if (IsIntersectionRootCell(cell.CellType))
		{
			return true;
		}
	}
	return false;
}
