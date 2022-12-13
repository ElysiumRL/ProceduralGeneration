// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

/* ------------ */
#include "Tools/PrimitiveShapeRenderer.h"
/* ------------ */

#include "SettingsExporterImporter.generated.h"

#define PRIMITIVE_RENDERING_SETTINGS TEXT("/ProceduralGenerationTool/Settings/PrimitiveRendererSettings.PrimitiveRendererSettings")
#define TAGS_SETTINGS TEXT("/ProceduralGenerationTool/Settings/TagsSettings.TagsSettings")

USTRUCT(BlueprintType, Blueprintable)
struct FPSRSettingsTable : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Box", meta = (DisplayName = "Primitive Shape"))
	EPrimitiveShapeType primitiveShape;

	UPROPERTY(EditAnywhere, Category = "Box", meta = (DisplayName = "Box Location"))
	FVector boxTransform;

	UPROPERTY(EditAnywhere, Category = "Box", meta = (DisplayName = "Box Rotation"))
	float rotation;

	UPROPERTY(EditAnywhere, Category = "Box", meta = (DisplayName = "Box Size"))
	FVector boxExtent;

	UPROPERTY(EditAnywhere, Category = "Box", meta = (DisplayName = "Box Color", HideAlphaChannel))
	FColor boxColor;

	UPROPERTY(EditAnywhere, Category = "Box", meta = (DisplayName = "Box Thickness"))
	float boxThickness;

	UPROPERTY(EditAnywhere, Category = "Box|Subdivisions", meta = (DisplayName = "Allow Subdivisions"))
	bool splitBox;

	UPROPERTY(EditAnywhere, Category = "Box|Subdivisions", meta = (DisplayName = "Rectangle Subdivisions", EditCondition = "splitBox"))
	FIntVector subdivisionCount;

	UPROPERTY(EditAnywhere, Category = "Box|Subdivisions", meta = (DisplayName = "Random Subdivision Color", EditCondition = "splitBox"))
	bool randomSubdivisionColor;

	UPROPERTY(EditAnywhere, Category = "Box|Subdivisions", meta = (DisplayName = "Subdivided Box Color", EditCondition = "!randomSubdivisionColor && splitBox", HideAlphaChannel))
	FColor subdivisionColor;

	UPROPERTY(EditAnywhere, Category = "Box|Subdivisions", meta = (DisplayName = "Subdivided Box Thickness", EditCondition = "splitBox"))
	float subdivisionThickness;
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
