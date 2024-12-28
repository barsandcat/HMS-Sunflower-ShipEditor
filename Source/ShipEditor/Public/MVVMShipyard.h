// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

#include "MVVMShipyard.generated.h"

UCLASS()
class SHIPEDITOR_API UMVVMShipyard : public UMVVMViewModelBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	float Test = 5;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 BrushId = 0;

public:
	void SetTest(float NewTest);

	void SetBrushId(int32 Id);
	int32 GetBrushId() const;
};
