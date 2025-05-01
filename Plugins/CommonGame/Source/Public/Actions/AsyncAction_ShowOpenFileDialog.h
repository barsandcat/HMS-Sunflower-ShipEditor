// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_ShowOpenFileDialog.generated.h"


enum class ECommonMessagingResult : uint8;

class FText;
class UCommonGameDialogDescriptor;
class ULocalPlayer;
struct FFrame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCommonMessagingOpenResultMCDelegate, ECommonMessagingResult, Result, FString, Name);

UCLASS()
class COMMONGAME_API UAsyncAction_ShowOpenFileDialog : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, meta = (BlueprintInternalUseOnly = "true", WorldContext = "InWorldContextObject"))
	static UAsyncAction_ShowOpenFileDialog* ShowOpenFileDialog(UObject* InWorldContextObject, FText Title, FText Message);

	virtual void Activate() override;

public:
	UPROPERTY(BlueprintAssignable)
	FCommonMessagingOpenResultMCDelegate OnResult;

private:
	void HandleResult(ECommonMessagingResult Result, FString Name);

	UPROPERTY(Transient)
	TObjectPtr<UObject> WorldContextObject;

	UPROPERTY(Transient)
	TObjectPtr<ULocalPlayer> TargetLocalPlayer;

	UPROPERTY(Transient)
	TObjectPtr<UCommonGameDialogDescriptor> Descriptor;
};