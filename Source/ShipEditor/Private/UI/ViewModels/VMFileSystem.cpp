// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/ViewModels/VMFileSystem.h"

#include "HAL/FileManagerGeneric.h"

void UVMFileSystem::SetCWD(const FString& cwd)
{
	UE_MVVM_SET_PROPERTY_VALUE(CWD, cwd);
}

FString UVMFileSystem::GetCWD() const
{
	return CWD;
}

TArray<UVMFileName*> UVMFileSystem::GetFiles() const
{
	IFileManager& FileManager = IFileManager::Get();

	FString path = "*.*";
	TArray<FString> output;
	FileManager.FindFiles(output, *path, true, true);
	TArray<UVMFileName*> files;
	for (const FString& file : output)
	{
		UVMFileName* fileName = NewObject<UVMFileName>();
		fileName->SetName(file);
		fileName->SetIsDirectory(false);
		files.Add(fileName);
	}

	return files;
}
