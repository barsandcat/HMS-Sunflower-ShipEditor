// Copyright (c) 2024, sillygilly. All rights reserved.

#include "EditState/PaintEditState.h"

#include "EditState/SelectEditState.h"

UPaintEditState::UPaintEditState(const FObjectInitializer& ObjectInitializer)
{
	UE_LOG(LogTemp, Warning, TEXT("UPaintEditState"));
}

UPaintEditState::~UPaintEditState()
{
	UE_LOG(LogTemp, Warning, TEXT("~UPaintEditState"));
}

TScriptInterface<IEditStateInterface> UPaintEditState::SelectPart_Implementation(int partId)
{
	return this;
}

TScriptInterface<IEditStateInterface> UPaintEditState::Cancel_Implementation()
{
	return NewObject<USelectEditState>();
}