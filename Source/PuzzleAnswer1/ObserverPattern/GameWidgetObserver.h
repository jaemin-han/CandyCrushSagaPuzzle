// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Observer.h"
#include "Blueprint/UserWidget.h"
#include "GameWidgetObserver.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEANSWER1_API UGameWidgetObserver : public UUserWidget, public IObserver
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer", meta = (AllowPrivateAccess = "true"))
	int32 CurrentScore;

public:
	virtual void OnNotify(int32 UpdateScore) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateScoreUI();
};
