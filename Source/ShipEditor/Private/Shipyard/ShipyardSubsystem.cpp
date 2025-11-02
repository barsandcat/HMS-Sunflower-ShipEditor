// Copyright (c) 2024, sillygilly. All rights reserved.

#include "ShipyardSubsystem.h"

#include "Engine/AssetManager.h"
#include "JsonObjectConverter.h"
#include "MVVMGameSubsystem.h"
#include "ShipData/ShipPartAsset.h"
#include "ShipData/ShipPlanData.h"
#include "ShipPlanRender.h"
#include "Shipyard/Filter/ElevationFilter.h"
#include "Shipyard/Filter/MountFilter.h"
#include "Shipyard/Filter/StructureFilter.h"

const double GRID_SIZE = 100.0f;

namespace
{

void AddPart(TUVMShipPartArray& List, const FText& name, FName id, int32 category_id, int32 elevation, bool dynamic, bool load_bearing)
{
	TObjectPtr<UVMShipPart> Part = NewObject<UVMShipPart>();
	Part->SetName(name);
	Part->SetPartId(id);
	Part->SetCategoryId(category_id);
	Part->SetElevation(elevation);
	Part->SetDynamicMount(dynamic);
	Part->SetLoadBearing(load_bearing);
	List.Add(Part);
}

void SetOverlayMaterial(AShipPlanCell* Cell, UMaterialInterface* Material)
{
	TInlineComponentArray<UStaticMeshComponent*> StaticMeshes;
	Cell->GetComponents(StaticMeshes);
	for (UStaticMeshComponent* StaticMesh : StaticMeshes)
	{
		StaticMesh->SetOverlayMaterial(Material);
	}
}

void SetMaterial(AShipPlanCell* Cell, UMaterialInterface* Material)
{
	TInlineComponentArray<UStaticMeshComponent*> StaticMeshes;
	Cell->GetComponents(StaticMeshes);
	for (UStaticMeshComponent* StaticMesh : StaticMeshes)
	{
		for (int Index = 0; Index < StaticMesh->GetNumMaterials(); Index++)
		{
			if (StaticMesh->GetMaterial(Index))
			{
				StaticMesh->SetMaterial(Index, Material);
			}
		}
	}
}

FIntVector2 WorldToCellId(const FVector& WorldPos)
{
	return {int32(round(WorldPos.X / GRID_SIZE)), int32(round(WorldPos.Z / GRID_SIZE))};
}

FVector CellIdToWorld(const FIntVector2& CellId)
{
	return {CellId.X * GRID_SIZE, 0.0f, CellId.Y * GRID_SIZE};
}

}    // namespace

void UShipyardSubsystem::AddCategory(TUVMShipPartCategoryArray& list, const FText& name, int32 id)
{
	TObjectPtr<UVMShipPartCategory> category = NewObject<UVMShipPartCategory>();

	category->AddFieldValueChangedDelegate(UVMShipPartCategory::FFieldNotificationClassDescriptor::Selected,
	    INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(this, &UShipyardSubsystem::OnCategorySelected));

	category->SetName(name);
	category->SetCategoryId(id);
	list.Add(category);
}

void UShipyardSubsystem::OnCategorySelected(UObject* ViewModel, UE::FieldNotification::FFieldId FieldId)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCategorySelected %s %s"), *ViewModel->GetName(), *FieldId.GetName().ToString());
	UpdatePartList();
}

void UShipyardSubsystem::OnFilterSelected(UObject* ViewModel, UE::FieldNotification::FFieldId FieldId)
{
	UE_LOG(LogTemp, Warning, TEXT("OnFilterSelected %s %s"), *ViewModel->GetName(), *FieldId.GetName().ToString());
	UpdatePartList();
}

void UShipyardSubsystem::SetCursorPosition(const TOptional<FVector>& WorldPosition)
{
	if (!CursorClassPtr)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (WorldPosition)
	{
		FVector CursorPos = CellIdToWorld(WorldToCellId(*WorldPosition));
		CursorPos.Y = WorldPosition->Y;

		if (!Cursor)
		{
			Cursor = World->SpawnActor<AActor>(CursorClassPtr, CursorPos, FRotator::ZeroRotator, {});
		}
		else
		{
			Cursor->SetActorLocation(CursorPos);
		}
	}
	else
	{
		if (Cursor)
		{
			Cursor->Destroy();
			Cursor = nullptr;
		}
	}
}

void UShipyardSubsystem::SetBrushPosition(const TOptional<FVector>& WorldPosition)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (WorldPosition && Brush)
	{
		FVector Pos = CellIdToWorld(WorldToCellId(*WorldPosition));
		Pos.Y = WorldPosition->Y;
		Brush->SetActorLocation(Pos);
	}
}

