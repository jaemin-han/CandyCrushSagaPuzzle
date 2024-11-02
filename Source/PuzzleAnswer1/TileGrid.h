// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileGrid.generated.h"

enum class ETileType: uint8;
class ATile;

// Delegate 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreIncreased, int32, ScoreAmount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovesDecreased, int32, MovesAmount);

DECLARE_DELEGATE(FOnGameOver);

/*
 *	TFilePair: 타일의 쌍을 저장하는 구조체.
 *	항상 주소가 작은 타일을 First에, 큰 타일을 Second 에 저장
 */
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

/** 
 * ETileGridState: 타일 그리드의 상태를 나타내는 enum. 
 * 게임의 흐름을 관리하는 데 사용됩니다.
 */
UENUM(BlueprintType)
enum class EGameState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	SwapCheck UMETA(DisplayName = "SwapCheck"),
	CheckRepeat UMETA(DisplayName = "CheckRepeat"),
	RemoveDropCreate UMETA(DisplayName = "RemoveDropCreate"),
	CheckValidPairs UMETA(DisplayName = "CheckValidPairs"),
	GameOver UMETA(DisplayName = "GameOver"),
};

/**
 * ATileGrid: 타일 그리드를 관리하는 클래스. 
 * 타일 생성, 상태 전환, 타일 이동 및 점수 계산을 담당합니다.
 */
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

private:
	// 타일을 구분하기 위한 material instance
	// 에디터에서 블루프린트로 할당함
	UPROPERTY(EditAnywhere, Category = "Tile Grid")
	TArray<UMaterialInstance*> Materials;

	// 생성할 Tile 의 블루프린트 클래스
	UPROPERTY(EditAnywhere, Category = "Tile Grid")
	TSubclassOf<ATile> TileClass;

	//	게임 내 타일의 실제 데이터를 담고 관리하는 배열
	UPROPERTY(EditAnywhere, Category = "Tile Grid")
	TArray<ATile*> TileArray;


	// 이 객체로 현재 State 를 추적함
	// Tick 에서 State 에 맞는 동작 수행
	UPROPERTY(VisibleAnywhere, Category = "State")
	EGameState CurrentState;

	// Transition Grid State Function
	UFUNCTION(BlueprintCallable, Category = "State")
	void TransitionToState(EGameState NewState);

	// grid initialize
	void InitializeGrid();

	// TileArray 에서 해당 Row, Col 에 따른 ATile* 를 받아온다
	ATile* GetTileAt(int32 Row, int32 Col) const;

	// TileArray 에 해당 Row,  Col 에 ATile* (혹은 nullptr) 을 배치한다
	void SetTileAt(int32 Row, int32 Col, ATile* Tile);

	// Tile 을 스폰하고, 랜덤하게 Material 을 결정하며  Row, Col 에 따른 위치를 설정한다
	ATile* SpawnAndInitializeTile(int32 Row, int32 Col);
	// Row 행 Col 열 타일이 world 상에서 어디에 위치할지를 리턴한다
	FVector GetTileLocationOfGrid(int32 Row, int32 Col) const;

	// 전체 TileArray 를 순회하여 연속된 타입이 얼마나 존재하는지 확인한다.
	// NumOfRepeatedTiles 에는 반복된 타일이 길이별로 얼마나 존재하는지
	// RepeatedTiles 에는 반복되는 타일들이 포함된다
	void CheckRepeatedTiles(TArray<int32>& NumOfRepeatedTiles, TSet<ATile*>& RepeatedTiles);

	UPROPERTY(VisibleAnywhere, Category = "Tile Grid")
	TArray<int32> NumOfRepeatedTilesArray;
	UPROPERTY(VisibleAnywhere, Category = "Tile Grid")
	TSet<ATile*> RepeatedTilesSet;

	// RepeatedTilesSet 에 포함되는 타일들을 제거한다
	void RemoveRepeatedTiles();
	// 삭제된 타일 위치에 위쪽에 있던 타일을 이동시킨다
	void MoveTilesDown();
	// 비어있는 타일에 새로운 타일을 생성한다
	void GenerateNewTiles();

	// 현재 움직이고 있는 타일 수를 세는 객체
	// Tile 과 delegate 로 연결되어 있다.
	FThreadSafeCounter MovingTilesCounter;
	// Tile 의 delegate 와 binding 될 두 함수
	UFUNCTION()
	void HandleTileStopMove();
	UFUNCTION()
	void HandleTileStartMove();

	// 교환하여 3 개의 연속된 타일을 만들 수 있는 타일의 쌍을 가지는 Set
	UPROPERTY(VisibleAnywhere, Category = "Tile Grid")
	TSet<FTilePair> ValidTilePairs;
	// ValidTilePairs 의 위치를 시각적으로 보여주는 debug 함수
	void DebugValidTilePairs();

	// get valid tile pairs in XOOX, OXO case
	// XOOX: 두 개의 같은 종류의 타일이 연속될떄, 양쪽 타일에서 가능성을 검사
	// OXO: 세 개의 연속된 타일 중 양쪽 타일의 종류가 같을 때, 중간 타일의 가능성을 검사
	void SetValidTilePairs();
	// SetValidTilePairs 내부에서 사용할 함수들, 하나의 방향씩을 담당한다.
	// 각각 상, 하, 좌, 우 방향으로 swap 했을 때 연속된 타일이 생기는지 검사 
	void CheckLeftTile(int32 Row, int32 Col, ETileType TileType);
	void CheckRightTile(int32 Row, int32 Col, ETileType TileType);
	void CheckUpTile(int32 Row, int32 Col, ETileType TileType);
	void CheckDownTile(int32 Row, int32 Col, ETileType TileType);

	// 타일에서 Onclicked 이벤트가 발생했을 때, FirstClickedTile, SecondClickedTile 에 적절
	// 하게 할당하고, 해당 타일에 highlight 효과를 적용한다
	UFUNCTION()
	void HandleOnTileClicked(ATile* ClickedTile);

	UPROPERTY(VisibleAnywhere, Category = "Select")
	ATile* FirstClickedTile;
	UPROPERTY(VisibleAnywhere, Category = "Select")
	ATile* SecondClickedTile;

	// Tile Swap Command 를 실행할 Invoker 객체
	UPROPERTY(VisibleAnywhere, Category = "Command")
	class UTileCommandInvoker* Invoker;


	// 두 타일의 Target Location 을 swap 한다.
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

public:
	// grid width, height
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Grid")
	int32 NumColumns;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Grid")
	int32 NumRows;
	// tile 의 cube mesh 를 뭘 고르느냐에 따라 일일이 수정해야 합니다
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tile Grid")
	float TileSize = 100.0f;
	FOnGameOver OnGameOver;
};
