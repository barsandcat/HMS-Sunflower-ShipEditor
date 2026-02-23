// Copyright (c) 2026, sillygilly. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CommonActivatableWidget.h"
#include "CommonUIExtensions.h"
#include "CoreMinimal.h"
#include "UI/ViewModels/VMDeviceStatus.h"

#include "DeviceStatusWidget.generated.h"

UCLASS(Blueprintable, Abstract)
class UDeviceStatusWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Device")
	void InitializeWithDeviceVM(UVMDeviceStatus* device_vm);
};
