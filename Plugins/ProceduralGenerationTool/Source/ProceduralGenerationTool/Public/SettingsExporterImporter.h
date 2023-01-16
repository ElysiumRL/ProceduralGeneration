// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

/* ------------ */
#include "Tools/PrimitiveShapeRenderer.h"
#include "Tools/TagManager.h"
/* ------------ */

#include "SettingsExporterImporter.generated.h"

#define PRIMITIVE_RENDERING_SETTINGS TEXT("/ProceduralGenerationTool/Settings/PrimitiveRendererSettings.PrimitiveRendererSettings")

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
	
	UPROPERTY(EditAnywhere, Category = "Misc", meta = (DisplayName = "Scene Capture 2D"))
	TObjectPtr<ASceneCapture2D> sceneCapture2D;

};