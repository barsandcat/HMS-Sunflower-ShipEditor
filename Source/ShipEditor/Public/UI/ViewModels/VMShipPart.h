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
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	FText Name;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	int32 PartId = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	int32 CategoryId = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	int32 Elevation = 0;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	bool DynamicMount = false;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	bool LoadBearing = false;

public:
	void SetName(const FText& name);
	FText GetName() const;
	void SetPartId(int32 part_id);
	int32 GetPartId() const;
	void SetCategoryId(int32 category_id);
	int32 GetCategoryId() const;
	void SetElevation(int32 elevation);
	int32 GetElevation() const;
	void SetDynamicMount(bool dynamic_mount);
	bool GetDynamicMount() const;
	void SetLoadBearing(bool load_bearing);
	bool GetLoadBearing() const;
};

typedef TArray<TObjectPtr<UVMShipPart>> TUVMShipPartArray;
