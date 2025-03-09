// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

#include "VMBrush.generated.h"

UCLASS()
class SHIPEDITOR_API UVMBrush : public UMVVMViewModelBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	bool Ready = false;

public:
	void SetReady(bool NewReady);
	bool GetReady() const;
};
