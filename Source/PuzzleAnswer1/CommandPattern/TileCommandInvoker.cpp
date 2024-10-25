// Fill out your copyright notice in the Description page of Project Settings.


#include "TileCommandInvoker.h"

#include "Command.h"

void UTileCommandInvoker::ExecuteCommand(ICommand* Command)
{
	Command->Execute();
	CommandHistory.Push(Command);
}

void UTileCommandInvoker::UndoLastCommand()
{
	if (!CommandHistory.IsEmpty())
	{
		ICommand* LastCommand = CommandHistory.Last();
		CommandHistory.Pop();
		LastCommand->Undo();
	}
}
