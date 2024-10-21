// Fill out your copyright notice in the Description page of Project Settings.


#include "Match3GameMode.h"

#include "MyGameInstance.h"
#include "TileGrid.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "ObserverPattern/GameWidgetObserver.h"

void AMatch3GameMode::BeginPlay()
{
	Super::BeginPlay();

	UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	ATileGrid* TileGrid = GetWorld()->SpawnActor<ATileGrid>(TileGridClass);
	if (MyGameInstance)
	{
		MyGameInstance->ResetGameState();

		if (TileGrid)
		{
			TileGrid->OnGameOver.BindLambda([this]()
			{
				UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
				if (GameOverWidget)
				{
					GameOverWidget->AddToViewport();
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("GameOverWidget is NULL"));
				}
				UGameplayStatics::SetGamePaused(GetWorld(), true);
			});
		}
	}


	// auto* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	auto* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->SetShowMouseCursor(true);
	PlayerController->bEnableClickEvents = true;
	PlayerController->SetInputMode(FInputModeGameAndUI());
	EnableInput(PlayerController);

	if (GameWidgetObserverClass)
	{
		UGameWidgetObserver* GameWidgetObserver = CreateWidget<UGameWidgetObserver>(GetWorld(),
			GameWidgetObserverClass);

		if (GameWidgetObserver)
		{
			GameWidgetObserver->AddToViewport();
			MyGameInstance->RegisterObserver(GameWidgetObserver);

			MyGameInstance->ResetGameState();
		}
	}
}