UShipyardSubsystem::UShipyardSubsystem()
    : UWorldSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SelectionYellow(TEXT("/Game/Materials/M_Selection_Yellow.M_Selection_Yellow"));
	SelectionMaterial = SelectionYellow.Object;
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> PreviewBlue(TEXT("/Game/Materials/M_Preview_Blue.M_Preview_Blue"));
	PreviewMaterial = PreviewBlue.Object;
}

void UShipyardSubsystem::Initialize(FSubsystemCollectionBase& SubsytemCollection)
{
	Super::Initialize(SubsytemCollection);

	LoadAllShipPartAssetsAsync();

	VMPartBrowser = NewObject<UVMPartBrowser>();
	/*
	AddPart(PartList, FText::FromString(TEXT("BL 4-inch Mk IX")), 1, 1, 1, false, true);
	AddPart(PartList, FText::FromString(TEXT("Vickers .50 cal")), 2, 1, 1, true, false);
	AddPart(PartList, FText::FromString(TEXT("Lewis .303 cal")), 3, 1, 2, true, false);
	AddPart(PartList, FText::FromString(TEXT("Whittle W.1")), 4, 2, 0, false, true);
	AddPart(PartList, FText::FromString(TEXT("Fuel tank 2t")), 5, 3, 1, false, true);
	AddPart(PartList, FText::FromString(TEXT("Petter 1260W")), 6, 4, 0, false, true);
	AddPart(PartList, FText::FromString(TEXT("4-inch magazine")), 7, 5, 0, false, true);
	AddPart(PartList, FText::FromString(TEXT("Quarters 400")), 8, 6, 0, false, true);
	*/
	VMPartBrowser->SetPartList(PartList);

	TUVMShipPartCategoryArray categories;
	AddCategory(categories, FText::FromString(TEXT("Guns")), 1);
	AddCategory(categories, FText::FromString(TEXT("Engines")), 2);
	AddCategory(categories, FText::FromString(TEXT("Fuel")), 3);
	AddCategory(categories, FText::FromString(TEXT("Power")), 4);
	AddCategory(categories, FText::FromString(TEXT("Magazines")), 5);
	AddCategory(categories, FText::FromString(TEXT("Quarters")), 6);
	VMPartBrowser->SetCategoryList(categories);

	FilterList.Add(MakeShared<MountFilter>(INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(this, &UShipyardSubsystem::OnFilterSelected)));
	FilterList.Add(MakeShared<StructureFilter>(INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(this, &UShipyardSubsystem::OnFilterSelected)));
	FilterList.Add(MakeShared<ElevationFilter>(INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(this, &UShipyardSubsystem::OnFilterSelected)));

	TUVMShipPartFilterArray filters;
	for (const auto& filter : FilterList)
	{
		filters.Add(filter->GetVM());
	}
	VMPartBrowser->SetFilterList(filters);
	VMPartBrowser->SelectPart.AddDynamic(this, &UShipyardSubsystem::SetBrushId);

	VMBrush = NewObject<UVMBrush>();

	VMShipPlan = NewObject<UVMShipPlan>();
	VMShipPlan->SetName(FString("Flower"));
	VMShipPlan->SaveShipPlan.AddDynamic(this, &UShipyardSubsystem::SaveShipPlan);
	VMShipPlan->LoadShipPlan.AddDynamic(this, &UShipyardSubsystem::LoadShipPlan);

	CursorClassPtr = CursorClass.LoadSynchronous();

	PartClassMap.Add(1, StaticLoadClass(AShipPlanCell::StaticClass(), nullptr, TEXT("/Game/BP_ShipPart_01.BP_ShipPart_01_C"), nullptr, LOAD_None, nullptr));
	PartClassMap.Add(2, StaticLoadClass(AShipPlanCell::StaticClass(), nullptr, TEXT("/Game/BP_ShipPart_02.BP_ShipPart_02_C"), nullptr, LOAD_None, nullptr));
	PartClassMap.Add(3, StaticLoadClass(AShipPlanCell::StaticClass(), nullptr, TEXT("/Game/BP_ShipPart_03.BP_ShipPart_03_C"), nullptr, LOAD_None, nullptr));
	PartClassMap.Add(4, StaticLoadClass(AShipPlanCell::StaticClass(), nullptr, TEXT("/Game/BP_ShipPart_04.BP_ShipPart_04_C"), nullptr, LOAD_None, nullptr));
	PartClassMap.Add(5, StaticLoadClass(AShipPlanCell::StaticClass(), nullptr, TEXT("/Game/BP_ShipPart_05.BP_ShipPart_05_C"), nullptr, LOAD_None, nullptr));

	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		FActorSpawnParameters spawn_params;
		spawn_params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ShipPlanRender = GetWorld()->SpawnActor<AShipPlanRender>(FVector(0, 0, 0), FRotator::ZeroRotator, spawn_params);
	}
}

