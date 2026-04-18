#include "ShipDevicesUpdate.h"

FShipDevicesUpdate::FShipDevicesUpdate(UVMDevices& devices)
    : VMDevices(devices)
{
	CurrentCells = devices.GetCurrentCells();
}

void FShipDevicesUpdate::SetDeviceStatus(const FText& name, const FIntVector2& device_pos, float usage, const FDeviceSector& sector, const FDeviceSector& available_sector)
{
	CurrentCells.Remove(device_pos);
	VMDevices.UpdateDeviceStatus(name, device_pos, usage, sector, available_sector);
	if (sector.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Device %s at (%d, %d) usage: %.2f%% sector: %s available sector: %s"),
		    *name.ToString(), device_pos.X, device_pos.Y, usage * 100.0f, *sector.ToDebugString(), *available_sector.ToDebugString());
	}
}

FShipDevicesUpdate::~FShipDevicesUpdate()
{
	for (const FIntVector2& device_pos : CurrentCells)
	{
		VMDevices.RemoveDevice(device_pos);
	}
}
