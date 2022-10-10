// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProceduralGenerationToolEditorMode.h"
#include "ProceduralGenerationToolEditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "ProceduralGenerationToolEditorModeCommands.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
#include "Tools/ProceduralGenerationToolSimpleTool.h"
#include "Tools/ProceduralGenerationToolInteractiveTool.h"
#include "Tools/PrimitiveShapeRenderer.h"

// step 2: register a ToolBuilder in FProceduralGenerationToolEditorMode::Enter() below


#define LOCTEXT_NAMESPACE "ProceduralGenerationToolEditorMode"

const FEditorModeID UProceduralGenerationToolEditorMode::EM_ProceduralGenerationToolEditorModeId = TEXT("EM_ProceduralGenerationToolEditorMode");

FString UProceduralGenerationToolEditorMode::SimpleToolName = TEXT("ProceduralGenerationTool_ActorInfoTool");
FString UProceduralGenerationToolEditorMode::InteractiveToolName = TEXT("ProceduralGenerationTool_MeasureDistanceTool");
FString UProceduralGenerationToolEditorMode::BoxToolName = TEXT("ProceduralGenerationTool_BoxTool");


UProceduralGenerationToolEditorMode::UProceduralGenerationToolEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(UProceduralGenerationToolEditorMode::EM_ProceduralGenerationToolEditorModeId,
		LOCTEXT("ModeName", "ProceduralGenerationTool"),
		FSlateIcon(),
		true);
}


UProceduralGenerationToolEditorMode::~UProceduralGenerationToolEditorMode()
{
}


void UProceduralGenerationToolEditorMode::ActorSelectionChangeNotify()
{
}

void UProceduralGenerationToolEditorMode::Enter()
{
	UEdMode::Enter();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FProceduralGenerationToolEditorModeCommands& SampleToolCommands = FProceduralGenerationToolEditorModeCommands::Get();

	RegisterTool(SampleToolCommands.SimpleTool, SimpleToolName, NewObject<UProceduralGenerationToolSimpleToolBuilder>(this));
	RegisterTool(SampleToolCommands.InteractiveTool, InteractiveToolName, NewObject<UProceduralGenerationToolInteractiveToolBuilder>(this));
	RegisterTool(SampleToolCommands.PrimitiveShapeRenderer, BoxToolName, NewObject<UPrimitiveShapeRendererToolBuilder>(this));
	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, SimpleToolName);
}

void UProceduralGenerationToolEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FProceduralGenerationToolEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UProceduralGenerationToolEditorMode::GetModeCommands() const
{
	return FProceduralGenerationToolEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
