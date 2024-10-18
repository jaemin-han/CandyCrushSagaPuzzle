// Fill out your copyright notice in the Description page of Project Settings.


#include "GameWidgetObserver.h"

void UGameWidgetObserver::OnNotify(int32 UpdateScore)
{
	CurrentScore = UpdateScore;

	UpdateScoreUI();
}
