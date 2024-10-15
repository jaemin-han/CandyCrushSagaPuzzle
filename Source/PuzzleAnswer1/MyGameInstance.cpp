// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

UMyGameInstance::UMyGameInstance()
{
	PlayerScore = 0;
	RemainingMoves = 30.0f;
}

void UMyGameInstance::AddScore(int32 Point)
{
	PlayerScore += Point;
}

void UMyGameInstance::DecreaseMoves()
{
	if (RemainingMoves > 0)
	{
		RemainingMoves--;
	}
}

void UMyGameInstance::ResetGameState()
{
	PlayerScore = 0;
	RemainingMoves = 30.0f;
}
