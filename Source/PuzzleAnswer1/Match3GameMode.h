// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Match3GameMode.generated.h"

class ATileGrid;
/**
 * 
 */
UCLASS()
class PUZZLEANSWER1_API AMatch3GameMode : public AGameMode
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
	TSubclassOf<ATileGrid> TileGridClass;
};