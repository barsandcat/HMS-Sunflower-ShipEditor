// Copyright (c) 2024, sillygilly. All rights reserved.

#pragma once

#include "CommonActivatableWidget.h"
#include "CommonListView.h"
#include "CoreMinimal.h"

#include "ShipPartBrowserActivatableWidget.generated.h"

UCLASS()
class UDummy : public UObject
{
	GENERATED_BODY()
};

UCLASS()
class SHIPEDITOR_API UShipPartBrowserActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

	virtual bool Initialize() override;
	TObjectPtr<UDummy> Dummy;

public:
	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<UCommonListView> PartList;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<UCommonListView> FilterList;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget))
	TObjectPtr<UCommonListView> CategoryList;
};
