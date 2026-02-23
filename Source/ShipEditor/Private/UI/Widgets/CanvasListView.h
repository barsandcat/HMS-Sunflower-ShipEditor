// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "Components/CanvasPanel.h"
#include "CoreMinimal.h"
#include "UI/ViewModels/VMDeviceStatus.h"

#include "CanvasListView.generated.h"

class UDeviceStatusWidget;

// A container for a map of device widgets that can position them selfs using their own transform
// Support mvvm
// Widgets stored in a map - device vm to widget
// Device widget has a function to initialize it with a device vm
UCLASS()
class UCanvasListView : public UCanvasPanel
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Canvas List View")
	TSubclassOf<UDeviceStatusWidget> DeviceWidgetClass;

	// * Iterate - Remove unnecessary widgets
	// * Iterate - Add missing widgets. Construct widgets and set their view model
	// Keep the ones that are there.
	UFUNCTION(BlueprintCallable)
	void SetList(const TArray<UVMDeviceStatus*>& device_list);

private:
	UPROPERTY(Transient)
	TMap<TObjectPtr<UVMDeviceStatus>, TObjectPtr<UDeviceStatusWidget>> DeviceWidgetMap;
};
