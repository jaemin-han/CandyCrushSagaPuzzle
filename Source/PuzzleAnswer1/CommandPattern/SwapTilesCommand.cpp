// Fill out your copyright notice in the Description page of Project Settings.


#include "SwapTilesCommand.h"

#include "PuzzleAnswer1/Tile.h"


void USwapTilesCommand::Execute()
{
	const FVector TempLocation = FirstTile->GetTargetLoc();
	FirstTile->SetTargetLoc(SecondTile->GetTargetLoc());
	SecondTile->SetTargetLoc(TempLocation);
	
	FirstTile->StartMoving();
	SecondTile->StartMoving();
}

void USwapTilesCommand::Undo()
{
	const FVector TempLocation = FirstTile->GetTargetLoc();
	FirstTile->SetTargetLoc(SecondTile->GetTargetLoc());
	SecondTile->SetTargetLoc(TempLocation);
	
	FirstTile->StartMoving();
	SecondTile->StartMoving();
}

void USwapTilesCommand::InitializeTiles(ATile* InFirstTile, ATile* InSecondTile)
{
	FirstTile = InFirstTile;
	SecondTile = InSecondTile;
}
