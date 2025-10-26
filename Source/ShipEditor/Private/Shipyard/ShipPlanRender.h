// Copyright (c) 2025, sillygilly. All rights reserved.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ShipPlanRender.generated.h"

UCLASS()
class AShipPlanRender : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShipPlanRender();

	void AddWall(const FInt32Point& pos);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMesh* WallMesh;

	UPROPERTY()
	TMap<FInt32Point, UStaticMeshComponent*> WallMeshComponents;

	UPROPERTY()
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	float MeshSpacing = 200.0f;
};
