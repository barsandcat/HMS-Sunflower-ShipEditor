// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "VMDeviceStatus.h"

#include "VMDevices.generated.h"

UCLASS()
class UVMDevices : public UMVVMViewModelBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	TArray<TObjectPtr<UVMDeviceStatus>> DeviceList;

public:
	void SetDeviceList(const TArray<TObjectPtr<UVMDeviceStatus>>& device_list);
	const TArray<TObjectPtr<UVMDeviceStatus>>& GetDeviceList() const;
};
