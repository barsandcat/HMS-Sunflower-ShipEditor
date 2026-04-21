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
}

FShipDevicesUpdate::~FShipDevicesUpdate()
{
	for (const FIntVector2& device_pos : CurrentCells)
	{
		VMDevices.RemoveDevice(device_pos);
	}
}
