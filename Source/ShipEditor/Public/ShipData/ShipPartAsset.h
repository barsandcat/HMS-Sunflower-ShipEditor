// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ShipCellData.h"

#include "ShipPartAsset.generated.h"

class UShipDeviceAsset;

UCLASS()
class SHIPEDITOR_API UShipPartAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText PartName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PartId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool LoadBearing = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UShipDeviceAsset> Device;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FShipCellData> Cells;
};
