// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTileStopMovingDelegate);

UCLASS()
class PUZZLEANSWER1_API ATile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	UStaticMeshComponent* CubeMeshComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	// tile 간의 구분 -> FName
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
	FName TileType;

	// 
	bool IsMatching(const ATile* OtherTile) const;

	FName GetTileType() const;

	void DestoryAndSpawnEmitter();

	bool bIsMoving = false;
	void SetMoving(bool IsMoving);
	FVector TargetLocation;
	void SetTargetLocation(FVector NewTargetLocation);

	FOnTileStopMovingDelegate OnTileStopMovingDelegate;

};
