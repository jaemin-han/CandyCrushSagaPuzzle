// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ObserverPattern/Observer.h"
#include "MyGameInstance.generated.h"

class IObserver;

DECLARE_MULTICAST_DELEGATE(FOnRemainingMovesComestoZero);

/**
 * 
 */
UCLASS()
class PUZZLEANSWER1_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	UMyGameInstance();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Observer", meta = (AllowPrivateAccess = "true"))
	TArray<TScriptInterface<IObserver>> Observers;
	// player score
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Data", meta=(AllowPrivateAccess="true"))
	int32 PlayerScore;
	// remaining maching number
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Data", meta=(AllowPrivateAccess="true"))
	int32 RemainingMoves;

public:

	void RegisterObserver(TScriptInterface<IObserver> Observer);
	void UnregisterObserver(TScriptInterface<IObserver> Observer);

	void NotifyObservers(EGameStateType StateType, int32 Value);

	UFUNCTION()
	void IncreasePlayerScore(int32 Amount);
	UFUNCTION()
	void DecreaseRemainingMoves(int32 Amount);

	int32 GetPlayerScore() const {return PlayerScore;};
	int32 GetRemainingMoves() const {return RemainingMoves;};

	// game state initialize - Retry
	UFUNCTION(BlueprintCallable, Category = "Game Function")
	void ResetGameState();

	FOnRemainingMovesComestoZero OnGameOver;

};
