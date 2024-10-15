// Fill out your copyright notice in the Description page of Project Settings.


#include "TileGrid.h"

#include "Tile.h"
#include "Async/ParallelFor.h"
#include "Async/Async.h"


// Sets default values
ATileGrid::ATileGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GridWidth = 8;
	GridHeight = 8;
	TileArray.SetNum(GridWidth * GridHeight);
	Materials.SetNum(4);
	NumOfRepeatedTilesArray.SetNum(GridWidth > GridHeight ? GridWidth : GridHeight);
}

void ATileGrid::BeginPlay()
{
	Super::BeginPlay();

	InitializeGrid();
	CheckRepeatedTiles(NumOfRepeatedTilesArray, RepeatedTilesSet);

	// 디버깅을 위해 RepeatedTilesSet의 모든 ATile 위치에 DebugCircle 생성
	for (ATile* Tile : RepeatedTilesSet)
	{
		if (Tile)
		{
			FVector TileLocation = Tile->GetActorLocation();
			TileLocation.Z += 200;
			DrawDebugSphere(GetWorld(), TileLocation, 20.0f, 12, FColor::Black, false, 360.0f); // 5초 동안 유지
		}
	}
}

void ATileGrid::InitializeGrid()
{
	for (int32 Row = 0; Row < GridWidth; Row++)
	{
		for (int32 Col = 0; Col < GridHeight; Col++)
		{
			ATile* NewTile = SpawnAndInitializeTile(Row, Col);
			SetTileAt(Row, Col, NewTile);
		}
	}
}

ATile* ATileGrid::GetTileAt(int32 Row, int32 Col) const
{
	if (Row >= 0 && Row < GridHeight && Col >= 0 && Col < GridWidth)
	{
		return TileArray[Row * GridWidth + Col];
	}
	else
	{
		return nullptr;
	}
}

void ATileGrid::SetTileAt(int32 Row, int32 Col, ATile* Tile)
{
	if (Row >= 0 && Row < GridHeight && Col >= 0 && Col < GridWidth)
	{
		TileArray[Row * GridWidth + Col] = Tile;
	}
}

ATile* ATileGrid::SpawnAndInitializeTile(int32 Row, int32 Col)
{
	// box extent 설정
	const FVector Position = GetTileLocation(Row, Col);
	ATile* NewTile = GetWorld()->SpawnActor<ATile>(TileClass, Position, FRotator());

	// material 설정

	UMaterial* Material;

	if (NewTile->TileType == "Red")
	{
		Material = Materials[0];
	}
	else if (NewTile->TileType == "Blue")
	{
		Material = Materials[1];
	}
	else if (NewTile->TileType == "Green")
	{
		Material = Materials[2];
	}
	else if (NewTile->TileType == "White")
	{
		Material = Materials[3];
	}
	else
	{
		// 빌드 안돼서 넣어놓음
		Material = Materials[3];
	}

	int32 NumMaterial = NewTile->CubeMeshComponent->GetMaterials().Num();
	for (int32 MaterialIndex = 0; MaterialIndex < NumMaterial; MaterialIndex++)
	{
		NewTile->CubeMeshComponent->SetMaterial(MaterialIndex, Material);
	}

	return NewTile;
}

FVector ATileGrid::GetTileLocation(int32 Row, int32 Col) const
{
	// box extent 설정
	FVector Position;
	// set x position by Tilesize and x, y
	// Position.X = - GridWidth * TileSize / 2 + TileSize / 2 + x * TileSize / 2;
	Position.X = (GridWidth - 1 - 2 * Col) * TileSize / 2;
	Position.Y = (GridHeight - 1 - 2 * Row) * TileSize / 2;

	return Position;
}

void ATileGrid::CheckRepeatedTiles(TArray<int32>& NumOfRepeatedTiles, TSet<ATile*>& RepeatedTiles)
{
	// 가로 방향으로 (Col 으로)
	for (int32 Row = 0; Row < GridHeight; Row++)
	{
		for (int32 Col = 0; Col < GridWidth;)
		{
			const int32 StartCol = Col;
			ATile* StartTile = GetTileAt(Row, StartCol);
			int32 EndCol = StartCol + 1;

			while (EndCol < GridWidth)
			{
				ATile* NextTile = GetTileAt(Row, EndCol);

				// 다르면 다음 Col 을 탐색
				if (StartTile->TileType != NextTile->TileType)
				{
					break;
				}

				// 같으면 연속된 타일 숫자 하나 증가
				EndCol++;
			}
			// 연속된 타일의 수가 3 이상이면
			if (EndCol - StartCol >= 3)
			{
				for (int32 IndexCol = StartCol; IndexCol < EndCol; IndexCol++)
				{
					// 해당 타일들을 RepeatedTiles 에 추가함
					RepeatedTiles.Add(GetTileAt(Row, IndexCol));
					// Row, IndexCol Debug
					UE_LOG(LogTemp, Log, TEXT("Row, IndexCol: %d %d"), Row, IndexCol);
				}
				// 해당 연속된 타일 수를 NumOfRepeated 에 반영
				NumOfRepeatedTiles[EndCol - StartCol]++;
				UE_LOG(LogTemp, Log, TEXT("NumOfRepeatedTiles: %d"), EndCol - StartCol);
			}
			// EndCol 부터 다시 검사
			Col = EndCol;
		}
	}

	// 세로 방향으로 (Row)
	for (int32 Col = 0; Col < GridHeight; Col++)
	{
		for (int32 Row = 0; Row < GridWidth;)
		{
			const int32 StartRow = Row;
			ATile* StartTile = GetTileAt(StartRow, Col);
			int32 EndRow = StartRow + 1;

			while (EndRow < GridHeight)
			{
				ATile* NextTile = GetTileAt(EndRow, Col);

				// 다르면 다음 Col 을 탐색
				if (StartTile->TileType != NextTile->TileType)
				{
					break;
				}

				// 같으면 연속된 타일 숫자 하나 증가
				EndRow++;
			}
			// 연속된 타일의 수가 3 이상이면
			if (EndRow - StartRow >= 3)
			{
				for (int32 IndexRow = StartRow; IndexRow < EndRow; IndexRow++)
				{
					// 해당 타일들을 RepeatedTiles 에 추가함
					RepeatedTiles.Add(GetTileAt(IndexRow, Col));
					// Row, IndexCol Debug
					UE_LOG(LogTemp, Log, TEXT("Row, IndexCol: %d %d"), IndexRow, Col);
				}
				// 해당 연속된 타일 수를 NumOfRepeated 에 반영
				NumOfRepeatedTiles[EndRow - StartRow]++;
				UE_LOG(LogTemp, Log, TEXT("NumOfRepeatedTiles: %d"), EndRow - StartRow);
			}
			// EndCol 부터 다시 검사
			Row = EndRow;
		}
	}
}
