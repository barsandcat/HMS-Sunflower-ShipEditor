// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ShipData/ShipCellData.h"
#include "ShipPartTransform.h"
#include "Shipyard/ShipPartInstance.h"
#include "UObject/NoExportTypes.h"

class FShipRenderUpdate;

enum class EStructuralDeckStatus : uint8
{
	NONE = 0,
	ROOT = 1,
	CONNECTED = 2,
};

struct FShipStructurePart
{
	bool LoadBearing = false;
	int32 Counter = 0;
	FShipRenderUpdate* Update = nullptr;
};

struct FShipStructureCell
{
	FShipStructureCell() = default;
	~FShipStructureCell() = default;
	FShipStructureCell(ECellType cell_type, TSharedPtr<FShipStructurePart> part)
	    : CellType(cell_type), Part(part)
	{
	}

	ECellType CellType = ECellType::NONE;
	EDeckType DeckType = EDeckType::NONE;

	TSharedPtr<FShipStructurePart> Part;
	int32 Counter = 0;
};

struct FShipStructure
{
	FShipStructure() = default;
	~FShipStructure() = default;
	FShipStructure(const FShipPartTransform& render_transform, const TArray<TObjectPtr<UShipPartInstance>>& part_instances);

	static bool MergeStructures(const FShipStructure& structure_a,
	    const FShipStructure& structure_b,
	    FShipStructure& out_merged_structure);

	void Process();

	void SetUpdate(FShipRenderUpdate* update);
	void CallUpdate() const;

	TMap<FIntVector2, TSharedPtr<FShipStructureCell>> Cells;
	TArray<TSharedPtr<FShipStructurePart>> Parts;
};
