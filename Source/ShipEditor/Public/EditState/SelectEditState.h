// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditStateInterface.h"
#include "UObject/NoExportTypes.h"

#include "SelectEditState.generated.h"

/**
 *
 */
UCLASS()
class SHIPEDITOR_API USelectEditState : public UObject, public IEditStateInterface
{
	GENERATED_BODY()
public:
	USelectEditState(const FObjectInitializer& ObjectInitializer);
	virtual ~USelectEditState();
	TScriptInterface<IEditStateInterface> SelectPart_Implementation(int partId) override;
	TScriptInterface<IEditStateInterface> Cancel_Implementation() override;
};
