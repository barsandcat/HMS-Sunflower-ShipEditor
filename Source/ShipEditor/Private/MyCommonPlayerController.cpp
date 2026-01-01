// Copyright (c) 2024, sillygilly. All rights reserved.

#include "MyCommonPlayerController.h"

#include "CommonInputSubsystem.h"
#include "Shipyard/ShipyardSubsystem.h"

void AMyCommonPlayerController::PlayerTick(float delta_time)
{
	Super::PlayerTick(delta_time);
	UCommonInputSubsystem* common_input_subsystem = UCommonInputSubsystem::Get(Cast<ULocalPlayer>(Player));
	if (!common_input_subsystem || !common_input_subsystem->IsUsingPointerInput())
	{
		return;
	}

	UWorld* world = GetWorld();
	if (!world)
	{
		return;
	}

	UShipyardSubsystem* shipyard = world->GetSubsystem<UShipyardSubsystem>();
	if (!shipyard)
	{
		return;
	}
	const auto mouse_position = GetMouseWorldPosition(0);
	shipyard->SetCursorPosition(mouse_position);
	shipyard->SetBrushPosition(mouse_position);
}

TOptional<FVector> AMyCommonPlayerController::GetMouseWorldPosition(double plane_y)
{
	ULocalPlayer* local_player = GetLocalPlayer();
	if (!local_player || !local_player->ViewportClient)
	{
		return {};
	}

	FVector2D mouse_screen_position = FVector2D::Zero();
	if (!local_player->ViewportClient->GetMousePosition(mouse_screen_position))
	{
		return {};
	}

	FVector deproject_location = FVector::Zero();
	FVector deproject_direction = FVector::Zero();
	if (!DeprojectScreenPositionToWorld(mouse_screen_position.X, mouse_screen_position.Y, deproject_location, deproject_direction))
	{
		return {};
	}

	FVector world_mouse_position = FVector::Zero();
	world_mouse_position.Y = plane_y;
	double distance_to_plane_y = plane_y - deproject_location.Y;

	world_mouse_position.X = deproject_location.X;
	if (!FMath::IsNearlyZero(deproject_direction.X))
	{
		world_mouse_position.X = deproject_location.X + (distance_to_plane_y * deproject_direction.X) / deproject_direction.Y;
	}

	world_mouse_position.Z = deproject_location.Z;
	if (!FMath::IsNearlyZero(deproject_direction.Z))
	{
		world_mouse_position.Z = deproject_location.Z + (distance_to_plane_y * deproject_direction.Z) / deproject_direction.Y;
	}

	return world_mouse_position;
}
