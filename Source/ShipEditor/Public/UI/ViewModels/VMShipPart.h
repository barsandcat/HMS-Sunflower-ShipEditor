// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

#include "VMShipPart.generated.h"

UCLASS(BlueprintType)
class UVMShipPart : public UMVVMViewModelBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	FString Name = "PartName";

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	int32 PartId = 0;

public:
	void SetName(const FString& NewName);
	void SetPartId(int32 NewPartId);
};

typedef TArray<TObjectPtr<UVMShipPart>> TUVMShipPartArray;
