// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "VMShipPlan.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSaveShipPlan, const FString&, Name);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadShipPlan, const FString&, Name);

UCLASS()
class SHIPEDITOR_API UVMShipPlan : public UMVVMViewModelBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	FString Name;

public:
	void SetName(const FString& name);
	FString GetName() const;

	UPROPERTY(BlueprintCallable)
	FSaveShipPlan SaveShipPlan;

	UPROPERTY(BlueprintCallable)
	FLoadShipPlan LoadShipPlan;
};
