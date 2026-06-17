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

enum class EShipStructureVisitState : uint8
{
	None,
	ConnectedDeck,
	CanReachBridge,
	ArmorPlacement,
	TechnicalCorridor,
};

bool IsFloor(const FIntVector3& cell_pos);
bool IsWall(const FIntVector3& cell_pos);
bool IsBackgroundWall(const FIntVector3& cell_pos);
bool IsForegroundWall(const FIntVector3& cell_pos);

struct FShipStructureDevice
{
	FShipStructureDevice() = default;
	~FShipStructureDevice() = default;
	FShipStructureDevice(const FDeviceStats& stats, const FShipPartTransform& transform, float rotation, FShipRenderUpdate* update)
	    : Stats(stats)
	    , Transform(transform)
	    , Update(update)
	    , Sector(rotation, FMath::Clamp(stats.SectorWidth, 0.0f, kFullSectorDegrees))
	{
	}

	FDeviceStats Stats;

	FShipPartTransform Transform;

	FShipRenderUpdate* Update = nullptr;

	bool CanPhoneTheBridge = false;
	bool RequiresPhoneConnection = false;
	bool CanReachTheBridge = false;
	bool IsPartOfTheShip() const
	{
		return Stats.DeviceType == EDeviceType::BRIDGE || CanReachTheBridge && (CanPhoneTheBridge || !RequiresPhoneConnection);
	}
	float Usage = 0.0f;
	FDeviceSector Sector;
	FDeviceSector AvailableSector;
	TArray<FDeviceSector> AvailableSectors;
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

	TSharedPtr<FShipStructureDevice> Device;    // Multiple cells point to same device
	EShipStructureVisitState Visited = EShipStructureVisitState::None;
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

	void SetCanReachTheBridge();

	void AddArmor();

	void ConnectDecks();

	void SetDevicesUpdate(FShipDevicesUpdate* devices_update);
	void CallUpdate() const;

	void ConnectTechnicalCorridors();
	void CalculateDeviceSectors();

	void CalculateFuelConsumption(const TSet<TSharedPtr<FShipStructureDevice>>& device_set);
	void CalculateAmmoConsumption(const TSet<TSharedPtr<FShipStructureDevice>>& device_set);

	ECellType GetCellType(const FIntVector3& pos) const;

	TOptional<FIntVector3> Root;
	TMap<FIntVector3, TSharedPtr<FShipStructureCell>> Cells;
	TArray<TSharedPtr<FShipStructureDevice>> Devices;
	FShipDevicesUpdate* DevicesUpdate = nullptr;

	static inline const FIntVector3 DIRS[4] = {FIntVector3(1, 0, 0), FIntVector3(-1, 0, 0), FIntVector3(0, 1, 0), FIntVector3(0, -1, 0)};
	static inline const FIntVector3 DIRS3D[6] = {FIntVector3(1, 0, 0), FIntVector3(-1, 0, 0), FIntVector3(0, 1, 0), FIntVector3(0, -1, 0), FIntVector3(0, 0, 1), FIntVector3(0, 0, -1)};
	static inline const FIntVector3 DIAGONAL_DIRS[4] = {FIntVector3(1, 1, 0), FIntVector3(1, -1, 0), FIntVector3(-1, 1, 0), FIntVector3(-1, -1, 0)};
	static inline const FIntVector3 VERTICAL_DIRS[2] = {FIntVector3(0, 1, 0), FIntVector3(0, -1, 0)};
	static inline const FIntVector3 HORIZONTAL_DIRS[2] = {FIntVector3(1, 0, 0), FIntVector3(-1, 0, 0)};
};
