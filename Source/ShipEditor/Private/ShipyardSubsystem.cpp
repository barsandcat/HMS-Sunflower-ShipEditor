// Copyright (c) 2024, sillygilly. All rights reserved.

#include "ShipyardSubsystem.h"

#include "MVVMGameSubsystem.h"

namespace
{

void AddPart(TUVMShipPartArray& List, FString Name, int32 Id)
{
	TObjectPtr<UVMShipPart> Part = NewObject<UVMShipPart>();
	Part->SetName(Name);
	Part->SetPartId(Id);
	List.Add(Part);
}

}    // namespace

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
	VMPartBrowser->SetPartId(0);
}
