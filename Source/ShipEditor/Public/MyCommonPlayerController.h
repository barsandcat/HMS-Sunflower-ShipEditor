// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CommonPlayerController.h"
#include "CoreMinimal.h"

#include "MyCommonPlayerController.generated.h"

UCLASS()
class SHIPEDITOR_API AMyCommonPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()
public:
	virtual void PlayerTick(float DeltaTime) override;
	FVector GetMouseWorldPosition(double planeY);
};
