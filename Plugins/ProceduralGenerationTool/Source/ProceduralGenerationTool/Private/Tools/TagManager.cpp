// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/TagManager.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"

DEFINE_LOG_CATEGORY(LogTagManager);

UInteractiveTool* UTagManagerToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UTagManager* NewTool = NewObject<UTagManager>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

#pragma region TagManager

UTagManager::UTagManager()
{
}

void UTagManager::SetWorld(UWorld* World)
{
	this->TargetWorld = World;
}

void UTagManager::Setup()
{
	USingleClickTool::Setup();

	Properties = NewObject<UTagManagerProperties>(this);
	Properties->tool = this;
	AddToolPropertySource(Properties);

}

void UTagManager::OnClicked(const FInputDeviceRay& ClickPos)
{
	// Do Nothing
}

void UTagManager::Render(IToolsContextRenderAPI* RenderAPI)
{
	// Do Nothing
}

void UTagManager::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	Properties->ExportProperties();
}

#pragma endregion TagManager

#pragma region Properties

UTagManagerProperties::UTagManagerProperties()
{
	InitializeDataTable();
	bool propertiesLoaded = ImportProperties();
	if (!propertiesLoaded)
	{
		UE_LOG(LogTagManager, Warning, TEXT("Using Default properties as backup"));
		DefaultProperties();
	}
}


void UTagManagerProperties::ExportProperties()
{
	FTableTags table;
	table.tags = tags;

	propertiesAsTable->AddRow(TEXT("Settings"), table);
	propertiesAsTable->MarkPackageDirty();

}

bool UTagManagerProperties::ImportProperties()
{
	TArray<FName> rowNames = propertiesAsTable->GetRowNames();

	for (int i = 0; i < rowNames.Num(); i++)
	{
		//TODO: AddTable
		FTableTags* table = propertiesAsTable->FindRow<FTableTags>(rowNames[i], "");

		if (table == nullptr)
		{
			continue;
		}
		tags = table->tags;

		UE_LOG(LogTagManager, Display, TEXT("Properties Loaded"));
		return true;
	}
	UE_LOG(LogTagManager, Warning, TEXT("Properties Not Found"));
	return false;
}

void UTagManagerProperties::DefaultProperties()
{
}

void UTagManagerProperties::InitializeDataTable()
{
	UDataTable* DT;
	FSoftObjectPath UnitDataTablePath = FSoftObjectPath(TAGS_SETTINGS);
	DT = Cast<UDataTable>(UnitDataTablePath.ResolveObject());
	if (DT)
	{
		propertiesAsTable = DT;
		UE_LOG(LogTagManager, Display, TEXT("Asset Loaded"));
		return;
	}
	else
	{
		DT = Cast<UDataTable>(UnitDataTablePath.TryLoad());
	}

	if (DT)
	{
		propertiesAsTable = DT;
		UE_LOG(LogTagManager, Display, TEXT("Asset Loaded"));
		return;
	}
	else
	{
		DT = NewObject<UDataTable>();
		UE_LOG(LogTagManager, Warning, TEXT("Property Data Table not found !"));
	}

}

#pragma endregion Properties