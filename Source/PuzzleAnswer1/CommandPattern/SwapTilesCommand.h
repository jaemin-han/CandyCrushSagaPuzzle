// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command.h"
#include "SwapTilesCommand.generated.h"

class ATile;
/**
 * 
 */
UCLASS()
class PUZZLEANSWER1_API USwapTilesCommand : public UObject, public ICommand
{
	GENERATED_BODY()

	ATile* FirstTile;
	ATile* SecondTile;

public:
	virtual void Execute() override;
	virtual void Undo() override;

	void InitializeTiles(ATile* InFirstTile, ATile* InSecondTile);
};
