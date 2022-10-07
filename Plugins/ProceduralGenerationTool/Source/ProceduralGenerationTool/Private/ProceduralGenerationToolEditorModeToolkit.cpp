// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProceduralGenerationToolEditorModeToolkit.h"
#include "ProceduralGenerationToolEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "ProceduralGenerationToolEditorModeToolkit"

FProceduralGenerationToolEditorModeToolkit::FProceduralGenerationToolEditorModeToolkit()
{
}

void FProceduralGenerationToolEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FProceduralGenerationToolEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FProceduralGenerationToolEditorModeToolkit::GetToolkitFName() const
{
	return FName("ProceduralGenerationToolEditorMode");
}

FText FProceduralGenerationToolEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "ProceduralGenerationToolEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
