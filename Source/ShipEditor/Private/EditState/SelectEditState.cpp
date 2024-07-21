// Copyright (c) 2024, sillygilly. All rights reserved.

#include "EditState/SelectEditState.h"

#include "EditState/PaintEditState.h"

USelectEditState::USelectEditState(const FObjectInitializer& ObjectInitializer)
{
	UE_LOG(LogTemp, Warning, TEXT("USelectEditState"));
}

USelectEditState::~USelectEditState()
{
	UE_LOG(LogTemp, Warning, TEXT("~USelectEditState"));
}

TScriptInterface<IEditStateInterface> USelectEditState::SelectPart_Implementation(int partId)
{
	return NewObject<UPaintEditState>();
}

TScriptInterface<IEditStateInterface> USelectEditState::Cancel_Implementation()
{
	return this;
}