// Copyright (c) 2025, sillygilly. All rights reserved.

#include "ShipData/ShipDeviceAsset.h"
#include "ShipData/ShipPartAsset.h"
#include "Shipyard/ShipDeviceSector.h"
#include "Shipyard/ShipPartInstance.h"
#include "Shipyard/ShipStructure.h"
#include "Tests/TestHarnessAdapter.h"

namespace
{

constexpr float kUsageTol = 0.001f;

FDeviceStats MakeStats(EDeviceType device_type, float fuel = 0.0f, float ammo = 0.0f, float sector_width = 0.0f)
{
	FDeviceStats stats;
	stats.DeviceType = device_type;
	stats.FuelConsumption = fuel;
	stats.AmmoConsumption = ammo;
	stats.SectorWidth = sector_width;
	return stats;
}

FShipCellData MakeCell(const FIntVector2& position, ECellType type)
{
	FShipCellData cell;
	cell.Position = position;
	cell.CellType = type;
	return cell;
}

UShipPartInstance* MakePart(const FDeviceStats& stats,
    const FShipPartTransform& transform,
    std::initializer_list<FShipCellData> cells,
    int32 height = 0,
    int32 device_height = 0,
    TSet<FIntVector2> device_cells = {})
{
	UShipDeviceAsset* device_asset = NewObject<UShipDeviceAsset>();
	device_asset->Stats = stats;

	UShipPartAsset* part_asset = NewObject<UShipPartAsset>();
	part_asset->Device = device_asset;
	part_asset->Height = height;
	part_asset->DeviceHeight = device_height;
	part_asset->DeviceCells = device_cells;
	part_asset->Cells.Reserve(static_cast<int32>(cells.size()));
	for (const FShipCellData& cell : cells)
	{
		part_asset->Cells.Add(cell);
	}

	UShipPartInstance* part_instance = NewObject<UShipPartInstance>();
	part_instance->PartAsset = part_asset;
	part_instance->Transform = transform;
	return part_instance;
}

UShipPartInstance* MakeBridge(const FIntVector2& position, std::initializer_list<FShipCellData> cells, int32 height = 0)
{
	return MakePart(MakeStats(EDeviceType::BRIDGE), FShipPartTransform(position, 0, false), cells, height);
}

UShipPartInstance* MakeEngine(const FIntVector2& position, std::initializer_list<FShipCellData> cells, int32 height = 0, int32 device_height = 0, float fuel = 0.0f, float ammo = 0.0f, float sector_width = 0.0f, TSet<FIntVector2> device_cells = {})
{
	return MakePart(MakeStats(EDeviceType::ENGINE, fuel, ammo, sector_width), FShipPartTransform(position, 0, false), cells, height, device_height, device_cells);
}

FShipStructure MakeStructure(std::initializer_list<UShipPartInstance*> part_instances)
{
	TArray<TObjectPtr<UShipPartInstance>> part_instance_array;
	part_instance_array.Reserve(static_cast<int32>(part_instances.size()));
	for (UShipPartInstance* part_instance : part_instances)
	{
		part_instance_array.Add(part_instance);
	}
	return FShipStructure(FShipPartTransform(), part_instance_array, nullptr);
}

TSharedPtr<FShipStructureDevice> GetDeviceAt(const FShipStructure& structure, const FIntVector3& pos)
{
	if (TSharedPtr<FShipStructureCell> cell = structure.Cells.FindRef(pos))
	{
		return cell->Device;
	}
	return nullptr;
}

UShipPartInstance* MakeSingleCabinPartInstance(int32 height,
    EDeviceType device_type = EDeviceType::QUARTERS,
    int32 device_height = 0)
{
	return MakePart(MakeStats(device_type), FShipPartTransform(), {MakeCell({0, 0}, ECellType::CABIN)}, height, device_height);
}

FShipStructure MakeSingleCabinStructure(int32 height,
    EDeviceType device_type = EDeviceType::QUARTERS,
    int32 device_height = 0)
{
	return MakeStructure({MakeSingleCabinPartInstance(height, device_type, device_height)});
}

}    // namespace

