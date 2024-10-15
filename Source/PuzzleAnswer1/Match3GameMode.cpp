// Fill out your copyright notice in the Description page of Project Settings.


#include "Match3GameMode.h"

#include "MyGameInstance.h"
#include "TileGrid.h"
#include "Kismet/GameplayStatics.h"

void AMatch3GameMode::BeginPlay()
{
	Super::BeginPlay();

	UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (MyGameInstance)
	{
		MyGameInstance->ResetGameState();
	}

	ATileGrid* TileGrid = GetWorld()->SpawnActor<ATileGrid>(TileGridClass);


}