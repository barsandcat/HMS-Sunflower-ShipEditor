// Copyright (c) 2025, sillygilly. All rights reserved.

#include "Shipyard/ShipStructure.h"

#include "Shipyard/ShipPartInstance.h"
#include "Shipyard/ShipPlanRender.h"

namespace
{

bool IsDeckIntersection(const FIntVector2& pos)
{
	return pos.X % 2 != 0 && pos.Y % 2 != 0;
}

bool IsDeckVertical(const FIntVector2& pos)
{
	return pos.X % 2 != 0 && pos.Y % 2 == 0;
}

bool IsDeckHorizontal(const FIntVector2& pos)
{
	return pos.X % 2 == 0 && pos.Y % 2 != 0;
}

bool IsCabin(const FIntVector2& pos)
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
		TSharedPtr<FShipStructureDevice> device = MakeShared<FShipStructureDevice>(part_instance->PartAsset->Device->Stats, render_transform(part_instance->Transform.Position));
		Devices[i] = device;
		for (FShipCellData& cell : part_instance->PartAsset->Cells)
		{
			if (IsIntersectionRootCell(cell.CellType))
			{
				device->RequiresPhoneConnection = true;
				if (device->Stats.DeviceType == EDeviceType::BRIDGE)
				{
					Root = render_transform(part_instance->Transform(cell.Position));
				}
			}
			Cells.Add(render_transform(part_instance->Transform(cell.Position)), MakeShared<FShipStructureCell>(cell.CellType, device, update));
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
	// Iterate BFS from root along empty cells (C). If cell has no adjacent cell - add armor in that spot

	ConnectDecks();

	// Mark devices as part of the ship.
	AddArmor();

	ConnectFuel();
}

void FShipStructure::AddArmor()
{
	TArray<FIntVector2> empty_queue;
	empty_queue.Reserve(Cells.Num());
	int32 empty_queue_head = 0;

	// Enqueue empty neighbors of root
	for (const FIntVector2& d : diagonal_dirs)
	{
		FIntVector2 neighbor_cabin_pos = *Root + d;
		if (TSharedPtr<FShipStructureCell> neighbor_cell = Cells.FindRef(neighbor_cabin_pos))
		{
			if (IsCabinTraversableCell(neighbor_cell->CellType))
			{
				neighbor_cell->Visited = 2;
				neighbor_cell->Device->CanReachTheBridge = true;
				empty_queue.Add(neighbor_cabin_pos);
			}
		}
	}

	while (empty_queue_head < empty_queue.Num())
	{
		FIntVector2 cabin_pos = empty_queue[empty_queue_head++];
		TSharedPtr<FShipStructureCell> cell = Cells.FindRef(cabin_pos);
		if (!cell)
		{
			continue;
		}

		// For each neighbor of this empty cell, if there is no cell present in structure => add armor there
		for (const FIntVector2& d : dirs)
		{
			FIntVector2 neighbor_cabin_pos = cabin_pos + d * 2;
			FIntVector2 neighbor_deck_pos = cabin_pos + d;

			TSharedPtr<FShipStructureCell> neighbor_cell = Cells.FindRef(neighbor_cabin_pos);

			// Add armor between traversable cabin and exterior
			if (IsCabinTraversableCell(cell->CellType) && (!neighbor_cell || neighbor_cell->CellType == ECellType::NONE))
			{
				TSharedPtr<FShipStructureCell> armor_cell = MakeShared<FShipStructureCell>(ECellType::DECK_ARMOR, cell->Device, cell->Update);
				Cells.Add(neighbor_deck_pos, armor_cell);
			}

			// Add neighbor to BFS if it's traversable and not visited yet
			if (neighbor_cell && IsCabinTraversableCell(neighbor_cell->CellType) && (neighbor_cell->Device->CanPhoneTheBridge || !neighbor_cell->Device->RequiresPhoneConnection))
			{
				if (neighbor_cell->Visited != 2)
				{
					neighbor_cell->Visited = 2;
					neighbor_cell->Device->CanReachTheBridge = true;
					empty_queue.Add(neighbor_cabin_pos);
				}
			}
		}
	}
}

void FShipStructure::ConnectDecks()
{
	// BFS along decks to mark structural decks
	TArray<FIntVector2> deck_queue;
	deck_queue.Reserve(Cells.Num());
	int32 deck_queue_head = 0;

	TSet<FIntVector2> visited_intersections;

	// Enqueue root
	deck_queue.Add(*Root);
	visited_intersections.Add(*Root);

	// Process deck BFS
	while (deck_queue_head < deck_queue.Num())
	{
		FIntVector2 deck_pos = deck_queue[deck_queue_head++];
		TSharedPtr<FShipStructureCell> cell = Cells.FindRef(deck_pos);

		if (IsDeckIntersection(deck_pos))
		{
			if (cell && IsIntersectionRootCell(cell->CellType))
			{
				cell->Device->CanPhoneTheBridge = true;
			}

			for (const FIntVector2& d : dirs)
			{
				FIntVector2 neighbor_deck_pos = deck_pos + d;
				TSharedPtr<FShipStructureCell> neighbor_cell = Cells.FindRef(neighbor_deck_pos);
				if (neighbor_cell && neighbor_cell->Visited != 1 && IsDeckCell(neighbor_cell->CellType))
				{
					neighbor_cell->Visited = 1;
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
				for (const FIntVector2& d : vertical_dirs)
				{
					FIntVector2 neighbor_deck_pos = deck_pos + d;
					if (!visited_intersections.Contains(neighbor_deck_pos))
					{
						deck_queue.Add(neighbor_deck_pos);
					}
				}
			}
			if (IsDeckHorizontal(deck_pos))
			{
				for (const FIntVector2& d : horizontal_dirs)
				{
					FIntVector2 neighbor_deck_pos = deck_pos + d;
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
		const FIntVector2& cell_pos = i.Key;
		const TSharedPtr<FShipStructureCell> cell = i.Value;
		cell->Update->SetCellMesh(cell_pos, cell->CellType);
	}

	for (const auto& i : Devices)
	{
		const TSharedPtr<FShipStructureDevice> device = i;
		if (device->Stats.FuelConsumption != 0.0f || device->Stats.AmmoConsumption != 0.0f)
		{
			DevicesUpdate->SetDeviceStatus(device->Stats.DeviceName, device->Position, device->Usage);
		}
	}
}

void FShipStructure::ConnectFuel()
{
	// Gather unvisited technical corridor roots that belong to the ship (device is wall-connected)
	TSet<FIntVector2> unvisited_roots;
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
		FIntVector2 start_pos = unvisited_roots.Array()[0];
		unvisited_roots.Remove(start_pos);

		// BFS queue of corridor positions
		TArray<FIntVector2> queue;
		queue.Add(start_pos);
		int32 queue_head = 0;

		// Mark the start position as visited for fuel traversal (Visited == 3)
		if (TSharedPtr<FShipStructureCell> start_cell = Cells.FindRef(start_pos))
		{
			start_cell->Visited = 3;
			devices_in_net.Add(start_cell->Device);
		}

		// Traverse
		while (queue_head < queue.Num())
		{
			FIntVector2 pos = queue[queue_head++];
			TSharedPtr<FShipStructureCell> cell = Cells.FindRef(pos);
			if (!cell)
			{
				continue;
			}

			// Explore 4 cardinal neighbors for technical corridor connectivity
			for (const FIntVector2& d : dirs)
			{
				FIntVector2 neighbor_pos = pos + d * 2;
				if (TSharedPtr<FShipStructureCell> neighbor_cell = Cells.FindRef(neighbor_pos))
				{
					// Only traverse along technical corridor cells or corridor roots
					if (IsTechnicalCorridorCell(neighbor_cell->CellType) && cell->Device->IsPartOfTheShip() == neighbor_cell->Device->IsPartOfTheShip() && neighbor_cell->Visited != 3)
					{
						neighbor_cell->Visited = 3;
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

		// Store discovered subnetwork devices for later processing
		subnetworks.Add(MoveTemp(devices_in_net));
	}

	for (const TSet<TSharedPtr<FShipStructureDevice>>& device_set : subnetworks)
	{
		CalculateFuelConsumption(device_set);
		CalculateAmmoConsumption(device_set);
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
