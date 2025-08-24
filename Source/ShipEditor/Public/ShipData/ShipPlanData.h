// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "ShipPlanData.generated.h"

class UShipPartData;

UCLASS()
class SHIPEDITOR_API UShipPlanData : public UObject
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TMap<FIntVector2, UShipPartData*> ShipParts;
};
