// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "PrimitiveShapeRenderer.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogShapeRenderer, Log, All);


UENUM(Blueprintable,BlueprintType)
enum class EPrimitiveShapeType : uint8
{
	RectangleFilled = 0,
	RectangleHollow = 1
};

UCLASS()
class PROCEDURALGENERATIONTOOL_API UPrimitiveShapeRendererToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()


public:

	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }

	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;

};

UCLASS(Transient)
class PROCEDURALGENERATIONTOOL_API UPrimitiveShapeRendererProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()


public:
	UPrimitiveShapeRendererProperties();
	
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

	void ExportProperties();

	bool ImportProperties();

	void DefaultProperties();

	void InitializeDataTable();

private:

	UDataTable* propertiesAsTable;

};

//FBox wrapper
class PROCEDURALGENERATIONTOOL_API UEnhancedBox
{
public:

	UEnhancedBox() = default;

	UEnhancedBox(const FVector& origin, const FVector& extent, float _rotation = 0.0f, const FIntVector& _relativeLocation = FIntVector::ZeroValue);
	
	UEnhancedBox(const FVector& origin, const FVector& extent, float _rotation, const FIntVector& _relativeLocation, const UEnhancedBox& centralBox);

	FBox box;

	TArray<FVector> vertices;

	FVector origin;

	FVector extent;

	FIntVector relativeLocation;

	float rotation;

	//Generates all the vertices of the Box
	void GenerateVertices(const UEnhancedBox& _centralBox);
	
	void GenerateVertices();

	void DrawBox(IToolsContextRenderAPI* RenderAPI, const FColor& color = FColor::Red, float thickness = 2.f);
	
	void DrawLine(IToolsContextRenderAPI* RenderAPI, const FVector& start, const FVector& end, const FColor& color = FColor::Red, float thickness = 2.f);

	FVector RotateBox(const FVector& boxOrigin, FVector fromLocation, float angle);

};



/**
 * Primitive Shape Renderer
**/
UCLASS()
class PROCEDURALGENERATIONTOOL_API UPrimitiveShapeRenderer : public USingleClickTool
{
	GENERATED_BODY()


public:
	UPrimitiveShapeRenderer();

	virtual void SetWorld(UWorld* World);

	virtual void Setup() override;

	virtual void OnClicked(const FInputDeviceRay& ClickPos);

	virtual void Render(IToolsContextRenderAPI* RenderAPI) override;

	virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;

protected:
	UPROPERTY()
	TObjectPtr<UPrimitiveShapeRendererProperties> Properties;


protected:
	UWorld* TargetWorld;

	static const int MoveSecondPointModifierID = 1;
	
	bool bSecondPointModifierDown = false;
	
	bool bMoveSecondPoint = false;

	UEnhancedBox centralBox;
	
	TArray<UEnhancedBox> subdivisionBoxes;

	TArray<FColor> randomSubdivisionBoxColor;

	FInputRayHit FindRayHit(const FRay& WorldRay, FVector& HitPos);
	
	void SelectUpdateMethod();

	void UpdateBoundingBox();
	
	void UpdateBoxSubdivisions();
	
	void SetRandomSubdivisionColors();

	FVector RotateBox(FVector boxOrigin, FVector fromLocation, float angle);

	void UpdateTool();
	
	TArray<FVector> GetAllBoxVertices(FBox box, FBox _centralBox);
};