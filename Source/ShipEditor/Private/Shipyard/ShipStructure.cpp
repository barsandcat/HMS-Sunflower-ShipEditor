// Copyright (c) 2025, sillygilly. All rights reserved.

#include "Shipyard/ShipStructure.h"

#include "Shipyard/ShipDeviceSector.h"
#include "Shipyard/ShipPartInstance.h"
#include "Shipyard/ShipPlanRender.h"

namespace
{

bool IsDeckIntersection(const FIntVector3& pos)
{
	return pos.X % 2 != 0 && pos.Y % 2 != 0;
}

bool IsDeckVertical(const FIntVector3& pos)
{
	return pos.X % 2 != 0 && pos.Y % 2 == 0;
}

bool IsDeckHorizontal(const FIntVector3& pos)
{
	return pos.X % 2 == 0 && pos.Y % 2 != 0;
}

bool IsCabin(const FIntVector3& pos)
{
	return pos.X % 2 == 0 && pos.Y % 2 == 0;
}

}    // namespace

FShipStructure::FShipStructure(const FShipPartTransform& render_transform, const TArray<TObjectPtr<UShipPartInstance>>& part_instances, FShipRenderUpdate* update)
{
	Devices.SetNum(part_instances.Num());
	for (int32 i = 0; i < part_instances.Num(); i++)
	{
		UShipPartInstance* part_instance = part_instances[i];
		FRotator part_rotation = render_transform(part_instance->Transform).ToRotator();
		float rotation = NormalizeSectorAngle(part_rotation.Pitch);
		TSharedPtr<FShipStructureDevice> device =
		    MakeShared<FShipStructureDevice>(part_instance->PartAsset->Device->Stats,
		        render_transform(part_instance->Transform), rotation, update);
		Devices[i] = device;
		for (FShipCellData& cell : part_instance->PartAsset->Cells)
		{
			FIntVector2 cell_pos = render_transform(part_instance->Transform(cell.Position));
			if (IsIntersectionRootCell(cell.CellType))
			{
				device->RequiresPhoneConnection = true;
				if (device->Stats.DeviceType == EDeviceType::BRIDGE)
				{
					Root = {cell_pos.X, cell_pos.Y, 0};
				}
			}

			// int32 to = part_instance->PartAsset->Height;
			// int32 from = -to;
			// for (int32 z = from; z <= to; z++)
			//{
			Cells.Add({cell_pos.X, cell_pos.Y, 0}, MakeShared<FShipStructureCell>(cell.CellType, device, update));
			//}
		}
	}
}

bool FShipStructure::MergeStructures(const FShipStructure& structure_a, const FShipStructure& structure_b, FShipStructure& out_merged_structure)
{
	if (structure_a.Root.IsSet() && structure_b.Root.IsSet())
	{
		return false;
	}

	out_merged_structure = structure_a;
	out_merged_structure.Cells.Reserve(structure_a.Cells.Num() + structure_b.Cells.Num());
	out_merged_structure.Devices.Reserve(structure_a.Devices.Num() + structure_b.Devices.Num());
	out_merged_structure.Root = structure_a.Root.IsSet() ? structure_a.Root : structure_b.Root;

	for (const auto& i : structure_b.Devices)
	{
		out_merged_structure.Devices.Add(i);
	}

	for (const auto& i : structure_b.Cells)
	{
		if (out_merged_structure.Cells.Contains(i.Key))
		{
			return false;
		}
		else
		{
			out_merged_structure.Cells.Add(i.Key, i.Value);
		}
	}
	return true;
}

void FShipStructure::Process()
{
	if (!Root)
	{
		// No root, nothing to process
		return;
	}
	// Example structure:
	// Y
	// 3
	// 2  C | C | C
	// 1  -   - * -
	// 0  C | C | C
	//-1
	//-2
	//   -2-1 0 1 2 X
	//
	// Find root (*), should be only one
	// Iterate BFS from root along decks (|, -) setting deck type to structural
	// Iterate BFS from root along cabins (C) to mark reachable devices
	// Add armor between reachable cabins and empty exterior

	ConnectDecks();

	// Mark devices as part of the ship.
	SetCanReachTheBridge();

	AddArmor();

	ConnectTechnicalCorridors();

	CalculateDeviceSectors();
}

