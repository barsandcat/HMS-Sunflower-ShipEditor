// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Optional.h"
#include "ShipData/ShipCellData.h"
#include "ShipData/ShipDeviceAsset.h"
#include "ShipPartTransform.h"
#include "Shipyard/ShipDevicesUpdate.h"
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
	FShipStructureDevice(UShipDeviceAsset* asset, const FIntVector2& pos, TSharedPtr<FShipStructurePart> part)
	    : Asset(asset), Position(pos), Part(part)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UShipDeviceAsset> Asset;

	FIntVector2 Position;

	EDeviceType DeviceType = EDeviceType::NONE;
	TSharedPtr<FShipStructurePart> Part = nullptr;
	bool WallConnected = true;
	float Usage = 0.0f;
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
	bool IsTechnicalCorridor() const { return CellType == ECellType::TECHNICAL_CORRIDOR || CellType == ECellType::TECHNICAL_CORRIDOR_ROOT; }
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

	void AddArmor();

	void ConnectDecks();

	void SetUpdate(FShipRenderUpdate* update);
	void SetDevicesUpdate(FShipDevicesUpdate* devices_update);
	void CallUpdate() const;

	void ConnectFuel();

	void CalculateFuelConsumption(const TSet<TSharedPtr<FShipStructureDevice>>& device_set);

	TOptional<FIntVector2> Root;
	TMap<FIntVector2, TSharedPtr<FShipStructureCell>> Cells;
	TArray<TSharedPtr<FShipStructurePart>> Parts;
	TArray<TSharedPtr<FShipStructureDevice>> Devices;
	FShipDevicesUpdate* DevicesUpdate = nullptr;

	static inline const FIntVector2 dirs[4] = {FIntVector2(1, 0), FIntVector2(-1, 0), FIntVector2(0, 1), FIntVector2(0, -1)};
	static inline const FIntVector2 diagonal_dirs[4] = {FIntVector2(1, 1), FIntVector2(1, -1), FIntVector2(-1, 1), FIntVector2(-1, -1)};
	static inline const FIntVector2 vertical_dirs[2] = {FIntVector2(0, 1), FIntVector2(0, -1)};
	static inline const FIntVector2 horizontal_dirs[2] = {FIntVector2(1, 0), FIntVector2(-1, 0)};
};
