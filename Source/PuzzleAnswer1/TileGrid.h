// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tile.h"
#include "GameFramework/Actor.h"
#include "TileGrid.generated.h"

class ATile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreIncreased, int32, ScoreAmount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovesDecreased, int32, MovesAmount);

DECLARE_DELEGATE(FOnGameOver);

USTRUCT(BlueprintType)
struct FTilePair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATile* First;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATile* Second;

	// 생성자: 항상 작은 주소를 First에, 큰 주소를 Second에 저장
	FTilePair(ATile* A = nullptr, ATile* B = nullptr)
	{
		if (A < B)
		{
			First = A;
			Second = B;
		}
		else
		{
			First = B;
			Second = A;
		}
	}

	// 연산자 오버로딩: 같은 쌍인지 비교
	bool operator==(const FTilePair& Other) const
	{
		return First == Other.First && Second == Other.Second;
	}

	// 해시 함수 정의 (TSet에서 사용됨)
	friend uint32 GetTypeHash(const FTilePair& Pair)
	{
		return HashCombine(GetTypeHash(Pair.First), GetTypeHash(Pair.Second));
	}
};

UENUM(BlueprintType)
enum class ETileGridState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	SwapCheck UMETA(DisplayName = "Swap"),
	CheckingForRepeated UMETA(DisplayName = "Checking For Repeated"),
	RemovingMatches UMETA(DisplayName = "Removing Matches"),
	CheckAllTilesToMoveAndDropTiles UMETA(DisplayName = "Generating New Tiles"),
	WaitUntilAllTilesStopMoving UMETA(DisplayName = "Wait Until All Tiles Stop Moving"),
	CheckingForPossibleTiles UMETA(DisplayName = "Checking For Possible Tiles"),
	GameOver UMETA(DisplayName = "Game Over"),
};

UCLASS()
class PUZZLEANSWER1_API ATileGrid : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATileGrid();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	// grid width, height
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Grid")
	int32 NumColumns;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Grid")
	int32 NumRows;
	// tile 의 cube mesh 를 뭘 고르느냐에 따라 일일이 수정해야 합니다
	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "Tile Grid")
	float TileSize = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Grid")
	TArray<UMaterialInstance*> Materials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Grid")
	TSubclassOf<ATile> TileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Grid")
	TArray<ATile*> TileArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	ETileGridState CurrentState;
	// Transition Grid State Function
	UFUNCTION(BlueprintCallable, Category = "State")
	void TransitionToState(ETileGridState NewState);
	// Tick 에서 CurrentState 체크

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

	void RemoveRepeatedTiles();
	void MoveTilesDown();
	void GenerateNewTiles();

	FThreadSafeCounter MovingTilesCounter;
	UFUNCTION()
	void OnTileStoppedMoving();
	UFUNCTION()
	void OnTileStartedMoving();

	UPROPERTY(editAnywhere, BlueprintReadWrite, Category = "Tile Grid")
	TSet<FTilePair> ValidTilePairs;
	void DebugValidTilePairs();

	// get valid tile pairs in XOOX, OXO case
	void SetValidTilePairs();
	// SetValidTilePairs 내부에서 사용할 함수들, 하나의 방향씩을 담당한다.
	void CheckLeftTile(int32 Row, int32 Col, FName TileType);
	void CheckRightTile(int32 Row, int32 Col, FName TileType);
	void CheckUpTile(int32 Row, int32 Col, FName TileType);
	void CheckDownTile(int32 Row, int32 Col, FName TileType);

	UFUNCTION()
	void HandleOnTileClicked(ATile* ClickedTile);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Command")
	class UTileCommandInvoker* Invoker;

	UPROPERTY()
	ATile* FirstClickedTile;
	UPROPERTY()
	ATile* SecondClickedTile;

	void SwapClickedTileOnTileArray(ATile* FirstTile, ATile* SecondTile);

	// Delegate 선언
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnScoreIncreased OnScoreIncreased;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnMovesDecreased OnMovesDecreased;

	// 타일 제거 후 호출할 함수 (점수 증가)
	void CalculateAndBroadcastScore();

	// 움직임 감소 함수 호출
	void NotifyMoveDecrease();

	bool bGameOverPending;

	FOnGameOver OnGameOver;
};