void FShipStructure::SetCanReachTheBridge()
{
	TArray<FIntVector3> cabin_queue;
	cabin_queue.Reserve(Cells.Num());
	int32 cabin_queue_head = 0;

	// Enqueue cabin neighbors of root
	for (const FIntVector3& d : DIAGONAL_DIRS)
	{
		FIntVector3 neighbor_cabin_pos = *Root + d;
		if (TSharedPtr<FShipStructureCell> neighbor_cell = Cells.FindRef(neighbor_cabin_pos))
		{
				if (IsCabinTraversableCell(neighbor_cell->CellType))
				{
					neighbor_cell->Visited = EShipStructureVisitState::CanReachBridge;
					neighbor_cell->Device->CanReachTheBridge = true;
					cabin_queue.Add(neighbor_cabin_pos);
				}
		}
	}

	while (cabin_queue_head < cabin_queue.Num())
	{
		FIntVector3 cabin_pos = cabin_queue[cabin_queue_head++];

		for (const FIntVector3& d : DIRS)
		{
			FIntVector3 neighbor_cabin_pos = cabin_pos + d * 2;

			TSharedPtr<FShipStructureCell> neighbor_cabin = Cells.FindRef(neighbor_cabin_pos);

			// Add neighbor to BFS if it's cabin and not visited yet
			if (neighbor_cabin && IsCabinTraversableCell(neighbor_cabin->CellType) && (neighbor_cabin->Device->CanPhoneTheBridge || !neighbor_cabin->Device->RequiresPhoneConnection))
			{
				if (neighbor_cabin->Visited != EShipStructureVisitState::CanReachBridge)
				{
					neighbor_cabin->Visited = EShipStructureVisitState::CanReachBridge;
					neighbor_cabin->Device->CanReachTheBridge = true;
					cabin_queue.Add(neighbor_cabin_pos);
				}
			}
		}
	}
}

ECellType FShipStructure::GetCellType(const FIntVector3& pos) const
{
	if (TSharedPtr<FShipStructureCell> cell = Cells.FindRef(pos))
	{
		return cell->CellType;
	}
	return ECellType::NONE;
}

void FShipStructure::AddArmor()
{
	TArray<FIntVector3> cabin_queue;
	cabin_queue.Reserve(Cells.Num());
	int32 cabin_queue_head = 0;

	for (const FIntVector3& d : DIAGONAL_DIRS)
	{
		FIntVector3 neighbor_cabin_pos = *Root + d;
		if (TSharedPtr<FShipStructureCell> neighbor_cell = Cells.FindRef(neighbor_cabin_pos))
		{
			if (IsCabinTraversableCell(neighbor_cell->CellType))
			{
				neighbor_cell->Visited = EShipStructureVisitState::ArmorPlacement;
				cabin_queue.Add(neighbor_cabin_pos);
			}
		}
	}

	while (cabin_queue_head < cabin_queue.Num())
	{
		FIntVector3 cabin_pos = cabin_queue[cabin_queue_head++];
		TSharedPtr<FShipStructureCell> cell = Cells.FindRef(cabin_pos);

		if (cell && IsCabinCell(cell->CellType))
		{
			for (const FIntVector3& d : DIRS3D)
			{
				FIntVector3 neighbor_cabin_pos = cabin_pos + d * 2;
				FIntVector3 neighbor_deck_pos = cabin_pos + d;

				TSharedPtr<FShipStructureCell> neighbor_cabin = Cells.FindRef(neighbor_cabin_pos);
				TSharedPtr<FShipStructureCell> neighbor_deck = Cells.FindRef(neighbor_deck_pos);

				if ((!neighbor_cabin || neighbor_cabin->CellType == ECellType::NONE) && !neighbor_deck)
				{
					TSharedPtr<FShipStructureCell> armor_cell = MakeShared<FShipStructureCell>(ECellType::DECK_ARMOR, cell->Device, cell->Update);
					Cells.Add(neighbor_deck_pos, armor_cell);
				}

				if (neighbor_cabin && IsCabinTraversableCell(neighbor_cabin->CellType) && (neighbor_cabin->Device->CanPhoneTheBridge || !neighbor_cabin->Device->RequiresPhoneConnection))
				{
					if (neighbor_cabin->Visited != EShipStructureVisitState::ArmorPlacement)
					{
						neighbor_cabin->Visited = EShipStructureVisitState::ArmorPlacement;
						cabin_queue.Add(neighbor_cabin_pos);
					}
				}
			}
		}
	}
}

