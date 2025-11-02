// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ShipPartAsset.generated.h"


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
	int32 CategoryId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Elevation = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DynamicMount = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool LoadBearing = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FIntVector2, bool> Walls;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FIntVector2, bool> Floors;
};
