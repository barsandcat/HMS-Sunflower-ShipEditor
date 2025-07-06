// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ShipPlanCell.generated.h"

UCLASS()
class SHIPEDITOR_API AShipPlanCell : public AActor
{
	GENERATED_BODY()

public:
	int32 PartId;
};
