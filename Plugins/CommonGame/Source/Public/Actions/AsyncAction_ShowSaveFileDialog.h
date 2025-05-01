// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_ShowSaveFileDialog.generated.h"

enum class ECommonMessagingResult : uint8;

class FText;
class UCommonGameDialogDescriptor;
class ULocalPlayer;
struct FFrame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCommonMessagingSaveResultMCDelegate, ECommonMessagingResult, Result, FString, Name);

UCLASS()
class COMMONGAME_API UAsyncAction_ShowSaveFileDialog : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObject"))
	static UAsyncAction_ShowSaveFileDialog* ShowSaveFileDialog(UObject* InWorldContextObject, FText Title, FText Message);

	virtual void Activate() override;

public:
	UPROPERTY(BlueprintAssignable)
	FCommonMessagingSaveResultMCDelegate OnResult;

private:
	void HandleResult(ECommonMessagingResult Result, FString Name);

	UPROPERTY(Transient)
	TObjectPtr<UObject> WorldContextObject;

	UPROPERTY(Transient)
	TObjectPtr<ULocalPlayer> TargetLocalPlayer;

	UPROPERTY(Transient)
	TObjectPtr<UCommonGameDialogDescriptor> Descriptor;
};