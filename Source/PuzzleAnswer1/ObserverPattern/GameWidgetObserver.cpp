// Fill out your copyright notice in the Description page of Project Settings.


#include "GameWidgetObserver.h"

#include "PuzzleAnswer1/MyGameInstance.h"

void UGameWidgetObserver::OnNotify(EGameStateType StateType, int32 UpdateValue)
{
	switch (StateType)
	{
	case EGameStateType::PlayerScore:
		CurrentScore = UpdateValue;
		UpdateScoreUI();
		break;
	case EGameStateType::RemainingMoves:
		RemainingMoves = UpdateValue;
		UpdateRemainingMovesUI();
		break;

	default:
		break;
	}
}