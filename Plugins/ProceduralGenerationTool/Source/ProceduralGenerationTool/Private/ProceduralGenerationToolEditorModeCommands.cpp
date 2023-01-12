// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProceduralGenerationToolEditorModeCommands.h"
#include "ProceduralGenerationToolEditorMode.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "ProceduralGenerationToolEditorModeCommands"

FProceduralGenerationToolEditorModeCommands::FProceduralGenerationToolEditorModeCommands()
	: TCommands<FProceduralGenerationToolEditorModeCommands>("ProceduralGenerationToolEditorMode",
		NSLOCTEXT("ProceduralGenerationToolEditorMode", "ProceduralGenerationToolEditorModeCommands", "ProceduralGenerationTool Editor Mode"),
		NAME_None,
		FAppStyle::GetAppStyleSetName())
{
}

void FProceduralGenerationToolEditorModeCommands::RegisterCommands()
{
	TArray <TSharedPtr<FUICommandInfo>>& ToolCommands = Commands.FindOrAdd(NAME_Default);

	//UI_COMMAND(SimpleTool, "Show Actor Info", "Opens message box with info about a clicked actor", EUserInterfaceActionType::Button, FInputChord());
	//ToolCommands.Add(SimpleTool);
	
	//UI_COMMAND(InteractiveTool, "Measure Distance", "Measures distance between 2 points (click to set origin, shift-click to set end point)", EUserInterfaceActionType::ToggleButton, FInputChord());
	//ToolCommands.Add(InteractiveTool);
	
	UI_COMMAND(PrimitiveShapeRenderer, "Generate", "Generates the procedural interior", EUserInterfaceActionType::RadioButton, FInputChord());
	UI_COMMAND(TagManager, "Tags", "Manage tags used for the auto filling of the rooms", EUserInterfaceActionType::RadioButton, FInputChord());

	ToolCommands.Add(PrimitiveShapeRenderer);
	ToolCommands.Add(TagManager);

}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FProceduralGenerationToolEditorModeCommands::GetCommands()
{
	return FProceduralGenerationToolEditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