void FShipStructure::ConnectDecks()
{
	// BFS along decks to mark structural decks
	TArray<FIntVector3> deck_queue;
	deck_queue.Reserve(Cells.Num());
	int32 deck_queue_head = 0;

	TSet<FIntVector3> visited_intersections;

	// Enqueue root
	deck_queue.Add(*Root);
	visited_intersections.Add(*Root);

	// Process deck BFS
	while (deck_queue_head < deck_queue.Num())
	{
		FIntVector3 deck_pos = deck_queue[deck_queue_head++];
		TSharedPtr<FShipStructureCell> cell = Cells.FindRef(deck_pos);

		if (IsDeckIntersection(deck_pos))
		{
			if (cell && IsIntersectionRootCell(cell->CellType))
			{
				cell->Device->CanPhoneTheBridge = true;
			}

			for (const FIntVector3& d : DIRS)
				{
					FIntVector3 neighbor_deck_pos = deck_pos + d;
					TSharedPtr<FShipStructureCell> neighbor_cell = Cells.FindRef(neighbor_deck_pos);
					if (neighbor_cell && neighbor_cell->Visited != EShipStructureVisitState::ConnectedDeck && IsDeckCell(neighbor_cell->CellType))
					{
						neighbor_cell->Visited = EShipStructureVisitState::ConnectedDeck;
						deck_queue.Add(neighbor_deck_pos);
					}
				}
		}

		if (cell && cell->CellType == ECellType::DECK)
		{
			cell->CellType = ECellType::DECK_PHONE_LINE;
		}

		if (cell)
		{
			if (IsDeckVertical(deck_pos))
			{
				for (const FIntVector3& d : VERTICAL_DIRS)
				{
					FIntVector3 neighbor_deck_pos = deck_pos + d;
					if (!visited_intersections.Contains(neighbor_deck_pos))
					{
						deck_queue.Add(neighbor_deck_pos);
					}
				}
			}
			if (IsDeckHorizontal(deck_pos))
			{
				for (const FIntVector3& d : HORIZONTAL_DIRS)
				{
					FIntVector3 neighbor_deck_pos = deck_pos + d;
					if (!visited_intersections.Contains(neighbor_deck_pos))
					{
						deck_queue.Add(neighbor_deck_pos);
					}
				}
			}
		}
	}
}

void FShipStructure::SetDevicesUpdate(FShipDevicesUpdate* devices_update)
{
	DevicesUpdate = devices_update;
}

void FShipStructure::CallUpdate() const
{
	for (const auto& i : Cells)
	{
		const TSharedPtr<FShipStructureCell> cell = i.Value;
		cell->Update->SetCellMesh(i.Key, cell->CellType);
	}

	for (TSharedPtr<FShipStructureDevice> device : Devices)
	{
		if (device->Stats.FuelConsumption != 0.0f || device->Stats.AmmoConsumption != 0.0f || device->Stats.SectorWidth > 0.0f)
		{
			DevicesUpdate->SetDeviceStatus(device->Stats.DeviceName, device->Transform.Position, device->Usage, device->Sector, device->AvailableSector);
		}
		device->Update->SetDeviceMesh(device->Transform, device->Stats.Mesh);
	}
}

