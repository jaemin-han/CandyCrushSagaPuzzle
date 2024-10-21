// Fill out your copyright notice in the Description page of Project Settings.


#include "TileGrid.h"

#include "MyGameInstance.h"
#include "Tile.h"
#include "Async/ParallelFor.h"
#include "Async/Async.h"
#include "CommandPattern/SwapTilesCommand.h"
#include "CommandPattern/TileCommandInvocker.h"


// Sets default values
ATileGrid::ATileGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GridWidth = 6;
	GridHeight = 6;
	TileArray.SetNum(GridWidth * GridHeight);
	Materials.SetNum(4);
	// 3이면 0, 1, 2, 3 총 4개 필요하니까 1 더해준다
	NumOfRepeatedTilesArray.SetNum(GridWidth > GridHeight ? GridWidth + 1 : GridHeight + 1);
	CurrentState = ETileGridState::Idle;

	Invocker = CreateDefaultSubobject<UTileCommandInvocker>(TEXT("Invocker"));

	bGameOverPending = false;
}

void ATileGrid::BeginPlay()
{
	Super::BeginPlay();

	InitializeGrid();
	TransitionToState(ETileGridState::CheckingForRepeated);

	UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GetGameInstance());
	if (MyGameInstance)
	{
		OnScoreIncreased.AddDynamic(MyGameInstance, &UMyGameInstance::IncreasePlayerScore);
		OnMovesDecreased.AddDynamic(MyGameInstance, &UMyGameInstance::DecreaseRemainingMoves);

		MyGameInstance->OnGameOver.AddLambda([this]()
		{
			bGameOverPending = true;
		});
	}
}

void ATileGrid::TransitionToState(ETileGridState NewState)
{
	UE_LOG(LogTemp, Warning, TEXT("TransitionToState: %s to %s"), *UEnum::GetValueAsString(CurrentState),
	       *UEnum::GetValueAsString(NewState));
	CurrentState = NewState;
}

void ATileGrid::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	switch (CurrentState)
	{
	case ETileGridState::Idle:
		if (bGameOverPending)
		{
			TransitionToState(ETileGridState::GameOver);
		}
		
		if (IsValid(FirstClickedTile) && IsValid(SecondClickedTile))
		{
			ICommand* Command = new SwapTilesCommand(FirstClickedTile, SecondClickedTile);
			Invocker->ExecuteCommand(Command);
			TransitionToState(ETileGridState::SwapCheck);
		}
		break;
	case ETileGridState::SwapCheck:
		if (MovingTilesCounter.GetValue() <= 0)
		{
			FTilePair TilePair(FirstClickedTile, SecondClickedTile);
			if (ValidTilePairs.Contains(TilePair))
			{
				SwapClickedTileOnTileArray(FirstClickedTile, SecondClickedTile);
				TransitionToState(ETileGridState::CheckingForRepeated);
				ValidTilePairs.Empty();
			}
			else
			{
				Invocker->UndoLastCommand();
				TransitionToState(ETileGridState::Idle);
			}
			NotifyMoveDecrease();
			FirstClickedTile = nullptr;
			SecondClickedTile = nullptr;
		}
		break;
	case ETileGridState::CheckingForRepeated:
		// CheckingForPossibleTiles 에서 왔을 때를 대비하여, RepeatedTilesSet 이 비어 있을때만 검사를 진행한다.
		if (RepeatedTilesSet.IsEmpty())
		{
			CheckRepeatedTiles(NumOfRepeatedTilesArray, RepeatedTilesSet);
		}
		if (RepeatedTilesSet.IsEmpty())
		{
			TransitionToState(ETileGridState::CheckingForPossibleTiles);
		}
		else
		{
			TransitionToState(ETileGridState::RemovingMatches);
		}
		break;
	case ETileGridState::RemovingMatches:
		RemoveRepeatedTiles();
		CalculateAndBroadcastScore();
		TransitionToState(ETileGridState::CheckAllTilesToMoveAndDropTiles);
		break;
	case ETileGridState::CheckAllTilesToMoveAndDropTiles:
		MoveTilesDown();
		GenerateNewTiles();
		TransitionToState(ETileGridState::WaitUntilAllTilesStopMoving);
		break;
	case ETileGridState::WaitUntilAllTilesStopMoving:
		if (MovingTilesCounter.GetValue() <= 0)
		{
			CheckRepeatedTiles(NumOfRepeatedTilesArray, RepeatedTilesSet);
			if (RepeatedTilesSet.IsEmpty())
			{
				TransitionToState(ETileGridState::CheckingForPossibleTiles);
			}
			else
			{
				TransitionToState(ETileGridState::CheckingForRepeated);
			}
		}
		break;
	case ETileGridState::CheckingForPossibleTiles:
		SetValidTilePairs();
	// DebugValidTilePairs();
		if (ValidTilePairs.IsEmpty())
		{
			TransitionToState(ETileGridState::GameOver);
		}
		else
		{
			TransitionToState(ETileGridState::Idle);
		}
		break;
	case ETileGridState::GameOver:
		OnGameOver.Execute();
		break;
	default:
		break;
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
		if (Tile != nullptr)
		{
			Tile->SetTargetLocation(GetTileLocation(Row, Col));
			Tile->SetRow(Row);
			Tile->SetCol(Col);

			Tile->OnTileStartMovingDelegate.Clear();
			Tile->OnTileStartMovingDelegate.AddDynamic(this, &ATileGrid::OnTileStartedMoving);
			Tile->StartMoving();

			UE_LOG(LogTemp, Log, TEXT("Tile at Row: %d, Col: %d started moving. MovingTilesCount: %d"), Row, Col,
			       MovingTilesCounter.GetValue());

			// Todo: Tile 에 delegate binding 진행 (OnTileStoppedMoving)
			Tile->OnTileStopMovingDelegate.Clear();
			Tile->OnTileStopMovingDelegate.AddDynamic(this, &ATileGrid::OnTileStoppedMoving);
		}
		TileArray[Row * GridWidth + Col] = Tile;
	}
}

