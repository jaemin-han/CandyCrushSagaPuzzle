// Fill out your copyright notice in the Description page of Project Settings.


#include "Match3GameMode.h"

#include "MyGameInstance.h"
#include "TileGrid.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ObserverPattern/GameWidgetObserver.h"

void AMatch3GameMode::BeginPlay()
{
	Super::BeginPlay();

	UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	// TileGrid 생성 및 game over delegate binding
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

	// 카메라 엑터 생성 및 설정
	ACameraActor* CameraActor = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass());
	if (CameraActor)
	{
		// 적절한 회전 설정
		CameraActor->SetActorRotation(FRotator(-90.0, 90.0, 0.0f));

		UCameraComponent* CameraComponent = CameraActor->GetCameraComponent();

		if (CameraComponent)
		{
			// camera mobility 설정
			CameraComponent->SetMobility(EComponentMobility::Static);
			// camera projection mode 설정
			CameraComponent->SetProjectionMode(ECameraProjectionMode::Orthographic);

			float TotalGridWidth = TileGrid->TileSize * TileGrid->NumColumns;
			float TotalGridHeight = TileGrid->TileSize * TileGrid->NumRows;
			float Padding = 100.0f;

			FVector2D ViewportSize;

			if (GEngine && GEngine->GameViewport)
			{
				GEngine->GameViewport->GetViewportSize(ViewportSize);
				UE_LOG(LogTemp, Log, TEXT("Viewport Size: %f x %f"), ViewportSize.X, ViewportSize.Y);
			}

			float ViewportAspectRatio = ViewportSize.X / ViewportSize.Y;

			CameraComponent->SetOrthoWidth(FMath::Max(TotalGridWidth, TotalGridHeight * ViewportAspectRatio) + Padding);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("CameraComponent is NULL"));

		PlayerController->SetViewTarget(CameraActor);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("CameraActor is NULL"));

	// 플레이어 메쉬 비활성화 (FVector(0, 0, 0) 에 위치해서 보기에 거슬림)
	if (APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		if (UStaticMeshComponent* StaticMeshComponent = PlayerPawn->FindComponentByClass<UStaticMeshComponent>())
			StaticMeshComponent->SetVisibility(false);
		else
			UE_LOG(LogTemp, Warning, TEXT("Static Mesh component not found on Player Pawn."));
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Player Pawn not found."));
}
