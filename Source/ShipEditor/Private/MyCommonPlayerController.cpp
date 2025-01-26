// Copyright (c) 2024, sillygilly. All rights reserved.

#include "MyCommonPlayerController.h"

#include "CommonInputSubsystem.h"
#include "ShipyardSubsystem.h"

void AMyCommonPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(Cast<ULocalPlayer>(Player));
	if (!CommonInputSubsystem || !CommonInputSubsystem->IsUsingPointerInput())
	{
		return;
	}
	const UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return;
	}

	UShipyardSubsystem* Shipyard = GameInstance->GetSubsystem<UShipyardSubsystem>();
	if (!Shipyard)
	{
		return;
	}
	Shipyard->SetCursorPosition(GetMouseWorldPosition(0));
}

TOptional<FVector> AMyCommonPlayerController::GetMouseWorldPosition(double PlaneY)
{
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer || !LocalPlayer->ViewportClient)
	{
		return {};
	}

	FVector2D MouseScreenPosition = FVector2D::Zero();
	if (!LocalPlayer->ViewportClient->GetMousePosition(MouseScreenPosition))
	{
		return {};
	}

	FVector DeprojectLocation = FVector::Zero();
	FVector DeprojectDirection = FVector::Zero();
	if (!DeprojectScreenPositionToWorld(MouseScreenPosition.X, MouseScreenPosition.Y, DeprojectLocation, DeprojectDirection))
	{
		return {};
	}

	FVector WorldMousePosition = FVector::Zero();
	WorldMousePosition.Y = PlaneY;
	double DistanceToPlaneY = PlaneY - DeprojectLocation.Y;

	WorldMousePosition.X = DeprojectLocation.X;
	if (!FMath::IsNearlyZero(DeprojectDirection.X))
	{
		WorldMousePosition.X = DeprojectLocation.X + (DistanceToPlaneY * DeprojectDirection.X) / DeprojectDirection.Y;
	}

	WorldMousePosition.Z = DeprojectLocation.Z;
	if (!FMath::IsNearlyZero(DeprojectDirection.Z))
	{
		WorldMousePosition.Z = DeprojectLocation.Z + (DistanceToPlaneY * DeprojectDirection.Z) / DeprojectDirection.Y;
	}

	return WorldMousePosition;
}