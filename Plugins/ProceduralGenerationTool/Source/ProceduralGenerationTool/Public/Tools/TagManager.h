// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"

#include "TagManager.generated.h"

#define TAGS_SETTINGS TEXT("/ProceduralGenerationTool/Settings/TagsSettings.TagsSettings")

DECLARE_LOG_CATEGORY_EXTERN(LogTagManager, Log, All);

//Used to register all the tags in the settings
USTRUCT(Blueprintable, BlueprintType)
struct FTag
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Tag")
		FName tag;

	UPROPERTY(EditAnywhere, Category = "Tag")
		FString description;

	UPROPERTY(EditAnywhere, Category = "Tag")
		TArray<TSubclassOf<AActor>> actorsInTag;

};

//Used to select multiple tags from the FTag table
USTRUCT(Blueprintable, BlueprintType)
struct FTagSelector
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Tag")
		FName tag;
};


USTRUCT(BlueprintType, Blueprintable)
struct FTableTags : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "General")
		TArray<FTag> tags;

};

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