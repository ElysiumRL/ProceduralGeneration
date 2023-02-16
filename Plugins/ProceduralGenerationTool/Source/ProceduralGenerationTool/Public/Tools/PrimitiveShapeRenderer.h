// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "Kismet/KismetMathLibrary.h"

#include "TagManager.h"

#include "EnhancedBox.h"
#include "DynamicMeshWall.h"
#include <Engine/SceneCapture2D.h>
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

	UPROPERTY(EditAnywhere, Category = "Misc", meta = (DisplayName = "Scene Capture 2D"))
	TObjectPtr<ASceneCapture2D> sceneCapture2D;

	void ExportProperties();

	bool ImportProperties();

	void DefaultProperties();

	void InitializeDataTable();

	UFUNCTION(CallInEditor,Category = "_ControlPanel")
	void StartGeneration();
	
	UFUNCTION(CallInEditor, Category = "Seed")
	void SetRandomSeed();
	
	UPROPERTY(EditAnywhere, Category = "Seed", meta = (DisplayName = "Seed"))
	int32 randomSeedValue;


	UPROPERTY(EditAnywhere, Category = "Demo", meta = (DisplayName = "Tag Selector Demo"))
	FTagSelector selector;
	



	UPrimitiveShapeRenderer* tool;

private:

	UDataTable* propertiesAsTable;


};

template<typename T1>
class PROCEDURALGENERATIONTOOL_API BoxWithObj
{
public:

	T1 object;

	UEnhancedBox enhancedBox;

};


UCLASS()
class PROCEDURALGENERATIONTOOL_API UBoxedRoom : public UObject
{
	GENERATED_BODY()

public:

	TPair<ADynamicMeshWall*,UEnhancedBox*> walls;
	TPair<AActor*,UEnhancedBox*> actorsInRoom;


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
	void RemoveHollowedSubdivisions();
	void BuildSubdivisions(FVector subdivExtent);

	void UpdateBoxSubdivisions();
	
	void SetRandomSubdivisionColors();

	FVector RotateBox(FVector boxOrigin, FVector fromLocation, float angle);

	void UpdateTool();
	
	TArray<FVector> GetAllBoxVertices(FBox box, FBox _centralBox);

	//////////////////////////////////////////////////////////////////////////

public:
	//BP Class of the Dynamic Wall
	UClass* dynamicWallBP;

	void StartProceduralGeneration();

	void PlaceObjectsOnWalls(ADynamicMeshWall* wall, FTagSelector tag);

};

/**
 * Subdivision type (Horizontal/Vertical)
 */
UENUM(BlueprintType)
enum class ESubdivisionType : uint8
{
	Horizontal = 0,
	Vertical = 1
};

/**
 * Utility methods for the procedural generation
 * TODO: Refactor a lot of the code
 */
class PROCEDURALGENERATIONTOOL_API GenerationUtilities
{
public:

	static FRandomStream randomSeedRNG;

	inline static ESubdivisionType RandomSubdivision() { return static_cast<ESubdivisionType>(UKismetMathLibrary::RandomIntegerFromStream(1, GenerationUtilities::randomSeedRNG)); }

	static TArray<UEnhancedBox*> results;

	static void Subdivide(UEnhancedBox* bounds, UEnhancedBox* boxToSubdivide, int iterations, ESubdivisionType subdivisionType,bool deleteSubdividedBounds = false);

	static TArray<UEnhancedBox*> Split(UEnhancedBox bounds,UEnhancedBox boxToSubdivide,float splitLocationFromAxis, ESubdivisionType subdivisionType);

	static void MergeBoxes();

	static TArray<ADynamicMeshWall*> CreateWall(UEnhancedBox box, UPrimitiveShapeRenderer* renderer);

	static void LinkBoxes(bool bAllowMultipleConnections);

	static void ExportResults();

	static void FillRoom();

};