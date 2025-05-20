// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonActivatableWidget.h"
#include "CommonMessagingSubsystem.h"

#include "CommonGameFileDialog.generated.h"

UCLASS(Abstract)
class COMMONGAME_API UCommonGameFileDialog : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UCommonGameFileDialog();

	virtual void SetupDialog(bool SaveDialog, FText Header, FString File, FFileDialogResultDelegate ResultCallback);
};