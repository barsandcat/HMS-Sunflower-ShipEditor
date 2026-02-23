// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/Widgets/CanvasListView.h"

#include "Components/CanvasPanelSlot.h"
#include "UI/Widgets/DeviceStatusWidget.h"

void UCanvasListView::SetList(const TArray<UVMDeviceStatus*>& device_list)
{
	TSet<UVMDeviceStatus*> new_devices(device_list);

	TArray<UVMDeviceStatus*> deleted_devices;
	for (const TPair<TObjectPtr<UVMDeviceStatus>, TObjectPtr<UDeviceStatusWidget>>& pair : DeviceWidgetMap)
	{
		if (!new_devices.Contains(pair.Key))
		{
			deleted_devices.Add(pair.Key);
		}
		else
		{
			new_devices.Remove(pair.Key);
		}
	}

	for (UVMDeviceStatus* device_vm : deleted_devices)
	{
		if (UDeviceStatusWidget* widget = DeviceWidgetMap.FindRef(device_vm))
		{
			RemoveChild(widget);
		}
		DeviceWidgetMap.Remove(device_vm);
	}

	if (!DeviceWidgetClass)
	{
		return;
	}

	for (UVMDeviceStatus* device_vm : new_devices)
	{
		UDeviceStatusWidget* widget = CreateWidget<UDeviceStatusWidget>(this, DeviceWidgetClass);
		UCanvasPanelSlot* CanvasSlot = AddChildToCanvas(widget);
		CanvasSlot->SetAutoSize(true);
		widget->InitializeWithDeviceVM(device_vm);
		DeviceWidgetMap.Add(device_vm, widget);
	}
}
