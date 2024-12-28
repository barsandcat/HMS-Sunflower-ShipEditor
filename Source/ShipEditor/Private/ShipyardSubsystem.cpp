// Copyright (c) 2024, sillygilly. All rights reserved.

#include "ShipyardSubsystem.h"

#include "MVVMGameSubsystem.h"

void UShipyardSubsystem::Initialize(FSubsystemCollectionBase& SubsytemCollection)
{
	SubsytemCollection.InitializeDependency(UMVVMGameSubsystem::StaticClass());
	MVVMShiyard = NewObject<UMVVMShipyard>();
	if (const UWorld* World = GetWorld())
	{
		if (const UGameInstance* GameInstance = World->GetGameInstance())
		{
			UMVVMViewModelCollectionObject* Collection = GameInstance->GetSubsystem<UMVVMGameSubsystem>()->GetViewModelCollection();
			if (Collection)
			{
				FMVVMViewModelContext Context;
				Context.ContextClass = UMVVMShipyard::StaticClass();
				Context.ContextName = "Shipyard";
				Collection->AddViewModelInstance(Context, MVVMShiyard);
			}
		}
	}
}

ETickableTickType UShipyardSubsystem::GetTickableTickType() const
{
	return HasAnyFlags(RF_ClassDefaultObject) ? ETickableTickType::Never : ETickableTickType::Always;
}

void UShipyardSubsystem::Tick(float DeltaTime)
{
	MVVMShiyard->SetTest(DeltaTime);
}

TStatId UShipyardSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UShipyardSubsystem, STATGROUP_Tickables);
}
