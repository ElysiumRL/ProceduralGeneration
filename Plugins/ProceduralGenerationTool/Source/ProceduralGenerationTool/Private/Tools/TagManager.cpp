// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/TagManager.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"

#include "SettingsExporterImporter.h"

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
		UE_LOG(LogShapeRenderer, Warning, TEXT("Using Default properties as backup"));
		DefaultProperties();
	}
}


void UTagManagerProperties::ExportProperties()
{
	//TODO: add data table
	FPSRSettingsTable table;
	/*
	table.boxColor = boxColor;
	table.boxExtent = boxExtent;
	table.boxThickness = boxThickness;
	table.boxTransform = boxTransform;
	table.primitiveShape = primitiveShape;
	table.rotation = rotation;
	table.splitBox = splitBox;
	table.randomSubdivisionColor = randomSubdivisionColor;
	table.subdivisionColor = subdivisionColor;
	table.subdivisionCount = subdivisionCount;
	table.subdivisionThickness = subdivisionThickness;
	*/
	propertiesAsTable->AddRow(TEXT("Settings"), table);
	propertiesAsTable->MarkPackageDirty();

}

bool UTagManagerProperties::ImportProperties()
{
	TArray<FName> rowNames = propertiesAsTable->GetRowNames();

	for (int i = 0; i < rowNames.Num(); i++)
	{
		//TODO: AddTable
		FPSRSettingsTable* table = propertiesAsTable->FindRow<FPSRSettingsTable>(rowNames[i], "");

		if (table == nullptr)
		{
			continue;
		}
		/*
		boxColor = table->boxColor;
		boxExtent = table->boxExtent;
		boxThickness = table->boxThickness;
		boxTransform = table->boxTransform;
		primitiveShape = table->primitiveShape;
		rotation = table->rotation;
		splitBox = table->splitBox;
		randomSubdivisionColor = table->randomSubdivisionColor;
		subdivisionColor = table->subdivisionColor;
		subdivisionCount = table->subdivisionCount;
		subdivisionThickness = table->subdivisionThickness;
		renderBox = true;

		if (splitBox)
		{
			renderSubdividedBoxes = true;
		}
		*/
		UE_LOG(LogShapeRenderer, Display, TEXT("Properties Loaded"));
		return true;
	}
	UE_LOG(LogShapeRenderer, Warning, TEXT("Properties Not Found"));
	return false;
}

void UTagManagerProperties::DefaultProperties()
{
}

void UTagManagerProperties::InitializeDataTable()
{
	UDataTable* DT;
	FSoftObjectPath UnitDataTablePath = FSoftObjectPath(PRIMITIVE_RENDERING_SETTINGS);
	DT = Cast<UDataTable>(UnitDataTablePath.ResolveObject());
	if (DT)
	{
		propertiesAsTable = DT;
		UE_LOG(LogShapeRenderer, Display, TEXT("Asset Loaded"));
		return;
	}
	else
	{
		DT = Cast<UDataTable>(UnitDataTablePath.TryLoad());
	}

	if (DT)
	{
		propertiesAsTable = DT;
		UE_LOG(LogShapeRenderer, Display, TEXT("Asset Loaded"));
		return;
	}
	else
	{
		DT = NewObject<UDataTable>();
		UE_LOG(LogShapeRenderer, Warning, TEXT("Property Data Table not found !"));
	}

}

#pragma endregion Properties