void FShipStructure::ConnectTechnicalCorridors()
{
	// Gather unvisited technical corridor roots that belong to the ship (device is wall-connected)
	TSet<FIntVector3> unvisited_roots;
	for (const auto& pair : Cells)
	{
		const TSharedPtr<FShipStructureCell> cell = pair.Value;
		if (cell && cell->CellType == ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT)
		{
			unvisited_roots.Add(pair.Key);
		}
	}

	// Storage for discovered subnetworks (each subnetwork is an set of device pointers)
	TArray<TSet<TSharedPtr<FShipStructureDevice>>> subnetworks;
	subnetworks.Reserve(unvisited_roots.Num());

	// BFS along technical corridors to build subnetworks
	while (unvisited_roots.Num() > 0)
	{
		// Devices that belong to the current subnetwork
		TSet<TSharedPtr<FShipStructureDevice>> devices_in_net;
		devices_in_net.Reserve(8);

		// Pick a start root from the set
		FIntVector3 start_pos = unvisited_roots.Array()[0];
		unvisited_roots.Remove(start_pos);

		// BFS queue of corridor positions
		TArray<FIntVector3> queue;
		queue.Add(start_pos);
		int32 queue_head = 0;

			// Mark the start position as visited for fuel traversal.
			if (TSharedPtr<FShipStructureCell> start_cell = Cells.FindRef(start_pos))
			{
				start_cell->Visited = EShipStructureVisitState::TechnicalCorridor;
				devices_in_net.Add(start_cell->Device);
			}

		// Traverse
		while (queue_head < queue.Num())
		{
			FIntVector3 pos = queue[queue_head++];
			TSharedPtr<FShipStructureCell> cell = Cells.FindRef(pos);
			if (cell)
			{
				// Explore 4 cardinal neighbors for technical corridor connectivity
				for (const FIntVector3& d : DIRS)
				{
					FIntVector3 neighbor_pos = pos + d * 2;
						if (TSharedPtr<FShipStructureCell> neighbor_cell = Cells.FindRef(neighbor_pos))
						{
							// Only traverse along technical corridor cells or corridor roots
							if (IsTechnicalCorridorCell(neighbor_cell->CellType) && cell->Device->IsPartOfTheShip() == neighbor_cell->Device->IsPartOfTheShip() && neighbor_cell->Visited != EShipStructureVisitState::TechnicalCorridor)
							{
								neighbor_cell->Visited = EShipStructureVisitState::TechnicalCorridor;
								queue.Add(neighbor_pos);

							// If this neighbor is a corridor root that was in the unvisited set, remove it
							if (neighbor_cell->CellType == ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT)
							{
								unvisited_roots.Remove(neighbor_pos);
								// Record neighbor's device
								devices_in_net.Add(neighbor_cell->Device);
							}
						}
					}
				}
			}
		}

		// Store discovered subnetwork devices for later processing
		subnetworks.Add(MoveTemp(devices_in_net));
	}

	for (const TSet<TSharedPtr<FShipStructureDevice>>& device_set : subnetworks)
	{
		CalculateFuelConsumption(device_set);
		CalculateAmmoConsumption(device_set);
	}
}

