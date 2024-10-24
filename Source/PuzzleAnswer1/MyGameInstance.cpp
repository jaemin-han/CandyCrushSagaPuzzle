// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include "ObserverPattern/Observer.h"

UMyGameInstance::UMyGameInstance()
{
	PlayerScore = 0;
	RemainingMoves = 10;
}

void UMyGameInstance::RegisterObserver(TScriptInterface<IObserver> Observer)
{
	Observers.Add(Observer);
}

void UMyGameInstance::UnregisterObserver(TScriptInterface<IObserver> Observer)
{
	Observers.Remove(Observer);
}

void UMyGameInstance::NotifyObservers(EGameStateType StateType, int32 Value)
{
	for (const auto& Observer : Observers)
	{
		if (Observer.GetObject() && Observer.GetObject()->GetClass()->ImplementsInterface(UObserver::StaticClass()))
		{
			Observer->OnNotify(StateType, Value); 
		}
	}
}

void UMyGameInstance::IncreasePlayerScore(int32 Amount)
{
	PlayerScore += Amount;
	NotifyObservers(EGameStateType::PlayerScore, PlayerScore);
}

void UMyGameInstance::DecreaseRemainingMoves(int32 Amount)
{
	RemainingMoves -= Amount;
	NotifyObservers(EGameStateType::RemainingMoves, RemainingMoves);
	
	if (RemainingMoves <= 0)
		OnGameOver.Broadcast();
}

void UMyGameInstance::ResetGameState()
{
	PlayerScore = 0;
	RemainingMoves = 30;

	// Widget 에 처음에 반영하기 위해서 수행함
	IncreasePlayerScore(0);
	DecreaseRemainingMoves(0);
}
