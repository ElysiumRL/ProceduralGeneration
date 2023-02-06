// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/TagManager.h"
#include "InteractiveToolManager.h"
#include "SNegativeActionButton.h"
#include "Utility/ElysiumUtilities.h"
#include "ToolBuilderUtil.h"

DEFINE_LOG_CATEGORY(LogTagManager);

void FActorTag::RecalculateBounds()
{
	UE_LOG(LogTemp, Warning, L"Test");
	if (!actor || !meshBounds)
	{
		actorBounds = FVector::ZeroVector;
		actorVolume = 0.0f;
		return;
	}

	UE_LOG(LogTemp, Warning, L"RecalculateBounds");

	if (actorBounds.Size() <= meshBounds->GetPositiveBoundsExtension().Size())
	{
		actorBounds = meshBounds->GetPositiveBoundsExtension();
		actorVolume = actorBounds.Size();
		UE_LOG(LogTemp, Warning, L"%s", *meshBounds->GetPositiveBoundsExtension().ToString());
	}
}

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
	if (Property->GetNameCPP() == "actor" || Property->GetNameCPP() == "meshBounds")
	{
		for (int i = 0; i < Properties->tags.Num(); i++)
		{
			for (int j = 0; j < Properties->tags[i].actorsInTag.Num(); j++)
			{
				Properties->tags[i].actorsInTag[j].RecalculateBounds();
			}
		}
	}
	Properties->ExportProperties();
}

FTag UTagManager::GetTagFromTable(const FName& tag)
{
	UDataTable* table = ElysiumUtilities::FindDataTableChecked(TAGS_SETTINGS);
	TArray<FTableTags*> allTags;
	table->GetAllRows("", allTags);

	if (allTags[0])
	{
		for (int i = 0; i < allTags[0]->tags.Num(); i++)
		{
			FTag tagObject = allTags[0]->tags[i];
			if (tagObject.tag == tag)
			{
				return tagObject;
			}
		}
		UE_LOG(LogTagManager, Warning, L"Tag not found");
		return FTag();
	}
	UE_LOG(LogTagManager, Warning, L"Table Row not found");

	return FTag();
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
	propertiesAsTable = ElysiumUtilities::FindDataTableChecked(TAGS_SETTINGS);
}

#pragma endregion Properties
