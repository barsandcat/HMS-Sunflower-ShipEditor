// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CommonPlayerController.h"
#include "CoreMinimal.h"
#include "Misc/Optional.h"

#include "MyCommonPlayerController.generated.h"

UCLASS()
class SHIPEDITOR_API AMyCommonPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()
public:
	virtual void PlayerTick(float delta_time) override;
	TOptional<FVector> GetMouseWorldPosition(double plane_y);
};
