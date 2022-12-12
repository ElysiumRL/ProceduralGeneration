// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProceduralGenerationToolModule.h"
#include "ProceduralGenerationToolEditorModeCommands.h"
#include <PropertyEditorModule.h>
#include "TagSelectorDetails.h"
#include "SettingsExporterImporter.h"

#define LOCTEXT_NAMESPACE "ProceduralGenerationToolModule"

void FProceduralGenerationToolModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogTemp, Warning, TEXT("Procedural Tool Loaded"));
	
	FProceduralGenerationToolEditorModeCommands::Register();

	FPropertyEditorModule& propertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	propertyModule.RegisterCustomPropertyTypeLayout(FTagSelector::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FTagSelectorDetails::MakeInstance));
	
	propertyModule.NotifyCustomizationModuleChanged();
}

void FProceduralGenerationToolModule::ShutdownModule()
{
	UE_LOG(LogTemp, Warning, TEXT("Procedural Tool Unloaded"));

	FProceduralGenerationToolEditorModeCommands::Unregister();

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout(FTagSelector::StaticStruct()->GetFName());
		PropertyModule.NotifyCustomizationModuleChanged();
	}


}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FProceduralGenerationToolModule, ProceduralGenerationToolEditorMode)