void UShipyardSubsystem::LoadAllShipPartAssetsAsync()
{
	UAssetManager& asset_manager = UAssetManager::Get();

	ShipPartAssetIds.Reset();
	asset_manager.GetPrimaryAssetIdList(TEXT("ShipPartAsset"), ShipPartAssetIds);

	if (ShipPartAssetIds.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No ShipPart assets found."));
		return;
	}

	asset_manager.LoadPrimaryAssets(ShipPartAssetIds, {}, FStreamableDelegate::CreateUObject(this, &UShipyardSubsystem::OnShipPartAssetsLoaded));
}

void UShipyardSubsystem::OnShipPartAssetsLoaded()
{
	UAssetManager& asset_manager = UAssetManager::Get();

	UE_LOG(LogTemp, Log, TEXT("All ShipPart assets loaded: %d"), ShipPartAssetIds.Num());

	for (const FPrimaryAssetId& id : ShipPartAssetIds)
	{
		UShipPartAsset* ship_part_asset = asset_manager.GetPrimaryAssetObject<UShipPartAsset>(id);
		if (ship_part_asset)
		{
			UE_LOG(LogTemp, Log, TEXT("Loaded ShipPart asset: %s"), *ship_part_asset->PartName.ToString());
			PartAssetMap.Add(ship_part_asset->PartId, ship_part_asset);
			TObjectPtr<UVMShipPart> Part = NewObject<UVMShipPart>();
			Part->Initialize(
			    ship_part_asset->PartName,
			    ship_part_asset->PartId,
			    ship_part_asset->CategoryId,
			    ship_part_asset->Elevation,
			    ship_part_asset->DynamicMount,
			    ship_part_asset->LoadBearing);
			PartList.Add(Part);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load ShipPart asset: %s"), *id.ToString());
		}
	}
}

TSubclassOf<AShipPlanCell> UShipyardSubsystem::GetPartClass(int32 part_id) const
{
	if (auto part_class = PartClassMap.Find(part_id))
	{
		return *part_class;
	}
	else
	{
		return nullptr;
	}
}

void UShipyardSubsystem::DoBrush()
{
	UE_LOG(LogTemp, Warning, TEXT("DoBrush %s"), *BrushId.ToString());

	UWorld* World = GetWorld();
	UShipPartAsset* part_asset = PartAssetMap.FindRef(BrushId);
	if (!World || !Cursor || BrushId == NAME_None || !part_asset)
	{
	    return;
	}

	FIntVector2 CellId = WorldToCellId(Cursor->GetActorLocation());

	/*if (ShipPlan.Contains(CellId))
	{
	    return;
	}*/

	for (const auto& wall : part_asset->Walls)
	{
		if (wall.Value)
		{
			if (ShipPlanRender)
			{
				ShipPlanRender->AddWall(CellId + wall.Key);
			}
		}
	}

	for (const auto& floor : part_asset->Floors)
	{
		if (floor.Value)
		{
			if (ShipPlanRender)
			{
				ShipPlanRender->AddFloor(CellId + floor.Key);
			}
		}
	}

	// ShipPlanCell->PartId = BrushId;

	SetBrushId(NAME_None);
}

void UShipyardSubsystem::Select()
{
	UE_LOG(LogTemp, Warning, TEXT("Select %s"), *BrushId.ToString());
	if (!Cursor || BrushId != NAME_None)
	{
		return;
	}

	if (Selection)
	{
		SetOverlayMaterial(Selection, nullptr);
		Selection = nullptr;
	}

	FIntVector2 CellId = WorldToCellId(Cursor->GetActorLocation());
	if (ShipPlan.Contains(CellId))
	{
		Selection = *ShipPlan.Find(CellId);
		SetOverlayMaterial(Selection, SelectionMaterial);
	}
}

void UShipyardSubsystem::Delete()
{
	UE_LOG(LogTemp, Warning, TEXT("Delete"));
	if (!Selection)
	{
		return;
	}

	ShipPlan.Remove(WorldToCellId(Selection->GetActorLocation()));
	Selection->Destroy();
	Selection = nullptr;
}

