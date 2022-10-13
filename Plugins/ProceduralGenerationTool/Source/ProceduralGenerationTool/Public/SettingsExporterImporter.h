// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

/* ------------ */
#include "Tools/PrimitiveShapeRenderer.h"
/* ------------ */

struct FPrimitiveShapeRendererExporter : public FTableRowBase
{
	UPROPERTY(EditAnywhere, Category = "Box", meta = (DisplayName = "Primitive Shape"))
	EPrimitiveShapeType primitiveShape;

	UPROPERTY(EditAnywhere, Category = "Box", meta = (DisplayName = "Render Box"))
	bool renderBox;

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

	UPROPERTY(EditAnywhere, Category = "Box|Subdivisions", meta = (DisplayName = "Render Box Subdivisions", EditCondition = "splitBox"))
	bool renderSubdividedBoxes;

	UPROPERTY(EditAnywhere, Category = "Box|Subdivisions", meta = (DisplayName = "Rectangle Subdivisions", EditCondition = "splitBox"))
	FIntVector subdivisionCount;

	UPROPERTY(EditAnywhere, Category = "Box|Subdivisions", meta = (DisplayName = "Random Subdivision Color", EditCondition = "splitBox"))
	bool randomSubdivisionColor;

	UPROPERTY(EditAnywhere, Category = "Box|Subdivisions", meta = (DisplayName = "Reset Subdivision Color", EditCondition = "splitBox && randomSubdivisionColor"))
	bool modifySubdivisionColor;

	UPROPERTY(EditAnywhere, Category = "Box|Subdivisions", meta = (DisplayName = "Subdivided Box Color", EditCondition = "!randomSubdivisionColor && splitBox", HideAlphaChannel))
	FColor subdivisionColor;

	UPROPERTY(EditAnywhere, Category = "Box|Subdivisions", meta = (DisplayName = "Subdivided Box Thickness", EditCondition = "splitBox"))
	float subdivisionThickness;

	UPROPERTY(EditAnywhere, Category = "Box", meta = (DisplayName = "Force reload"))
	bool reload;

};


struct PROCEDURALGENERATIONTOOL_API FSettingsExporterImporter
{
public:

	static void ExportToCSV(const TCHAR* path,void* object);

	static void* ImportFromCSV(const TCHAR* path);

};