void FShipStructure::CalculateDeviceSectors()
{
	for (const auto& pair : Cells)
	{
		FIntVector2 cabin_pos = {pair.Key.X, pair.Key.Y};
		const TSharedPtr<FShipStructureCell>& cell = pair.Value;
		if (cell && IsCabinCell(cell->CellType))
		{
			for (const TSharedPtr<FShipStructureDevice>& device : Devices)
			{
				if (device->Sector.IsValid())
				{
					if (cell->CellType != ECellType::CABIN_BLOCKED || device != cell->Device)
					{
						const FDeviceSector available_sector = FindAvailableSector(device->Transform.Position, cabin_pos);
						if (available_sector.IsValid())
						{
							device->AvailableSectors.Add(available_sector);
						}
					}
				}
			}
		}
	}

	for (const TSharedPtr<FShipStructureDevice>& device : Devices)
	{
		if (device->Sector.IsValid())
		{
			device->AvailableSectors.Sort([&device](const FDeviceSector& a, const FDeviceSector& b)
			    {
					const float distance_a = FMath::Abs(DeltaSectorAngle(a.Rotation, device->Sector.Rotation));
					const float distance_b = FMath::Abs(DeltaSectorAngle(b.Rotation, device->Sector.Rotation));

				if (!FMath::IsNearlyEqual(distance_a, distance_b))
				{
					return distance_a < distance_b;
				}

				return a.Width > b.Width; });

			FDeviceSector common_sector = device->Sector;
			for (const FDeviceSector& available_sector : device->AvailableSectors)
			{
				common_sector = FindCommonSector(common_sector, available_sector);
				if (!common_sector.IsValid())
				{
					break;
				}
			}

			device->AvailableSector = common_sector;
		}
	}
}

void FShipStructure::CalculateFuelConsumption(const TSet<TSharedPtr<FShipStructureDevice>>& device_set)
{
	// Sum production and consumption in this subnetwork
	float total_production = 0.0f;
	float total_consumption = 0.0f;

	for (const TSharedPtr<FShipStructureDevice>& device : device_set)
	{
		if (device->Stats.FuelConsumption > 0.0f)
		{
			total_consumption -= device->Stats.FuelConsumption;
		}
		if (device->Stats.FuelConsumption < 0.0f)
		{
			total_production -= device->Stats.FuelConsumption;
		}
	}

	// Determine fractions
	float used_fraction = 0.0f;
	if (total_production > 0.0f)
	{
		used_fraction = FMath::Min(1.0f, total_production / -total_consumption);
	}

	float satisfaction = 1.0f;
	if (total_consumption < 0.0f)
	{
		satisfaction = FMath::Min(1.0f, -total_consumption / total_production);
	}

	// Apply to devices
	for (const TSharedPtr<FShipStructureDevice>& device : device_set)
	{
		// Producers: set how much of their production is actually used (absolute amount)
		if (device->Stats.FuelConsumption > 0.0f)
		{
			// Absolute amount of fuel that this device supplies to consumers
			device->Usage = used_fraction;
		}

		// Consumers: set fraction of consumption satisfied
		if (device->Stats.FuelConsumption < 0.0f)
		{
			device->Usage = satisfaction;
		}
	}
}

void FShipStructure::CalculateAmmoConsumption(const TSet<TSharedPtr<FShipStructureDevice>>& device_set)
{
	// Sum production and consumption in this subnetwork
	float total_production = 0.0f;
	float total_consumption = 0.0f;

	for (const TSharedPtr<FShipStructureDevice>& device : device_set)
	{
		if (device->Stats.AmmoConsumption > 0.0f)
		{
			total_consumption -= device->Stats.AmmoConsumption;
		}
		if (device->Stats.AmmoConsumption < 0.0f)
		{
			total_production -= device->Stats.AmmoConsumption;
		}
	}

	// Determine fractions
	float used_fraction = 0.0f;
	if (total_production > 0.0f)
	{
		used_fraction = FMath::Min(1.0f, total_production / -total_consumption);
	}

	float satisfaction = 1.0f;
	if (total_consumption < 0.0f)
	{
		satisfaction = FMath::Min(1.0f, -total_consumption / total_production);
	}

	// Apply to devices
	for (const TSharedPtr<FShipStructureDevice>& device : device_set)
	{
		// Producers: set how much of their production is actually used (absolute amount)
		if (device->Stats.AmmoConsumption > 0.0f)
		{
			// Absolute amount of ammo that this device supplies to consumers
			device->Usage = used_fraction;
		}

		// Consumers: set fraction of consumption satisfied
		if (device->Stats.AmmoConsumption < 0.0f)
		{
			device->Usage = satisfaction;
		}
	}
}
