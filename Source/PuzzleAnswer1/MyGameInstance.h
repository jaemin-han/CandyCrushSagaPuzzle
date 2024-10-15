// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEANSWER1_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	UMyGameInstance();

public:

	// 게임에 전체적으로 필요한 변수들
	// player score
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Data")
	int32 PlayerScore;
	
	// remaining maching number
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Data")
	int32 RemainingMoves;

	// 점수 증가 method
	UFUNCTION(BlueprintCallable, Category = "Game Function")
	void AddScore(int32 Point);

	// RemainingMoves 제어
	UFUNCTION(BlueprintCallable, Category = "Game Function")
	void DecreaseMoves();

	// game state initialize - Retry
	UFUNCTION(BlueprintCallable, Category = "Game Function")
	void ResetGameState();
	
};
