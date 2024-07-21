#pragma once

#include "CoreMinimal.h"
#include "EditStateInterface.h"
#include "GameFramework/GameMode.h"

#include "MyGameMode.generated.h"

UCLASS()
class SHIPEDITOR_API AMyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void StartPlay() override;

	UPROPERTY(BlueprintReadWrite)
	TScriptInterface<IEditStateInterface> EditState;
};
