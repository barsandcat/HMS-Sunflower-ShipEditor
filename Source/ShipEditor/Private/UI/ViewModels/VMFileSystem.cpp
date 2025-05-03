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
	class FShipPlansVisitor : public IPlatformFile::FDirectoryVisitor
	{
	public:
		TArray<UVMFileName*> Result;

		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool is_directory)
		{
			FString file_name = FPaths::GetCleanFilename(FilenameOrDirectory);
			if (!is_directory)
			{
				if (FPaths::GetExtension(file_name, false).ToLower() != TEXT("shipplan"))
				{
					return true;
				}
				else
				{
					file_name = FPaths::GetBaseFilename(file_name);
				}
			}
			UVMFileName* vm = NewObject<UVMFileName>();
			vm->SetName(file_name);
			vm->SetIsDirectory(is_directory);
			Result.Add(vm);
			return true;
		}
	};

	IFileManager& file_manager = IFileManager::Get();

	FString path = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("ShipPlans"));
	UE_LOG(LogTemp, Warning, TEXT("path: %s"), *path);
	file_manager.MakeDirectory(*path, true);
	FShipPlansVisitor visitor;
	file_manager.IterateDirectory(*path, visitor);

	return visitor.Result;
}