void UShipyardSubsystem::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab"));
	if (!Cursor)
	{
		return;
	}

	if (Selection)
	{
		SetOverlayMaterial(Selection, nullptr);
		Selection = nullptr;
	}

	FIntVector2 CellId = WorldToCellId(Cursor->GetActorLocation());
	if (ShipPlan.Contains(CellId))
	{
		// TObjectPtr<AShipPlanCell> ShipPlanCell = *ShipPlan.Find(CellId);
		// ShipPlan.Remove(CellId);
		// SetBrushId(ShipPlanCell->PartId);
		// ShipPlanCell->Destroy();
	}
}

void UShipyardSubsystem::SetBrushId(FName brush_id)
{
	UE_LOG(LogTemp, Warning, TEXT("SetBrushId %s"), *brush_id.ToString());
	VMBrush->SetReady(brush_id != NAME_None);

	if (brush_id == BrushId)
	{
		return;
	}

	if (BrushId != NAME_None)
	{
		// if (ensure(Brush))
		//{
		//	Brush->Destroy();
		// }

		// Brush = nullptr;

		if (brush_id == NAME_None)
		{
			OnBrushCleared.Broadcast();
		}
	}

	if (brush_id != NAME_None)
	{
		// check(!Brush);

		// if (UWorld* World = GetWorld())
		//{
		//	TSubclassOf<AShipPlanCell> part_class = GetPartClass(brush_id);
		//	if (part_class)
		//	{
		//		Brush = World->SpawnActor<AShipPlanCell>(part_class, Cursor->GetActorLocation(), {}, {});
		//		SetMaterial(Brush, PreviewMaterial);
		//	}
		// }

		// if (Brush)
		//{
		//	Brush->PartId = brush_id;
		// }

		if (BrushId == NAME_None)
		{
			OnBrushReady.Broadcast();
		}
	}
	BrushId = brush_id;
}

void UShipyardSubsystem::SaveShipPlan(const FString& name)
{
	UE_LOG(LogTemp, Warning, TEXT("SaveShipPlan %s"), *name);
	FShipPlanData ship_plan_data;
	ship_plan_data.Name = "SuperFlower";
	FString file_content;

	if (!FJsonObjectConverter::UStructToJsonObjectString<FShipPlanData>(ship_plan_data, file_content, 0, 0))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed convert ship plan to %s"), *name);
		return;
	}

	if (!FFileHelper::SaveStringToFile(file_content, *name))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save ship plan to %s"), *name);
		return;
	}

	VMShipPlan->SetName(name);
}

void UShipyardSubsystem::LoadShipPlan(const FString& name)
{
	UE_LOG(LogTemp, Warning, TEXT("LoadShipPlan %s"), *name);
	FString file_content;
	if (!FFileHelper::LoadFileToString(file_content, *name))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load ship plan from %s"), *name);
		return;
	}

	FShipPlanData ship_plan_data;
	FText reason;
	if (!FJsonObjectConverter::JsonObjectStringToUStruct<FShipPlanData>(file_content, &ship_plan_data, 0, 0, false, &reason))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load ship plan from %s. Reason: %s"), *name, *reason.ToString());
	}

	VMShipPlan->SetName(ship_plan_data.Name);
}

std::set<int32> UShipyardSubsystem::GetSelectedCategories() const
{
	std::set<int32> result;
	for (const auto& category_vm : VMPartBrowser->GetCategoryList())
	{
		if (category_vm->GetSelected())
		{
			result.insert(category_vm->GetCategoryId());
		}
	}
	return result;
}

bool UShipyardSubsystem::IsAllowedByFiters(const TObjectPtr<UVMShipPart>& part_vm) const
{
	for (const auto& filter : FilterList)
	{
		if (!filter->IsAllowed(part_vm))
		{
			return false;
		}
	}
	return true;
}

void UShipyardSubsystem::UpdatePartList()
{
	TUVMShipPartArray list;
	std::set<int32> categories = GetSelectedCategories();
	for (const auto& part_vm : PartList)
	{
		if (categories.empty() || categories.contains(part_vm->GetCategoryId()))
		{
			if (IsAllowedByFiters(part_vm))
			{
				list.Add(part_vm);
			}
		}
	}
	for (const auto& category_vm : VMPartBrowser->GetCategoryList())
	{
		int32 count = 0;
		for (const auto& part_vm : list)
		{
			if (part_vm->GetCategoryId() == category_vm->GetCategoryId())
			{
				count++;
			}
		}
		category_vm->SetMatchingPartsCount(count);
	}

	for (const auto& filter : FilterList)
	{
		filter->UpdateMatchingPartsCount(list);
	}

	VMPartBrowser->SetPartList(list);
}
