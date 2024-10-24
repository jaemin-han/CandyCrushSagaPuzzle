// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTileStopMovingDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTileStartMovingDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileClicked, ATile*, ClickedTile);

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

	void DestroyAndSpawnEmitter();

	bool bIsMoving = false;
	FVector TargetLocation;
	void SetTargetLocation(FVector NewTargetLocation);

	FOnTileStopMovingDelegate OnTileStopMovingDelegate;
	FOnTileStartMovingDelegate OnTileStartMovingDelegate;

	void StartMoving();
	void StopMoving();

	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;

	FOnTileClicked OnTileClicked;

private:
	int32 Row;
	int32 Col;

public:
	void SetRow(const int32 NewRow) { Row = NewRow; };
	void SetCol(const int32 NewCol) { Col = NewCol; };
	int32 GetRow() const { return Row; };
	int32 GetCol() const { return Col; };

	void SetMaterialEmission(bool bEmission) const;
};
