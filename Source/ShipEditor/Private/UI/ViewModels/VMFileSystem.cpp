// Copyright (c) 2025, sillygilly. All rights reserved.

#include "UI/ViewModels/VMFileSystem.h"

#include "HAL/FileManagerGeneric.h"

void UVMFileSystem::SetCWD(const FString& new_cwd)
{
	IFileManager::Get().MakeDirectory(*new_cwd, true);
	UE_MVVM_SET_PROPERTY_VALUE(CWD, new_cwd);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetFiles);
}

FString UVMFileSystem::GetCWD() const
{
	return CWD;
}

UVMFileSystem::UVMFileSystem(const FObjectInitializer& object_initializer)
    : UMVVMViewModelBase(object_initializer)
{
	SetCWD(FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("ShipPlans")));
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

	FShipPlansVisitor visitor;
	IFileManager::Get().IterateDirectory(*CWD, visitor);

	return visitor.Result;
}
