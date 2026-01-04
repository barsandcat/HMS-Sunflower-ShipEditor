// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ShipData/ShipPartAsset.h"
#include "Shipyard/ShipPartTransform.h"
#include "UObject/NoExportTypes.h"

#include "ShipPartInstance.generated.h"

UCLASS()
class UShipPartInstance : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TObjectPtr<UShipPartAsset> PartAsset;

	UPROPERTY()
	FShipPartTransform Transform;
};
