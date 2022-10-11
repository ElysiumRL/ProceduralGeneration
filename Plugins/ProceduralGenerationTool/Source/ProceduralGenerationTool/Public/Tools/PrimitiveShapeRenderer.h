// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "PrimitiveShapeRenderer.generated.h"

UENUM()
enum class EPrimitiveShapeType : uint8
{
	Rectangle = 0
};



/**
 * 
 */
UCLASS()
class PROCEDURALGENERATIONTOOL_API UPrimitiveShapeRendererToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()
public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;

};

/**
 * Settings UObject for UProceduralGenerationToolSimpleTool. This UClass inherits from UInteractiveToolPropertySet,
 * which provides an OnModified delegate that the Tool will listen to for changes in property values.
 */
UCLASS(Transient)
class PROCEDURALGENERATIONTOOL_API UPrimitiveShapeRendererProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()
public:
	UPrimitiveShapeRendererProperties();
	
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

	UPROPERTY(EditAnywhere, Category = "Box|Subdivisions", meta = (DisplayName = "Subdivision Colors", EditCondition = "!randomSubdivisionColor", HideAlphaChannel))
	FColor subdivisionColor;

	UPROPERTY(EditAnywhere, Category = "Box|Subdivisions", meta = (DisplayName = "Box Thickness"))
	float subdivisionThickness;



	UPROPERTY(EditAnywhere, Category = "Box", meta = (DisplayName = "Force reload"))
	bool reload;


};

/**
 * UProceduralGenerationToolSimpleTool is an example Tool that opens a message box displaying info about an actor that the user
 * clicks left mouse button. All the action is in the ::OnClicked handler.
 */
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

	FBox box;
	
	TArray<FBox> subdivisionBoxes;


	FInputRayHit FindRayHit(const FRay& WorldRay, FVector& HitPos);
	
	void UpdateBoundingBox();
	
	void UpdateBoxSubdivisions();

	void UpdateTool();
	
	void DrawLine(FVector start, FVector end, IToolsContextRenderAPI* RenderAPI);

	void DrawLine(FVector start, FVector end, FColor color, IToolsContextRenderAPI* RenderAPI);

	void DrawLine(FVector start, FVector end, FColor color, float thickness, IToolsContextRenderAPI* RenderAPI);
	
	void DrawBox(TArray<FVector> vertices, IToolsContextRenderAPI* RenderAPI);
	
	void DrawBox(TArray<FVector> vertices, FColor color, IToolsContextRenderAPI* RenderAPI);
	
	void DrawBox(TArray<FVector> vertices, FColor color, float thickness, IToolsContextRenderAPI* RenderAPI);

	TArray<FVector> GetAllBoxVertices(FBox box);

};