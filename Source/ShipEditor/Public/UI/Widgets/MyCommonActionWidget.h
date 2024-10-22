// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CommonActionWidget.h"
#include "CoreMinimal.h"

#include "MyCommonActionWidget.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;

UCLASS()
class SHIPEDITOR_API UMyCommonActionWidget : public UCommonActionWidget
{
	GENERATED_BODY()

public:
	//~ Begin UCommonActionWidget interface
	virtual FSlateBrush GetIcon() const override;
	//~ End of UCommonActionWidget interface

	/** The Enhanced Input Action that is associated with this Common Input action. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	const TObjectPtr<UInputAction> AssociatedInputAction;

private:
	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;
};