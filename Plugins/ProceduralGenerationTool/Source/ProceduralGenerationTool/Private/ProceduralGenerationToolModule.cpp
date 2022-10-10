// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProceduralGenerationToolModule.h"
#include "ProceduralGenerationToolEditorModeCommands.h"

#define LOCTEXT_NAMESPACE "ProceduralGenerationToolModule"

void FProceduralGenerationToolModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogTemp, Warning, TEXT("Procedural Tool Loaded"));
	
	FProceduralGenerationToolEditorModeCommands::Register();
}

void FProceduralGenerationToolModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogTemp, Warning, TEXT("Procedural Tool Unloaded"));

	FProceduralGenerationToolEditorModeCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FProceduralGenerationToolModule, ProceduralGenerationToolEditorMode)