TEST_CASE_NAMED(FShipStructureProcessTest, "ShipEditor::ShipStructure::Process", "[ShipEditor][ShipStructure]")
{
	SECTION("Decks connected to root become DECK_PHONE_LINE")
	{
		FShipStructure structure = MakeStructure({
		    MakeBridge({0, 0}, {
		                           MakeCell({1, 1}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                           MakeCell({1, 2}, ECellType::DECK),
		                       }),
		});

		structure.Process();

		TSharedPtr<FShipStructureCell> deck_cell = structure.Cells.FindRef(FIntVector3(1, 2, 0));
		CHECK(deck_cell);
		CHECK(deck_cell->CellType == ECellType::DECK_PHONE_LINE);
	}

	SECTION("Connected deck phone lines continue through generated height layers")
	{
		FShipStructure structure = MakeStructure({
		    MakeBridge({0, 0}, {
		                           MakeCell({1, 1}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                           MakeCell({1, 2}, ECellType::DECK),
		                       },
		        2),
		});

		structure.Process();

		CHECK(structure.GetCellType(FIntVector3(1, 2, -4)) == ECellType::DECK_PHONE_LINE);
		CHECK(structure.GetCellType(FIntVector3(1, 2, -2)) == ECellType::DECK_PHONE_LINE);
		CHECK(structure.GetCellType(FIntVector3(1, 2, 0)) == ECellType::DECK_PHONE_LINE);
		CHECK(structure.GetCellType(FIntVector3(1, 2, 2)) == ECellType::DECK_PHONE_LINE);
		CHECK(structure.GetCellType(FIntVector3(1, 2, 4)) == ECellType::DECK_PHONE_LINE);
	}

	SECTION("Decks not connected to root stay DECK")
	{
		FShipStructure structure = MakeStructure({
		    MakeBridge({0, 0}, {
		                           MakeCell({1, 1}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                           MakeCell({5, 4}, ECellType::DECK),
		                       }),
		});

		structure.Process();

		TSharedPtr<FShipStructureCell> deck_cell = structure.Cells.FindRef(FIntVector3(5, 4, 0));
		CHECK(deck_cell);
		CHECK(deck_cell->CellType == ECellType::DECK);
	}

	SECTION("Intersection root on phone line can phone the bridge")
	{
		FShipStructure structure = MakeStructure({
		    MakeBridge({0, 0}, {
		                           MakeCell({1, 1}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                       }),
		    MakeEngine({2, 0}, {
		                           MakeCell({0, 1}, ECellType::DECK),
		                           MakeCell({1, 1}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                       }),
		});
		TSharedPtr<FShipStructureDevice> phone_device = GetDeviceAt(structure, FIntVector3(3, 1, 0));

		structure.Process();

		CHECK(phone_device->CanPhoneTheBridge);
	}

	SECTION("Intersection root not on phone line cannot phone the bridge")
	{
		FShipStructure structure = MakeStructure({
		    MakeBridge({0, 0}, {
		                           MakeCell({1, 1}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                       }),
		    MakeEngine({4, 0}, {
		                           MakeCell({1, 1}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                       }),
		});
		TSharedPtr<FShipStructureDevice> phone_device = GetDeviceAt(structure, FIntVector3(5, 1, 0));

		structure.Process();

		CHECK(!phone_device->CanPhoneTheBridge);
	}

	SECTION("Traversable cabins connected to root can reach the bridge")
	{
		FShipStructure structure = MakeStructure({
		    MakeBridge({0, 0}, {
		                           MakeCell({1, 1}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                           MakeCell({0, 0}, ECellType::CABIN),
		                       }),
		    MakeEngine({2, 0}, {
		                           MakeCell({0, 0}, ECellType::CABIN),
		                       }),
		});
		TSharedPtr<FShipStructureDevice> cabin_device = GetDeviceAt(structure, FIntVector3(2, 0, 0));

		structure.Process();

		CHECK(cabin_device->CanReachTheBridge);
	}

	SECTION("CABIN_BLOCKED stops interior reach beyond it")
	{
		FShipStructure structure = MakeStructure({
		    MakeBridge({0, 0}, {
		                           MakeCell({1, 1}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                           MakeCell({0, 0}, ECellType::CABIN),
		                       }),
		    MakeEngine({2, 0}, {
		                           MakeCell({0, 0}, ECellType::CABIN_BLOCKED),
		                       }),
		    MakeEngine({4, 0}, {
		                           MakeCell({0, 0}, ECellType::CABIN),
		                       }),
		});
		TSharedPtr<FShipStructureDevice> front_device = GetDeviceAt(structure, FIntVector3(0, 0, 0));
		TSharedPtr<FShipStructureDevice> far_device = GetDeviceAt(structure, FIntVector3(4, 0, 0));

		structure.Process();

		CHECK(front_device->CanReachTheBridge);
		CHECK(!far_device->CanReachTheBridge);
	}

	SECTION("Device 0 vs armor test")
	{
		FShipStructure structure = MakeStructure({
		    MakeBridge({0, 0}, {
		                           MakeCell({1, 1}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                           MakeCell({0, 0}, ECellType::CABIN_BLOCKED),
		                           MakeCell({0, 2}, ECellType::CABIN),
		                       }),
		    MakeEngine({2, 0}, {
		                           MakeCell({0, 0}, ECellType::CABIN_OUTSIDE),
		                       }),
		});

		structure.Process();

		CHECK(structure.GetCellType(FIntVector3(-1, 0, 0)) == ECellType::DECK_ARMOR);
		CHECK(structure.GetCellType(FIntVector3(1, 0, 0)) == ECellType::NONE);
		CHECK(structure.GetCellType(FIntVector3(3, 0, 0)) == ECellType::NONE);
		CHECK(structure.GetCellType(FIntVector3(0, 0, 1)) == ECellType::DECK_ARMOR);
		CHECK(structure.GetCellType(FIntVector3(2, 0, 1)) == ECellType::NONE);
	}

	SECTION("Device 1 vs armor test")
	{
		FShipStructure structure = MakeStructure({
		    MakeBridge({0, 0}, {
		                           MakeCell({1, 1}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                           MakeCell({0, 0}, ECellType::CABIN),
		                       }),
		    MakeEngine({2, 0}, {
		                           MakeCell({0, 0}, ECellType::CABIN),
		                           MakeCell({2, 0}, ECellType::CABIN_BLOCKED),
		                       },
		        0, 1, 0.0f, 0.0f, 0.0f, {FIntVector2(2, 0)}),
		});

		structure.Process();

		CHECK(structure.GetCellType(FIntVector3(-1, 0, 0)) == ECellType::DECK_ARMOR);
		CHECK(structure.GetCellType(FIntVector3(2, 0, 1)) == ECellType::DECK_ARMOR);
		CHECK(structure.GetCellType(FIntVector3(3, 0, 0)) == ECellType::NONE);
		CHECK(structure.GetCellType(FIntVector3(5, 0, 0)) == ECellType::DECK_ARMOR);
		CHECK(structure.GetCellType(FIntVector3(4, 0, 1)) == ECellType::NONE);
		CHECK(structure.GetCellType(FIntVector3(2, 0, 2)) == ECellType::NONE);
		CHECK(structure.GetCellType(FIntVector3(4, 0, 2)) == ECellType::CABIN_OUTSIDE);
		CHECK(structure.GetCellType(FIntVector3(4, 0, 3)) == ECellType::NONE);
		CHECK(structure.GetCellType(FIntVector3(3, 0, 2)) == ECellType::NONE);
		CHECK(structure.GetCellType(FIntVector3(5, 0, 2)) == ECellType::NONE);
	}

	SECTION("Wall and floor armor is only placed around ship parts")
	{
		FShipStructure structure = MakeStructure({
		    MakeBridge({0, 0}, {
		                           MakeCell({1, 1}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                           MakeCell({0, 0}, ECellType::CABIN),
		                           MakeCell({2, 0}, ECellType::CABIN),
		                       }),
		    MakeEngine({4, 0}, {
		                           MakeCell({0, 0}, ECellType::CABIN),
		                           MakeCell({1, 1}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                       }),
		});
		TSharedPtr<FShipStructureDevice> disconnected_device = GetDeviceAt(structure, FIntVector3(4, 0, 0));

		structure.Process();

		CHECK(structure.GetCellType(FIntVector3(-1, 0, 0)) == ECellType::DECK_ARMOR);
		CHECK(structure.GetCellType(FIntVector3(0, 1, 0)) == ECellType::DECK_ARMOR);
		CHECK(structure.GetCellType(FIntVector3(0, 0, 1)) == ECellType::DECK_ARMOR);
		CHECK(structure.GetCellType(FIntVector3(4, 0, -1)) == ECellType::DECK_ARMOR);
		CHECK(structure.GetCellType(FIntVector3(4, 0, 1)) == ECellType::DECK_ARMOR);
		CHECK(structure.GetCellType(FIntVector3(3, 0, 0)) == ECellType::DECK_ARMOR);
		CHECK(structure.GetCellType(FIntVector3(4, -1, 0)) == ECellType::NONE);
		CHECK(!disconnected_device->CanReachTheBridge);
		CHECK(!structure.Cells.Contains(FIntVector3(1, 0, 0)));
	}

	SECTION("Armor is not placed between height layers of a bridge cabin")
	{
		FShipStructure structure = MakeSingleCabinStructure(1, EDeviceType::BRIDGE);

		structure.Process();

		CHECK(structure.GetCellType(FIntVector3(0, 0, -2)) == ECellType::CABIN);
		CHECK(structure.GetCellType(FIntVector3(0, 0, 0)) == ECellType::CABIN);
		CHECK(structure.GetCellType(FIntVector3(0, 0, 2)) == ECellType::CABIN);
		CHECK(structure.GetCellType(FIntVector3(0, 0, -1)) == ECellType::NONE);
		CHECK(structure.GetCellType(FIntVector3(0, 0, 1)) == ECellType::NONE);
		CHECK(structure.GetCellType(FIntVector3(0, 0, -3)) == ECellType::DECK_ARMOR);
		CHECK(structure.GetCellType(FIntVector3(0, 0, 3)) == ECellType::DECK_ARMOR);
	}

	SECTION("Device sector is partially blocked by one cabin cell")
	{
		FShipStructure structure = MakeStructure({
		    MakeBridge({10, 10}, {
		                             MakeCell({0, 0}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                         }),
		    MakeEngine({0, 0}, {
		                           MakeCell({0, 0}, ECellType::CABIN_BLOCKED),
		                           MakeCell({2, 0}, ECellType::CABIN),
		                       },
		        0, 0, 0.0f, 0.0f, 180.0f),
		});
		TSharedPtr<FShipStructureDevice> gun_device = GetDeviceAt(structure, FIntVector3(0, 0, 0));

		structure.Process();

		CHECK(gun_device->AvailableSector.IsValid());
		CHECK(IsSectorAngleNear(gun_device->AvailableSector.Rotation, 67.5f));
		CHECK(IsSectorAngleNear(gun_device->AvailableSector.Width, 45.0f));
	}

	SECTION("Split device sector keeps the side closest to the device rotation")
	{
		FShipStructure structure = MakeStructure({
		    MakeBridge({10, 10}, {
		                            MakeCell({0, 0}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                        }),
		    MakePart(MakeStats(EDeviceType::GUN, 0.0f, 0.0f, 270.0f), FShipPartTransform({0, 0}, 1, false), {
		                                                                                                      MakeCell({0, 0}, ECellType::CABIN_BLOCKED),
		                                                                                                      MakeCell({2, 0}, ECellType::CABIN),
		                                                                                                  }),
		});
		TSharedPtr<FShipStructureDevice> gun_device = GetDeviceAt(structure, FIntVector3(0, 0, 0));

		structure.Process();

		CHECK(gun_device->AvailableSector.IsValid());
		CHECK(IsSectorAngleNear(gun_device->AvailableSector.Rotation, 180.0f));
		CHECK(IsSectorAngleNear(gun_device->AvailableSector.Width, 90.0f));
	}

	SECTION("Disconnected corridor roots compute fuel usage separately")
	{
		FShipStructure structure = MakeStructure({
		    MakeBridge({1, 1}, {
		                           MakeCell({0, 0}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                       }),
		    MakeEngine({0, 0}, {
		                           MakeCell({0, 0}, ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT),
		                       },
		        0, 0, -10.0f),
		    MakeEngine({0, 2}, {
		                           MakeCell({0, 0}, ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT),
		                       },
		        0, 0, 5.0f),
		    MakeEngine({0, 4}, {
		                           MakeCell({0, 0}, ECellType::CABIN_TECHNICAL_CORRIDOR),
		                       },
		        0, 0, 100.0f),
		    MakeEngine({10, 0}, {
		                            MakeCell({0, 0}, ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT),
		                        },
		        0, 0, -4.0f),
		    MakeEngine({12, 0}, {
		                            MakeCell({0, 0}, ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT),
		                        },
		        0, 0, 8.0f),
		});
		TSharedPtr<FShipStructureDevice> producer_a = GetDeviceAt(structure, FIntVector3(0, 0, 0));
		TSharedPtr<FShipStructureDevice> consumer_a = GetDeviceAt(structure, FIntVector3(0, 2, 0));
		TSharedPtr<FShipStructureDevice> ignored_a = GetDeviceAt(structure, FIntVector3(0, 4, 0));
		TSharedPtr<FShipStructureDevice> producer_b = GetDeviceAt(structure, FIntVector3(10, 0, 0));
		TSharedPtr<FShipStructureDevice> consumer_b = GetDeviceAt(structure, FIntVector3(12, 0, 0));

		structure.Process();

		CHECK(FMath::IsNearlyEqual(consumer_a->Usage, 1.0f, kUsageTol));
		CHECK(FMath::IsNearlyEqual(producer_a->Usage, 0.5f, kUsageTol));
		CHECK(FMath::IsNearlyEqual(ignored_a->Usage, 0.0f, kUsageTol));
		CHECK(FMath::IsNearlyEqual(consumer_b->Usage, 0.5f, kUsageTol));
		CHECK(FMath::IsNearlyEqual(producer_b->Usage, 1.0f, kUsageTol));
	}

	SECTION("Connected corridors with mixed IsPartOfTheShip split subnetworks")
	{
		FShipStructure structure = MakeStructure({
		    MakeBridge({1, 1}, {
		                           MakeCell({0, 0}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                       }),
		    MakeEngine({0, 0}, {
		                           MakeCell({0, 0}, ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT),
		                           MakeCell({2, 0}, ECellType::CABIN_TECHNICAL_CORRIDOR),
		                       },
		        0, 0, -10.0f),
		    MakeEngine({0, 2}, {
		                           MakeCell({0, 0}, ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT),
		                       },
		        0, 0, 5.0f),
		    MakeEngine({4, 0}, {
		                           MakeCell({0, 0}, ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT),
		                           MakeCell({100, 100}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                       },
		        0, 0, -4.0f),
		    MakeEngine({4, 2}, {
		                           MakeCell({0, 0}, ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT),
		                           MakeCell({100, 100}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
		                       },
		        0, 0, 8.0f),
		});
		TSharedPtr<FShipStructureDevice> producer_a = GetDeviceAt(structure, FIntVector3(0, 0, 0));
		TSharedPtr<FShipStructureDevice> consumer_a = GetDeviceAt(structure, FIntVector3(0, 2, 0));
		TSharedPtr<FShipStructureDevice> producer_b = GetDeviceAt(structure, FIntVector3(4, 0, 0));
		TSharedPtr<FShipStructureDevice> consumer_b = GetDeviceAt(structure, FIntVector3(4, 2, 0));

		structure.Process();

		CHECK(FMath::IsNearlyEqual(consumer_a->Usage, 1.0f, kUsageTol));
		CHECK(FMath::IsNearlyEqual(producer_a->Usage, 0.5f, kUsageTol));
		CHECK(FMath::IsNearlyEqual(consumer_b->Usage, 0.5f, kUsageTol));
		CHECK(FMath::IsNearlyEqual(producer_b->Usage, 1.0f, kUsageTol));
	}

	SECTION("Fuel usage fractions reflect production vs consumption balance")
	{
		{
			FShipStructure structure = MakeStructure({
			    MakeBridge({1, 1}, {
			                           MakeCell({0, 0}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
			                       }),
			    MakeEngine({0, 0}, {
			                           MakeCell({0, 0}, ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT),
			                       },
			        0, 0, -10.0f),
			    MakeEngine({0, 2}, {
			                           MakeCell({0, 0}, ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT),
			                       },
			        0, 0, 5.0f),
			});
			TSharedPtr<FShipStructureDevice> producer = GetDeviceAt(structure, FIntVector3(0, 0, 0));
			TSharedPtr<FShipStructureDevice> consumer = GetDeviceAt(structure, FIntVector3(0, 2, 0));

			structure.Process();

			CHECK(FMath::IsNearlyEqual(consumer->Usage, 1.0f, kUsageTol));
			CHECK(FMath::IsNearlyEqual(producer->Usage, 0.5f, kUsageTol));
		}

		{
			FShipStructure structure = MakeStructure({
			    MakeBridge({1, 1}, {
			                           MakeCell({0, 0}, ECellType::INTERSECTION_PHONE_LINE_ROOT),
			                       }),
			    MakeEngine({0, 0}, {
			                           MakeCell({0, 0}, ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT),
			                       },
			        0, 0, -5.0f),
			    MakeEngine({0, 2}, {
			                           MakeCell({0, 0}, ECellType::CABIN_TECHNICAL_CORRIDOR_ROOT),
			                       },
			        0, 0, 10.0f),
			});
			TSharedPtr<FShipStructureDevice> producer = GetDeviceAt(structure, FIntVector3(0, 0, 0));
			TSharedPtr<FShipStructureDevice> consumer = GetDeviceAt(structure, FIntVector3(0, 2, 0));

			structure.Process();

			CHECK(FMath::IsNearlyEqual(consumer->Usage, 0.5f, kUsageTol));
			CHECK(FMath::IsNearlyEqual(producer->Usage, 1.0f, kUsageTol));
		}
	}
}

TEST_CASE_NAMED(FShipStructureLifecycleTest, "ShipEditor::ShipStructure::Lifecycle", "[ShipEditor][ShipStructure]")
{
	SECTION("Height 0 creates only base layer")
	{
		FShipStructure structure = MakeSingleCabinStructure(0);

		CHECK(structure.Cells.Num() == 1);

		TSharedPtr<FShipStructureCell> base_cell = structure.Cells.FindRef(FIntVector3(0, 0, 0));
		CHECK(base_cell);
		if (base_cell)
		{
			CHECK(base_cell->CellType == ECellType::CABIN);
		}
		CHECK(!structure.Cells.Contains(FIntVector3(0, 0, -1)));
		CHECK(!structure.Cells.Contains(FIntVector3(0, 0, 1)));
	}

	SECTION("Height 1 creates base, upper, and lower layers")
	{
		FShipStructure structure = MakeSingleCabinStructure(1);

		CHECK(structure.Cells.Num() == 3);

		TSharedPtr<FShipStructureDevice> shared_device;
		for (int32 z = -2; z <= 2; z += 2)
		{
			TSharedPtr<FShipStructureCell> cell = structure.Cells.FindRef(FIntVector3(0, 0, z));
			CHECK(cell);
			if (cell)
			{
				CHECK(cell->CellType == ECellType::CABIN);
				CHECK(cell->Device);
				if (shared_device)
				{
					CHECK(cell->Device == shared_device);
				}
				else
				{
					shared_device = cell->Device;
				}
			}
		}
	}

	SECTION("Destructor releases generated cells")
	{
		TArray<TWeakPtr<FShipStructureCell>> weak_cells;
		{
			FShipStructure structure = MakeSingleCabinStructure(1);
			for (int32 z = -2; z <= 2; z += 2)
			{
				TSharedPtr<FShipStructureCell> cell = structure.Cells.FindRef(FIntVector3(0, 0, z));
				CHECK(cell);
				if (cell)
				{
					weak_cells.Add(cell);
				}
			}
		}

		CHECK(weak_cells.Num() == 3);
		for (const TWeakPtr<FShipStructureCell>& weak_cell : weak_cells)
		{
			CHECK(!weak_cell.IsValid());
		}
	}
}
