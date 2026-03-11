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

struct FShipStructureDevice
{
	FShipStructureDevice() = default;
	~FShipStructureDevice() = default;
	FShipStructureDevice(const FDeviceStats& stats, const FIntVector2& pos)
	    : Stats(stats), Position(pos)
	{
	}

	FDeviceStats Stats;

	FIntVector2 Position;

	bool CanPhoneTheBridge = false;
	bool RequiresPhoneConnection = false;
	bool CanReachTheBridge = false;
	bool IsPartOfTheShip() const
	{
		return Stats.DeviceType == EDeviceType::BRIDGE || CanReachTheBridge && (CanPhoneTheBridge || !RequiresPhoneConnection);
	}
	float Usage = 0.0f;
};

struct FShipStructureCell
{
	FShipStructureCell() = default;
	~FShipStructureCell() = default;
	FShipStructureCell(ECellType cell_type, TSharedPtr<FShipStructureDevice> device, FShipRenderUpdate* update)
	    : CellType(cell_type), Device(device), Update(update)
	{
	}

	ECellType CellType = ECellType::NONE;

	TSharedPtr<FShipStructureDevice> Device;
	int32 Visited = 0;
	FShipRenderUpdate* Update = nullptr;
};

struct FShipStructure
{
	FShipStructure() = default;
	~FShipStructure() = default;
	FShipStructure(const FShipPartTransform& render_transform, const TArray<TObjectPtr<UShipPartInstance>>& part_instances, FShipRenderUpdate* update);

	static bool MergeStructures(const FShipStructure& structure_a,
	    const FShipStructure& structure_b,
	    FShipStructure& out_merged_structure);

	void Process();

	void AddArmor();

	void ConnectDecks();

	void SetDevicesUpdate(FShipDevicesUpdate* devices_update);
	void CallUpdate() const;

	void ConnectFuel();

	void CalculateFuelConsumption(const TSet<TSharedPtr<FShipStructureDevice>>& device_set);
	void CalculateAmmoConsumption(const TSet<TSharedPtr<FShipStructureDevice>>& device_set);

	TOptional<FIntVector2> Root;
	TMap<FIntVector2, TSharedPtr<FShipStructureCell>> Cells;
	TArray<TSharedPtr<FShipStructureDevice>> Devices;
	FShipDevicesUpdate* DevicesUpdate = nullptr;

	static inline const FIntVector2 dirs[4] = {FIntVector2(1, 0), FIntVector2(-1, 0), FIntVector2(0, 1), FIntVector2(0, -1)};
	static inline const FIntVector2 diagonal_dirs[4] = {FIntVector2(1, 1), FIntVector2(1, -1), FIntVector2(-1, 1), FIntVector2(-1, -1)};
	static inline const FIntVector2 vertical_dirs[2] = {FIntVector2(0, 1), FIntVector2(0, -1)};
	static inline const FIntVector2 horizontal_dirs[2] = {FIntVector2(1, 0), FIntVector2(-1, 0)};
};
