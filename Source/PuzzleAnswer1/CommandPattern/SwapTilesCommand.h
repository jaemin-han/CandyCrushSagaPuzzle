// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Command.h"

class ATile;
/**
 * 
 */
class PUZZLEANSWER1_API SwapTilesCommand: public ICommand
{
public:
	SwapTilesCommand();
	~SwapTilesCommand();

private:
	ATile* FirstTile;
	ATile* SecondTile;

public:
	virtual void Execute() override;
	virtual void Undo() override;

	SwapTilesCommand(ATile* FirstTile, ATile* SecondTile);
};
