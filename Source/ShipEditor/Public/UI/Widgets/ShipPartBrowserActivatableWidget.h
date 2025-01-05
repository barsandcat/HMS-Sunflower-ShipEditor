// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CommonActivatableWidget.h"
#include "CommonListView.h"
#include "CoreMinimal.h"

#include "ShipPartBrowserActivatableWidget.generated.h"

UCLASS(BlueprintType)
class UListFilterOption : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	FString Name = "Hehehe";
};

UCLASS()
class SHIPEDITOR_API UShipPartBrowserActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

	virtual bool Initialize() override;
	TArray<TObjectPtr<UListFilterOption>> Catergories;
	TArray<TObjectPtr<UListFilterOption>> Filters;

public:
	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<UCommonListView> FilterList;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<UCommonListView> CategoryList;
};
