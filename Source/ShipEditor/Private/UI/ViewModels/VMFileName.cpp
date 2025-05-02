// Copyright (c) 2025, sillygilly. All rights reserved.


#include "UI/ViewModels/VMFileName.h"

void UVMFileName::SetName(const FString& name)
{
	UE_MVVM_SET_PROPERTY_VALUE(Name, name);
}

FString UVMFileName::GetName() const
{
	return Name;
}

void UVMFileName::SetIsDirectory(bool isDirectory)
{
	UE_MVVM_SET_PROPERTY_VALUE(IsDirectory, isDirectory);
}

bool UVMFileName::GetIsDirectory() const
{
	return IsDirectory;
}
