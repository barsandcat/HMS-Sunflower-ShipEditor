// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Messaging/CommonMessagingSubsystem.h"

#include "MyCommonMessagingSubsystem.generated.h"

class FSubsystemCollectionBase;
class UCommonGameDialog;
class UCommonGameDialogDescriptor;
class UObject;

UCLASS()
class SHIPEDITOR_API UMyCommonMessagingSubsystem : public UCommonMessagingSubsystem
{
	GENERATED_BODY()

public:
	UMyCommonMessagingSubsystem() {}

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void ShowConfirmation(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback = FCommonMessagingResultDelegate()) override;
	virtual void ShowError(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback = FCommonMessagingResultDelegate()) override;

	virtual void ShowOpenFileDialog(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback) override;
	virtual void ShowSaveFileDialog(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback) override;

private:
	UPROPERTY()
	TSubclassOf<UCommonGameDialog> ConfirmationDialogClassPtr;

	UPROPERTY()
	TSubclassOf<UCommonGameDialog> ErrorDialogClassPtr;

	UPROPERTY()
	TSubclassOf<UCommonGameDialog> OpenFileDialogClassPtr;

	UPROPERTY()
	TSubclassOf<UCommonGameDialog> SaveFileDialogClassPtr;

	UPROPERTY(config)
	TSoftClassPtr<UCommonGameDialog> ConfirmationDialogClass;

	UPROPERTY(config)
	TSoftClassPtr<UCommonGameDialog> ErrorDialogClass;

	UPROPERTY(config)
	TSoftClassPtr<UCommonGameDialog> OpenFileDialogClass;

	UPROPERTY(config)
	TSoftClassPtr<UCommonGameDialog> SaveFileDialogClass;
};