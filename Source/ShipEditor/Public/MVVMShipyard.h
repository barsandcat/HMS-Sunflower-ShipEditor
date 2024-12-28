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

public:
	void SetTest(float NewTest)
	{
		UE_MVVM_SET_PROPERTY_VALUE(Test, NewTest);
	}
};
