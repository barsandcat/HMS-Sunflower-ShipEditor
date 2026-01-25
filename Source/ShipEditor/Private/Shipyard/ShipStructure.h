// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Optional.h"
#include "ShipData/ShipCellData.h"
#include "ShipData/ShipDeviceAsset.h"
#include "ShipPartTransform.h"
#include "Shipyard/ShipPartInstance.h"
#include "UObject/NoExportTypes.h"

class FShipRenderUpdate;
struct FShipStructureDevice;

struct FShipStructurePart
{
	FShipRenderUpdate* Update = nullptr;
	TSharedPtr<FShipStructureDevice> Device = nullptr;
};

struct FShipStructureDevice
{
	FShipStructureDevice() = default;
	~FShipStructureDevice() = default;
	FShipStructureDevice(EDeviceType device_type, bool load_bearing, TSharedPtr<FShipStructurePart> part)
	    : DeviceType(device_type), RequiresWallConnection(load_bearing), Part(part)
	{
	}
	EDeviceType DeviceType = EDeviceType::NONE;
	bool RequiresWallConnection = false;
	TSharedPtr<FShipStructurePart> Part = nullptr;
	bool WallConnected = false;
};

struct FShipStructureCell
{
	FShipStructureCell() = default;
	~FShipStructureCell() = default;
	FShipStructureCell(ECellType cell_type, TSharedPtr<FShipStructurePart> part, TSharedPtr<FShipStructureDevice> device)
	    : CellType(cell_type), Part(part), Device(device)
	{
	}

	ECellType CellType = ECellType::NONE;
	EDeckType DeckType = EDeckType::NONE;

	TSharedPtr<FShipStructurePart> Part;
	TSharedPtr<FShipStructureDevice> Device;
	int32 Visited = 0;
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

	TOptional<FIntVector2> Root;
	TMap<FIntVector2, TSharedPtr<FShipStructureCell>> Cells;
	TArray<TSharedPtr<FShipStructurePart>> Parts;
	TArray<TSharedPtr<FShipStructureDevice>> Devices;
};
