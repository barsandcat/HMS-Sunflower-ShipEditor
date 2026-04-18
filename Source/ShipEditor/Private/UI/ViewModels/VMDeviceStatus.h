// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Shipyard/ShipDeviceSector.h"

#include "VMDeviceStatus.generated.h"

UCLASS()
class UVMDeviceStatus : public UMVVMViewModelBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	FText Name;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	float Usage = 0.0f;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	FIntVector2 CellId;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	FDeviceSector AvailableSector;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	FDeviceSector Sector;

public:
	void SetName(const FText& name);
	FText GetName() const;

	void SetUsage(float usage);
	float GetUsage() const;

	void SetCellId(const FIntVector2& cell_id);
	FIntVector2 GetCellId() const;

	void SetAvailableSector(const FDeviceSector& sector);
	FDeviceSector GetAvailableSector() const;

	void SetSector(const FDeviceSector& sector);
	FDeviceSector GetSector() const;
};
