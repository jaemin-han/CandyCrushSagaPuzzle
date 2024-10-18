// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStateSubject.h"

#include "Observer.h"

UGameStateSubject::UGameStateSubject()
{
	PlayerScore = 0;
}

void UGameStateSubject::RegisterObserver(TScriptInterface<IObserver> Observer)
{
	Observers.Push(Observer);
}

void UGameStateSubject::UnregisterObserver(TScriptInterface<IObserver> Observer)
{
	Observers.Remove(Observer);
}

void UGameStateSubject::NotifyObservers()
{
	for (auto Observer : Observers)
	{
		if (Observer.GetObject() && Observer.GetObject()->GetClass()->ImplementsInterface(UObserver::StaticClass()))
		{
			Observer->OnNotify(PlayerScore);
		}
	}
}

void UGameStateSubject::IncreasePlayerScore(const int32 Amount)
{
	PlayerScore += Amount;
	NotifyObservers();
}
