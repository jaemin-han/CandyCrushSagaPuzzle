// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameStateSubject.generated.h"

class IObserver;
/**
 * 
 */
UCLASS()
class PUZZLEANSWER1_API UGameStateSubject : public UObject
{
	GENERATED_BODY()

private:
	TArray<TScriptInterface<IObserver>> Observers;
	int32 PlayerScore;

public:
	UGameStateSubject();

	void RegisterObserver(TScriptInterface<IObserver> Observer);

	void UnregisterObserver(TScriptInterface<IObserver> Observer);

	void NotifyObservers();

	void IncreasePlayerScore(int32 Amount);

	int32 GetPlayerScore() const {return PlayerScore;};
};
