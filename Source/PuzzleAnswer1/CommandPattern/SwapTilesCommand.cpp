// Fill out your copyright notice in the Description page of Project Settings.


#include "SwapTilesCommand.h"

#include "PuzzleAnswer1/Tile.h"


void USwapTilesCommand::Execute()
{
	Swap(FirstTile->TargetLocation, SecondTile->TargetLocation);
	FirstTile->StartMoving();
	SecondTile->StartMoving();
}

void USwapTilesCommand::Undo()
{
	Swap(FirstTile->TargetLocation, SecondTile->TargetLocation);
	FirstTile->StartMoving();
	SecondTile->StartMoving();
}

void USwapTilesCommand::InitializeTiles(ATile* InFirstTile, ATile* InSecondTile)
{
	FirstTile = InFirstTile;
	SecondTile = InSecondTile;
}