ATile* ATileGrid::SpawnAndInitializeTile(int32 Row, int32 Col)
{
	// box extent 설정
	const FVector Position = GetTileLocation(Row, Col);
	ATile* NewTile = GetWorld()->SpawnActor<ATile>(TileClass, Position, FRotator());

	// Click 관련 delegate 에 연결
	NewTile->OnTileClicked.AddDynamic(this, &ATileGrid::HandleOnTileClicked);

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
	Position.X = (GridWidth - 1 - 2 * Col) * TileSize / 2;
	Position.Y = (GridHeight - 1 - 2 * Row) * TileSize / 2;
	Position.Z = 0.0f;

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

void ATileGrid::RemoveRepeatedTiles()
{
	for (ATile* Tile : RepeatedTilesSet)
	{
		if (Tile)
		{
			int32 Index = TileArray.IndexOfByKey(Tile);
			if (Index != INDEX_NONE)
			{
				TileArray[Index] = nullptr;
			}
		}
		Tile->DestoryAndSpawnEmitter();
	}
	RepeatedTilesSet.Empty();
}

void ATileGrid::MoveTilesDown()
{
	for (int32 Col = 0; Col < GridWidth; Col++)
	{
		for (int32 Row = GridHeight - 1; Row >= 0; Row--)
		{
			ATile* CurrentTile = GetTileAt(Row, Col);
			if (CurrentTile == nullptr)
			{
				for (int32 AboveRow = Row - 1; AboveRow >= 0; AboveRow--)
				{
					ATile* AboveTile = GetTileAt(AboveRow, Col);
					if (AboveTile != nullptr)
					{
						UE_LOG(LogTemp, Log, TEXT("Moving tile from Row: %d, Col: %d to Row: %d, Col: %d"), AboveRow,
						       Col, Row, Col);
						SetTileAt(Row, Col, AboveTile);
						SetTileAt(AboveRow, Col, nullptr);
						break;
					}
				}
			}
		}
	}
}

void ATileGrid::GenerateNewTiles()
{
	for (int32 Col = 0; Col < GridWidth; Col++)
	{
		int32 EmptyCount = 0;
		for (int32 Row = 0; Row < GridHeight; Row++)
		{
			if (GetTileAt(Row, Col) != nullptr)
			{
				break;
			}
			EmptyCount++;
		}

		for (int32 i = 0; i < EmptyCount; i++)
		{
			ATile* NewTile = SpawnAndInitializeTile(GridHeight - 1, Col);
			NewTile->SetActorLocation(GetTileLocation(-i - 1, Col));

			SetTileAt(EmptyCount - 1 - i, Col, NewTile);
			UE_LOG(LogTemp, Log, TEXT("Spawned new tile at Row: %d, Col: %d"), EmptyCount - 1 - i, Col);
		}
	}
}

void ATileGrid::OnTileStoppedMoving()
{
	MovingTilesCounter.Decrement();
	UE_LOG(LogTemp, Log, TEXT("A tile stopped moving. Current MovingTilesCount: %d"), MovingTilesCounter.GetValue());
}

void ATileGrid::OnTileStartedMoving()
{
	MovingTilesCounter.Increment();
	UE_LOG(LogTemp, Log, TEXT("A tile started moving. Current MovingTilesCount: %d"), MovingTilesCounter.GetValue());
}

void ATileGrid::DebugValidTilePairs()
{
	for (const FTilePair& Pair : ValidTilePairs)
	{
		if (Pair.First && Pair.Second) // 유효한 포인터인지 확인
		{
			FVector Start = Pair.First->GetActorLocation();
			FVector End = Pair.Second->GetActorLocation();

			Start.Z += 100.0;
			End.Z += 100.0;
			// 디버그 라인 그리기 (2초 동안 유지, 흰색)
			DrawDebugLine(GetWorld(), Start, End, FColor::White, false, 2.0f, 0, 5.0f);

			// 로그로 출력 (디버깅 용도)
			UE_LOG(LogTemp, Log, TEXT("Drawing line between %s and %s"),
			       *Pair.First->GetName(), *Pair.Second->GetName());
		}
	}
}

void ATileGrid::SetValidTilePairs()
{
	// XOOX 가로
	for (int32 Row = 0; Row < GridHeight; Row++)
	{
		// 길이가 2를 구하니까
		for (int32 Col = 0; Col < GridWidth - 1; Col++)
		{
			ATile* FirstTile = GetTileAt(Row, Col);
			ATile* SecondTile = GetTileAt(Row, Col + 1);

			FName TileType = FirstTile->TileType;
			if (TileType != SecondTile->TileType)
				continue;

			// 왼쪽 타일 기준으로 위, 왼, 아래 타일을 검사한다
			CheckLeftTile(Row, Col - 1, TileType);
			CheckUpTile(Row, Col - 1, TileType);
			CheckDownTile(Row, Col - 1, TileType);

			// 오른쪽 타일 기준으로 위, 오, 아래 타일을 검사한다
			CheckUpTile(Row, Col + 2, TileType);
			CheckRightTile(Row, Col + 2, TileType);
			CheckDownTile(Row, Col + 2, TileType);
		}
	}

	// XOOX 세로
	for (int32 Col = 0; Col < GridWidth; Col++)
	{
		for (int32 Row = 0; Row < GridHeight - 1; Row++)
		{
			ATile* FirstTile = GetTileAt(Row, Col);
			ATile* SecondTile = GetTileAt(Row + 1, Col);
			FName TileType = FirstTile->TileType;

			if (TileType != SecondTile->TileType)
				continue;

			// 위쪽 타일을 기준으로 왼, 위, 오른 타일을 검사한다
			CheckLeftTile(Row - 1, Col, TileType);
			CheckUpTile(Row - 1, Col, TileType);
			CheckRightTile(Row - 1, Col, TileType);

			// 아래쪽 타일을 기준으로 왼, 아래, 오른 타일을 검사한다
			CheckLeftTile(Row + 2, Col, TileType);
			CheckDownTile(Row + 2, Col, TileType);
			CheckRightTile(Row + 2, Col, TileType);
		}
	}

	// OXO 가로
	for (int32 Row = 0; Row < GridHeight; Row++)
	{
		for (int32 Col = 0; Col < GridWidth - 2; Col++)
		{
			ATile* FirstTile = GetTileAt(Row, Col);
			ATile* ThirdTile = GetTileAt(Row, Col + 2);
			FName TileType = FirstTile->TileType;

			if (TileType != ThirdTile->TileType)
				continue;

			// 중간 타일을 기준으로 위, 아래 타일을 검사한다
			CheckUpTile(Row, Col + 1, TileType);
			CheckDownTile(Row, Col + 1, TileType);
		}
	}

	// OXO 세로
	for (int32 Col = 0; Col < GridWidth; Col++)
	{
		for (int32 Row = 0; Row < GridHeight - 2; Row++)
		{
			ATile* FirstTile = GetTileAt(Row, Col);
			ATile* ThirdTile = GetTileAt(Row + 2, Col);
			FName TileType = FirstTile->TileType;

			if (TileType != ThirdTile->TileType)
				continue;

			// 중간 타일을 기준으로 왼, 오른 타일을 검사한다
			CheckLeftTile(Row + 1, Col, TileType);
			CheckRightTile(Row + 1, Col, TileType);
		}
	}
}

void ATileGrid::CheckLeftTile(int32 Row, int32 Col, FName TileType)
{
	ATile* CurTile = GetTileAt(Row, Col);
	ATile* LeftTile = GetTileAt(Row, Col - 1);
	if (CurTile && LeftTile && LeftTile->TileType == TileType)
	{
		ValidTilePairs.Add(FTilePair(CurTile, LeftTile));
	}
}

void ATileGrid::CheckRightTile(int32 Row, int32 Col, FName TileType)
{
	ATile* CurTile = GetTileAt(Row, Col);
	ATile* RightTile = GetTileAt(Row, Col + 1);
	if (CurTile && RightTile && RightTile->TileType == TileType)
	{
		ValidTilePairs.Add(FTilePair(CurTile, RightTile));
	}
}

void ATileGrid::CheckUpTile(int32 Row, int32 Col, FName TileType)
{
	ATile* CurTile = GetTileAt(Row, Col);
	ATile* UpTile = GetTileAt(Row - 1, Col);
	if (CurTile && UpTile && UpTile->TileType == TileType)
	{
		ValidTilePairs.Add(FTilePair(CurTile, UpTile));
	}
}

void ATileGrid::CheckDownTile(int32 Row, int32 Col, FName TileType)
{
	ATile* CurTile = GetTileAt(Row, Col);
	ATile* DownTile = GetTileAt(Row + 1, Col);
	if (CurTile && DownTile && DownTile->TileType == TileType)
	{
		ValidTilePairs.Add(FTilePair(CurTile, DownTile));
	}
}

void ATileGrid::HandleOnTileClicked(ATile* ClickedTile)
{
	// Idle 상태에서만 클릭 입력을 받을 수 있다.
	if (CurrentState != ETileGridState::Idle)
		return;

	if (FirstClickedTile == nullptr)
	{
		FirstClickedTile = ClickedTile;
	}
	else if (SecondClickedTile == nullptr && ClickedTile != FirstClickedTile)
	{
		int32 FirstRow = FirstClickedTile->GetRow();
		int32 FirstCol = FirstClickedTile->GetCol();

		int32 SecondRow = ClickedTile->GetRow();
		int32 SecondCol = ClickedTile->GetCol();

		// FirstTile 의 상하좌우 타일만 선택 가능
		if ((FMath::Abs(SecondRow - FirstRow) == 1 && SecondCol == FirstCol) ||
			(FMath::Abs(SecondCol - FirstCol) == 1 && SecondRow == FirstRow))
		{
			SecondClickedTile = ClickedTile;
		}
	}
	else
	{
		// 둘 다 nullptr 이 아닌데 이 함수가 실해되는 일이 없게 할 것.
	}
}

void ATileGrid::SwapClickedTileOnTileArray(ATile* FirstTile, ATile* SecondTile)
{
	int32 FirstRow = FirstTile->GetRow();
	int32 FirstCol = FirstTile->GetCol();

	int32 SecondRow = SecondTile->GetRow();
	int32 SecondCol = SecondTile->GetCol();

	SetTileAt(FirstRow, FirstCol, SecondTile);
	SetTileAt(SecondRow, SecondCol, FirstTile);
}

void ATileGrid::CalculateAndBroadcastScore()
{
	int32 TotalScore = 0;

	// NumOfRepeatedTilesArray를 사용해 점수 계산
	for (int32 i = 3; i < NumOfRepeatedTilesArray.Num(); ++i)
	{
		if (NumOfRepeatedTilesArray[i] > 0)
		{
			int32 ScoreForThisCombo = i * 10 * NumOfRepeatedTilesArray[i]; // 예: 3개 = 30점, 4개 = 40점 등
			TotalScore += ScoreForThisCombo;
			UE_LOG(LogTemp, Log, TEXT("Combo of %d tiles: %d points"), i, ScoreForThisCombo);
		}
	}

	// 총 점수를 Delegate로 Broadcast
	if (TotalScore > 0)
	{
		OnScoreIncreased.Broadcast(TotalScore);
		UE_LOG(LogTemp, Log, TEXT("Total Score Broadcast: %d"), TotalScore);
	}
	// reset
	for (int32& Value: NumOfRepeatedTilesArray)
	{
		Value = 0;
	}
}

void ATileGrid::NotifyMoveDecrease()
{
	OnMovesDecreased.Broadcast(1);
}