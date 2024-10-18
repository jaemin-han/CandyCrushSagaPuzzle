// Fill out your copyright notice in the Description page of Project Settings.


#include "SwapTilesCommand.h"

#include "PuzzleAnswer1/Tile.h"


SwapTilesCommand::SwapTilesCommand()
{
}

SwapTilesCommand::~SwapTilesCommand()
{
}

void SwapTilesCommand::Execute()
{
	Swap(FirstTile->TargetLocation, SecondTile->TargetLocation);
	FirstTile->StartMoving();
	SecondTile->StartMoving();
}

void SwapTilesCommand::Undo()
{
	Swap(FirstTile->TargetLocation, SecondTile->TargetLocation);
	FirstTile->StartMoving();
	SecondTile->StartMoving();
}

SwapTilesCommand::SwapTilesCommand(ATile* FirstTile, ATile* SecondTile)
	: FirstTile(FirstTile), SecondTile(SecondTile)
{
}
