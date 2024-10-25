// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TileCommandInvoker.generated.h"

class ICommand;
/**
 * 
 */
UCLASS()
class PUZZLEANSWER1_API UTileCommandInvoker : public UObject
{
	GENERATED_BODY()

private:
	TArray<ICommand*> CommandHistory;

public:
	void ExecuteCommand(ICommand* Command);

	void UndoLastCommand();
};
