// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

// 타일 유형
UENUM(BlueprintType)
enum class ETileType: uint8
{
	Red UMETA(DisplayName = "Red"),
	Green UMETA(DisplayName = "Green"),
	Blue UMETA(DisplayName = "Blue"),
	White UMETA(DisplayName = "White"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTileStopMoveDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTileStartMoveDelegate);

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

private:
	ETileType GetRandomTileTypeEnum() const;
	// Tile이 목표로 할 위치, bIsMoving 이 참이면 Tick 에서 타일을 해당 위치로 이동시킨다
	FVector TargetLocation;

	bool bIsMoving = false;
	void StopMoving();

	// tile 이 클릭당했을 때 실행될 함수
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;

	UPROPERTY(VisibleAnywhere, Category = "Tile")
	int32 Row;
	UPROPERTY(VisibleAnywhere, Category = "Tile")
	int32 Col;

public:
	void SetRow(const int32 NewRow) { Row = NewRow; };
	void SetCol(const int32 NewCol) { Col = NewCol; };
	int32 GetRow() const { return Row; };
	int32 GetCol() const { return Col; };

	FOnTileStopMoveDelegate OnTileStopMoveDelegate;
	FOnTileStartMoveDelegate OnTileStartMoveDelegate;
	FOnTileClicked OnTileClicked;

	FVector GetTargetLoc() const { return TargetLocation; };
	void SetTargetLoc(const FVector& NewLocation);
	void StartMoving();


	void SetMaterialEmission(bool bEmission) const;
	void DestroyAndSpawnEmitter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile Properties")
	ETileType TileType;
};
