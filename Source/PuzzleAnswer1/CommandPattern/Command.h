// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Command.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UCommand : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PUZZLEANSWER1_API ICommand
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Execute() = 0;
	virtual void Undo() = 0;
};