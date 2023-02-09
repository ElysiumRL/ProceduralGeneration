// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"

#include "TagManager.generated.h"

#define TAGS_SETTINGS TEXT("/ProceduralGenerationTool/Settings/TagsSettings.TagsSettings")

DECLARE_LOG_CATEGORY_EXTERN(LogTagManager, Log, All);

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ETaggedActorFlags : uint8
{
	None = 0,
	SingleOverallUse = 1 << 0,
	SingleRoomUse = 1 << 1,
	MustHaveInRoom = 1 << 2,
};
ENUM_CLASS_FLAGS(ETaggedActorFlags);


//Used to set default actors & flags for the object
USTRUCT(Blueprintable, BlueprintType)
struct FActorTag
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Actor")
	TSubclassOf<AActor> actor;

	UPROPERTY(EditAnywhere,Category = "Actor")
	UStaticMesh* meshBounds;
	
	UPROPERTY(VisibleAnywhere,Category = "Info")
	FVector actorBounds;

	UPROPERTY(VisibleAnywhere,Category = "Info")
	float actorVolume;

	//UPROPERTY(EditAnywhere,Category = "Info")
	//bool reload;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/ProceduralGenerationTool.ETaggedActorFlags"))
	int32 flags = 0;

	inline bool operator==(const FActorTag& a)
	{
		return flags == a.flags && actor == a.actor;
	}

	friend inline bool operator==(const FActorTag& a, const FActorTag& b)
	{
		return b.flags == a.flags && b.actor == a.actor;
	}
	
	void RecalculateBounds();
};

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
	TArray<FActorTag> actorsInTag;
	
	inline bool operator==(const FTag& a)
	{
		return tag == a.tag;
	}

	static inline bool Equals(const FTag& a, const FTag& b)
	{
		return a.tag == b.tag;
	}
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


	UE_NODISCARD static FTag GetTagFromTable(const FName& tag);

protected:
	UPROPERTY()
	TObjectPtr<UTagManagerProperties> Properties;

	UWorld* TargetWorld;

	static const int MoveSecondPointModifierID = 1;

	bool bSecondPointModifierDown = false;

	bool bMoveSecondPoint = false;
};