// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditStateInterface.h"
#include "UObject/NoExportTypes.h"

#include "PaintEditState.generated.h"

/**
 *
 */
UCLASS()
class SHIPEDITOR_API UPaintEditState : public UObject, public IEditStateInterface
{
	GENERATED_BODY()
public:
	UPaintEditState(const FObjectInitializer& ObjectInitializer);
	virtual ~UPaintEditState();
	TScriptInterface<IEditStateInterface> SelectPart_Implementation(int partId) override;
	TScriptInterface<IEditStateInterface> Cancel_Implementation() override;
};
