// Copyright (c) 2024, sillygilly. All rights reserved.

#include "ShipyardSubsystem.h"

#include "MVVMGameSubsystem.h"

const double GRID_SIZE = 100.0f;

namespace
{

void AddPart(TUVMShipPartArray& List, FString Name, int32 Id)
{
	TObjectPtr<UVMShipPart> Part = NewObject<UVMShipPart>();
	Part->SetName(Name);
	Part->SetPartId(Id);
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

FIntVector2 WorldToCellId(const FVector& WordlPos)
{
	return {int32(round(WordlPos.X / GRID_SIZE)), int32(round(WordlPos.Z / GRID_SIZE))};
}

FVector CellIdToWorld(const FIntVector2& CellId)
{
	return {CellId.X * GRID_SIZE, 0.0f, CellId.Y * GRID_SIZE};
}

}    // namespace

void UShipyardSubsystem::SetCursorPosition(FVector WorldPosition)
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

	FVector CursorPos = CellIdToWorld(WorldToCellId(WorldPosition));
	CursorPos.Y = WorldPosition.Y;

	if (!Cursor)
	{
		Cursor = World->SpawnActor<AActor>(CursorClassPtr, CursorPos, FRotator::ZeroRotator, {});
	}
	else
	{
		Cursor->SetActorLocation(CursorPos);
	}
}

void UShipyardSubsystem::AddModelViewToGlobal(UMVVMViewModelBase* ViewModel, UClass* Class, const FName& Name)
{
	const UWorld* World = GetWorld();
	if (!World)
		return;
	const UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
		return;
	UMVVMViewModelCollectionObject* Collection = GameInstance->GetSubsystem<UMVVMGameSubsystem>()->GetViewModelCollection();
	if (!Collection)
		return;

	FMVVMViewModelContext Context;
	Context.ContextClass = Class;
	Context.ContextName = Name;
	Collection->AddViewModelInstance(Context, ViewModel);
}

UShipyardSubsystem::UShipyardSubsystem()
    : UGameInstanceSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SelectionYellow(TEXT("/Game/Materials/M_Selection_Yellow.M_Selection_Yellow"));
	SelectionMaterial = SelectionYellow.Object;
}

void UShipyardSubsystem::Initialize(FSubsystemCollectionBase& SubsytemCollection)
{
	Super::Initialize(SubsytemCollection);
	SubsytemCollection.InitializeDependency(UMVVMGameSubsystem::StaticClass());

	VMPartBrowser = NewObject<UVMPartBrowser>();
	VMPartBrowser->AddFieldValueChangedDelegate(UVMPartBrowser::FFieldNotificationClassDescriptor::PartId,
	    INotifyFieldValueChanged::FFieldValueChangedDelegate::CreateUObject(this, &UShipyardSubsystem::OnBrushIdChanged));
	TUVMShipPartArray List;
	AddPart(List, "BL 4-inch Mk IX", 1);
	AddPart(List, "Vickers .50 cal", 2);
	AddPart(List, "Lewis .303 cal", 3);
	VMPartBrowser->SetPartList(List);

	VMBrush = NewObject<UVMBrush>();

	AddModelViewToGlobal(VMPartBrowser, UVMPartBrowser::StaticClass(), "VMPartBrowser");
	AddModelViewToGlobal(VMBrush, UVMBrush::StaticClass(), "VMBrush");

	CursorClassPtr = CursorClass.LoadSynchronous();

	PartClassPtr = StaticLoadClass(AShipPlanCell::StaticClass(), nullptr, TEXT("/Game/BP_ShipPart.BP_ShipPart_C"), nullptr, LOAD_None, nullptr);
}

void UShipyardSubsystem::OnBrushIdChanged(UObject* ViewModel, UE::FieldNotification::FFieldId FieldId)
{
	UE_LOG(LogTemp, Warning, TEXT("OnBrushIdChanged %s %s %d"), *ViewModel->GetName(), *FieldId.GetName().ToString(), VMPartBrowser->GetPartId());
	VMBrush->SetReady(VMPartBrowser->GetPartId() != 0);
	if (BrushId == 0 && VMPartBrowser->GetPartId() != 0)
	{
		OnBrushReady.Broadcast();
	}
	if (BrushId != 0 && VMPartBrowser->GetPartId() == 0)
	{
		OnBrushCleared.Broadcast();
	}
	BrushId = VMPartBrowser->GetPartId();
}

void UShipyardSubsystem::DoBrush()
{
	UE_LOG(LogTemp, Warning, TEXT("DoBrush %d"), BrushId);
	UWorld* World = GetWorld();
	if (!World || !Cursor || !BrushId || !PartClassPtr)
	{
		return;
	}

	FVector CursorPos = Cursor->GetActorLocation();
	FIntVector2 CellId = WorldToCellId(CursorPos);
	if (ShipPlan.Contains(CellId))
	{
		return;
	}
	TObjectPtr<AShipPlanCell> ShipPlanCell = World->SpawnActor<AShipPlanCell>(PartClassPtr, CursorPos, {}, {});
	if (!ShipPlanCell)
	{
		return;
	}
	ShipPlanCell->PartId = BrushId;

	ShipPlan.Add(CellId, ShipPlanCell);
	VMPartBrowser->SetPartId(0);
}

void UShipyardSubsystem::Select()
{
	UE_LOG(LogTemp, Warning, TEXT("Select %d"), BrushId);
	if (!Cursor || BrushId)
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
		TObjectPtr<AShipPlanCell> ShipPlanCell = *ShipPlan.Find(CellId);
		ShipPlan.Remove(CellId);
		VMPartBrowser->SetPartId(ShipPlanCell->PartId);
		ShipPlanCell->Destroy();
	}
}
