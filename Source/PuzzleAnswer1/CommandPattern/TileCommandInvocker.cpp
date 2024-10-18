// Fill out your copyright notice in the Description page of Project Settings.


#include "TileCommandInvocker.h"

#include "Command.h"

void UTileCommandInvocker::ExecuteCommand(ICommand* Command)
{
	Command->Execute();
	CommandHistory.Push(Command);
}

void UTileCommandInvocker::UndoLastCommand()
{
	if (!CommandHistory.IsEmpty())
	{
		ICommand* LastCommand = CommandHistory.Last();
		CommandHistory.Pop();
		LastCommand->Undo();
	}
}
