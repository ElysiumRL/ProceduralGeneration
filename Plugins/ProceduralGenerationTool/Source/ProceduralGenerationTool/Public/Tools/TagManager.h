// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "SettingsExporterImporter.h"

#include "TagManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTagManager, Log, All);


UCLASS()
class PROCEDURALGENERATIONTOOL_API UTagManagerToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()


public:

	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }

	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;

};

UCLASS(Transient)
class PROCEDURALGENERATIONTOOL_API UTagManagerProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()


public:

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "General")
	TArray<FTag> tags;

	UTagManagerProperties();

	void ExportProperties();

	bool ImportProperties();

	void DefaultProperties();

	void InitializeDataTable();

	UTagManager* tool;

private:

	UDataTable* propertiesAsTable;

};

UCLASS()
class PROCEDURALGENERATIONTOOL_API UTagManager : public USingleClickTool
{
	GENERATED_BODY()


public:
	UTagManager();

	virtual void SetWorld(UWorld* World);

	virtual void Setup() override;

	virtual void OnClicked(const FInputDeviceRay& ClickPos);

	virtual void Render(IToolsContextRenderAPI* RenderAPI) override;

	virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;

protected:
	UPROPERTY()
	TObjectPtr<UTagManagerProperties> Properties;

	UWorld* TargetWorld;

	static const int MoveSecondPointModifierID = 1;

	bool bSecondPointModifierDown = false;

	bool bMoveSecondPoint = false;
};