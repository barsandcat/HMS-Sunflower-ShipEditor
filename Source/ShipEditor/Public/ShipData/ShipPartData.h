// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "ShipPartData.generated.h"


class UShipStructureData;
UCLASS()
class SHIPEDITOR_API UShipPartData : public UObject
{
	GENERATED_BODY()
private:
	UPROPERTY()
	UShipStructureData* ShipStructure;
};
