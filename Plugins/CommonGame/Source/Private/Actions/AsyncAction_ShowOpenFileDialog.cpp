// Copyright (c) 2025, sillygilly. All rights reserved.

#include "Actions/AsyncAction_ShowOpenFileDialog.h"

#include "Engine/GameInstance.h"
#include "Messaging/CommonGameDialog.h"
#include "Messaging/CommonMessagingSubsystem.h"

UAsyncAction_ShowOpenFileDialog* UAsyncAction_ShowOpenFileDialog::ShowOpenFileDialog(UObject* InWorldContextObject, FText Header, FString File)
{
	UAsyncAction_ShowOpenFileDialog* Action = NewObject<UAsyncAction_ShowOpenFileDialog>();
	Action->WorldContextObject = InWorldContextObject;
	Action->Header = Header;
	Action->File = File;
	Action->RegisterWithGameInstance(InWorldContextObject);

	return Action;
}

void UAsyncAction_ShowOpenFileDialog::Activate()
{
	if (WorldContextObject && !TargetLocalPlayer)
	{
		if (UUserWidget* UserWidget = Cast<UUserWidget>(WorldContextObject))
		{
			TargetLocalPlayer = UserWidget->GetOwningLocalPlayer<ULocalPlayer>();
		}
		else if (APlayerController* PC = Cast<APlayerController>(WorldContextObject))
		{
			TargetLocalPlayer = PC->GetLocalPlayer();
		}
		else if (UWorld* World = WorldContextObject->GetWorld())
		{
			if (UGameInstance* GameInstance = World->GetGameInstance<UGameInstance>())
			{
				TargetLocalPlayer = GameInstance->GetPrimaryPlayerController(false)->GetLocalPlayer();
			}
		}
	}

	if (TargetLocalPlayer)
	{
		if (UCommonMessagingSubsystem* Messaging = TargetLocalPlayer->GetSubsystem<UCommonMessagingSubsystem>())
		{
			FFileDialogResultDelegate ResultCallback = FFileDialogResultDelegate::CreateUObject(this, &UAsyncAction_ShowOpenFileDialog::HandleResult);
			Messaging->ShowFileDialog(false, Header, File, ResultCallback);
			return;
		}
	}

	// If we couldn't make the confirmation, just handle an unknown result and broadcast nothing
	HandleResult(ECommonMessagingResult::Unknown, "");
}

void UAsyncAction_ShowOpenFileDialog::HandleResult(ECommonMessagingResult ConfirmationResult, FString ResultFile)
{
	OnResult.Broadcast(ConfirmationResult, ResultFile);

	SetReadyToDestroy();
}
