// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Messaging/CommonGameFileDialog.h"

#include "MyCommonGameFileDialog.generated.h"

class IWidgetCompilerLog;

class UCommonTextBlock;
class UInputAction;
class UDynamicEntryBox;

UCLASS()
class SHIPEDITOR_API UMyCommonGameFileDialog : public UCommonGameFileDialog
{
	GENERATED_BODY()
public:
	virtual void SetupDialog(bool save_dialog, FText header, FString file, FFileDialogResultDelegate result_callback);

	UFUNCTION(BlueprintCallable)
	void CloseDialogWithResult(ECommonMessagingResult result, FString file);

	UFUNCTION(BlueprintImplementableEvent)
	void OnConfirmClicked();

	UFUNCTION(BlueprintImplementableEvent)
	void OnCancelClicked();

	UFUNCTION(BlueprintImplementableEvent)
	void OnSetup(bool is_save_dialog, const FString& file);

protected:
	UPROPERTY(Transient, BlueprintReadWrite)
	FString File;

	UPROPERTY(Transient, BlueprintReadOnly)
	bool IsSaveDialog = false;

private:
	FFileDialogResultDelegate OnResultCallback;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UDynamicEntryBox> EntryBoxButtons;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> TextTitle;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> CancelAction;
};
