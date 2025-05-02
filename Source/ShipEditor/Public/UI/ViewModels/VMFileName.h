// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "VMFileName.generated.h"

UCLASS()
class SHIPEDITOR_API UVMFileName : public UMVVMViewModelBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	FString Name;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	bool IsDirectory;

public:
	void SetName(const FString& name);
	FString GetName() const;

	void SetIsDirectory(bool isDirectory);
	bool GetIsDirectory() const;
};
