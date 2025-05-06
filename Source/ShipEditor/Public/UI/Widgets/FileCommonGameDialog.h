// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Messaging/CommonGameDialog.h"

#include "FileCommonGameDialog.generated.h"

class IWidgetCompilerLog;

class UCommonTextBlock;
class UInputAction;

UCLASS()
class SHIPEDITOR_API UFileCommonGameDialog : public UCommonGameDialog
{
	GENERATED_BODY()
public:
	virtual void SetupDialog(UCommonGameDialogDescriptor* descriptor, FCommonMessagingResultDelegate result_callback);
	virtual void KillDialog();

private:
	FCommonMessagingResultDelegate OnResultCallback;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> TextTitle;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> CancelAction;
};
