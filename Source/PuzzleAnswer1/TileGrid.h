// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileGrid.generated.h"

class ATile;

UCLASS()
class PUZZLEANSWER1_API ATileGrid : public AActor
{
	GENERATED_BODY()


public:
	// Sets default values for this actor's properties
	ATileGrid();
	
protected:
	virtual void BeginPlay() override;

public:
	// grid width, height
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Grid")
	int32 GridWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Grid")
	int32 GridHeight;
	// tile 의 cube mesh 를 뭘 고르느냐에 따라 일일이 수정해야 합니다
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "Tile Grid")
	float TileSize = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Grid")
	TArray<UMaterial*> Materials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Grid")
	TSubclassOf<ATile> TileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Grid")
	TArray<ATile*> TileArray;

	// grid initialize
	void InitializeGrid();

	// get tile of Specific position
	ATile* GetTileAt(int32 Row, int32 Col) const;

	// set tile of specific position
	void SetTileAt(int32 Row, int32 Col, ATile* Tile);

	// Tile 을 스폰하고 Material 을 결정하고 BoxExtent를 결정하고 X, Y 에 따른 위치를 결정한다.
	ATile* SpawnAndInitializeTile(int32 Row, int32 Col);
	// x 행 y 열 타일의 위치를 리턴
	FVector GetTileLocation(int32 Row, int32 Col) const;
	// 전체 TileArray 를 순회하여 연속된 타입이 얼마나 존재하는지 확인한다.
	// 
	void CheckRepeatedTiles(TArray<int32>& NumOfRepeatedTiles, TSet<ATile*>& RepeatedTiles);

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "Tile Grid")
	TArray<int32> NumOfRepeatedTilesArray;
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "Tile Grid")
	TSet<ATile*> RepeatedTilesSet;
	
};

