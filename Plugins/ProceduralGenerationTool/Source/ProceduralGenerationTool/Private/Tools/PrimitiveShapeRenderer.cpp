// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/PrimitiveShapeRenderer.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/ConstructorHelpers.h"

#include "SettingsExporterImporter.h"
#include <Components/SceneCaptureComponent2D.h>
#include <Engine/TextureRenderTarget2D.h>

DEFINE_LOG_CATEGORY(LogShapeRenderer);

#define BP_DYNAMIC_WALL_PATH TEXT("/ProceduralGenerationTool/GeometryScript/BP_DynamicWall")

UInteractiveTool* UPrimitiveShapeRendererToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UPrimitiveShapeRenderer* NewTool = NewObject<UPrimitiveShapeRenderer>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

#pragma region PrimitiveShapeRenderer

UPrimitiveShapeRenderer::UPrimitiveShapeRenderer()
{
	ConstructorHelpers::FClassFinder<ADynamicMeshWall> dynamicWallBPFinder(BP_DYNAMIC_WALL_PATH);
	dynamicWallBP = dynamicWallBPFinder.Class;
}

void UPrimitiveShapeRenderer::SetWorld(UWorld* World)
{
	this->TargetWorld = World;
}

void UPrimitiveShapeRenderer::Setup()
{
	USingleClickTool::Setup();

	Properties = NewObject<UPrimitiveShapeRendererProperties>(this);
	Properties->tool = this;
	AddToolPropertySource(Properties);

	centralBox.box.Init();
	subdivisionBoxes = TArray<UEnhancedBox>();
	UpdateTool();
}

void UPrimitiveShapeRenderer::OnClicked(const FInputDeviceRay& ClickPos)
{
	// Do Nothing
}

void UPrimitiveShapeRenderer::Render(IToolsContextRenderAPI* RenderAPI)
{
	if (Properties->renderBox)
	{
		centralBox.DrawBox(RenderAPI, Properties->boxColor, Properties->boxThickness);
		//DrawBox(GetAllBoxVertices(centralBox,centralBox), Properties->boxColor, Properties->boxThickness, RenderAPI);
	}
	if (Properties->splitBox && Properties->renderSubdividedBoxes)
	{
		if (Properties->randomSubdivisionColor && subdivisionBoxes.Num() != randomSubdivisionBoxColor.Num())
		{
			SetRandomSubdivisionColors();
		}
		for (int i = 0; i < subdivisionBoxes.Num(); i++)
		{
			if (Properties->randomSubdivisionColor)
			{
				subdivisionBoxes[i].DrawBox(RenderAPI, randomSubdivisionBoxColor[i], Properties->subdivisionThickness);
			}
			else
			{
				subdivisionBoxes[i].DrawBox(RenderAPI, Properties->subdivisionColor, Properties->subdivisionThickness);
			}
		}
	}

	for (int i = 0; i < GenerationUtilities::results.Num(); i++)
	{
		if (GenerationUtilities::results[i] != nullptr)
		{
			GenerationUtilities::results[i]->DrawBox(RenderAPI, GenerationUtilities::results[i]->color, 5.f);
		}
		else
		{
			UE_LOG(LogShapeRenderer, Warning, TEXT("Result is null"));
		}
	}

}

TArray<FVector> UPrimitiveShapeRenderer::GetAllBoxVertices(FBox _Box, FBox _centralBox)
{
	TArray<FVector> allPoints = TArray<FVector>();
	allPoints.Add(_Box.Min);										// 1
	allPoints.Add(FVector(_Box.Max.X, _Box.Min.Y, _Box.Min.Z));		// 2
	allPoints.Add(FVector(_Box.Max.X, _Box.Max.Y, _Box.Min.Z));		// 3
	allPoints.Add(FVector(_Box.Min.X, _Box.Max.Y, _Box.Min.Z));		// 4
	allPoints.Add(FVector(_Box.Max.X, _Box.Min.Y, _Box.Max.Z));		// 5
	allPoints.Add(FVector(_Box.Min.X, _Box.Min.Y, _Box.Max.Z));		// 6
	allPoints.Add(FVector(_Box.Min.X, _Box.Max.Y, _Box.Max.Z));		// 7
	allPoints.Add(_Box.Max);										// 8


	for (int i = 0; i < allPoints.Num(); i++)
	{
		allPoints[i] = RotateBox(_centralBox.GetCenter(), allPoints[i], Properties->rotation * UKismetMathLibrary::GetPI() / 180.f);
	}

	return allPoints;
}


void UPrimitiveShapeRenderer::StartProceduralGeneration()
{
	//Remove all existing boxes
	GenerationUtilities::results.Empty();

	GenerationUtilities::randomSeedRNG = FRandomStream(Properties->randomSeedValue);

	if (Properties->splitBox)
	{
		for (int i = 0; i < subdivisionBoxes.Num(); i++)
		{
			GenerationUtilities::Subdivide(&subdivisionBoxes[i], &subdivisionBoxes[i], 4, GenerationUtilities::RandomSubdivision());
		}
	}
	else
	{
		//Generation
		GenerationUtilities::Subdivide(&centralBox, &centralBox, 4, GenerationUtilities::RandomSubdivision());
	}

	//Logging 
	//UE_LOG(LogShapeRenderer, Warning, TEXT("Generation Done"));
	//
	//for (int i = 0; i < GenerationUtilities::results.Num(); i++)
	//{
	//	UE_LOG(LogShapeRenderer, Warning, TEXT("%s"), *GenerationUtilities::results[i].ToString());
	//}

	TArray<AActor*> walls = TArray<AActor*>();

	for (int i = 0; i < GenerationUtilities::results.Num(); i++)
	{
		walls.Append(GenerationUtilities::CreateWall(*GenerationUtilities::results[i], this));
	}


	Properties->sceneCapture2D->GetCaptureComponent2D()->ShowOnlyActors = walls;
	Properties->sceneCapture2D->GetCaptureComponent2D()->CaptureScene();
}

void UPrimitiveShapeRenderer::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	FString propertyName = Property->GetFName().ToString();
	SelectUpdateMethod();
	if (Properties->randomSubdivisionColor)
	{
		SetRandomSubdivisionColors();
	}

	if (Properties->reload)
	{
		UpdateTool();
		UE_LOG(LogTemp, Display, TEXT("Procedural Generation Tool : Tool Updated!"));
	}
	Properties->reload = false;
	Properties->modifySubdivisionColor = false;
	Properties->ExportProperties();
}

FInputRayHit UPrimitiveShapeRenderer::FindRayHit(const FRay& WorldRay, FVector& HitPos)
{
	return FInputRayHit();
}

void UPrimitiveShapeRenderer::SelectUpdateMethod()
{
	UpdateBoundingBox();

	if (Properties->splitBox)
	{
		if (Properties->primitiveShape == EPrimitiveShapeType::RectangleHollow)
		{
			UpdateBoxSubdivisions();
		}
		else
		{
			UpdateBoxSubdivisions();
		}
	}
}

void UPrimitiveShapeRenderer::UpdateBoundingBox()
{
	centralBox = UEnhancedBox(Properties->boxTransform, Properties->boxExtent, Properties->rotation, FIntVector(0, 0, 0));
}

void UPrimitiveShapeRenderer::UpdateBoxSubdivisions()
{
	subdivisionBoxes.Empty();

	if (Properties->subdivisionCount.X <= 0) { Properties->subdivisionCount.X = 1; }
	if (Properties->subdivisionCount.Y <= 0) { Properties->subdivisionCount.Y = 1; }
	if (Properties->subdivisionCount.Z <= 0) { Properties->subdivisionCount.Z = 1; }

	FVector subdivExtent = FVector(
		(float)Properties->boxExtent.X / (float)Properties->subdivisionCount.X,
		(float)Properties->boxExtent.Y / (float)Properties->subdivisionCount.Y,
		(float)Properties->boxExtent.Z / (float)Properties->subdivisionCount.Z);

	//i,j,k represents locations
	//relI,relJ,relK represents coordinates relative to the boxes
	int relI = 0;
	int relJ = 0;
	int relK = 0;
	for (float i = -Properties->boxExtent.X; i < Properties->boxExtent.X; i += UKismetMathLibrary::FCeil(2 * Properties->boxExtent.X / Properties->subdivisionCount.X))
	{
		relI++;
		for (float j = -Properties->boxExtent.Y; j < Properties->boxExtent.Y; j += UKismetMathLibrary::FCeil(2 * Properties->boxExtent.Y / Properties->subdivisionCount.Y))
		{
			relJ++;
			for (float k = -Properties->boxExtent.Z; k < Properties->boxExtent.Z; k += UKismetMathLibrary::FCeil(2 * Properties->boxExtent.Z / Properties->subdivisionCount.Z))
			{
				relK++;
				FVector subdivTransform = FVector(
					Properties->boxTransform.X - i - subdivExtent.X,
					Properties->boxTransform.Y - j - subdivExtent.Y,
					Properties->boxTransform.Z - k - subdivExtent.Z);

				UEnhancedBox generatedBox = UEnhancedBox(subdivTransform, subdivExtent, Properties->rotation, FIntVector(relI, relJ, relK), centralBox);

				subdivisionBoxes.Add(generatedBox);
			}
			relK = 0;
		}
		relJ = 0;
	}

	//Remove boxes if hollow is enabled
	if (Properties->primitiveShape == EPrimitiveShapeType::RectangleHollow)
	{
		for (int i = 0; i < subdivisionBoxes.Num(); i++)
		{
			if (
				(subdivisionBoxes[i].relativeLocation.X == 1 || subdivisionBoxes[i].relativeLocation.X == Properties->subdivisionCount.X)
				|| (subdivisionBoxes[i].relativeLocation.Y == 1 || subdivisionBoxes[i].relativeLocation.Y == Properties->subdivisionCount.Y))
			{

			}
			else
			{
				subdivisionBoxes.RemoveAt(i);
				i--;
			}
		}
	}
}

void UPrimitiveShapeRenderer::SetRandomSubdivisionColors()
{
	randomSubdivisionBoxColor.SetNum(subdivisionBoxes.Num(), true);
	for (int i = 0; i < randomSubdivisionBoxColor.Num(); i++)
	{
		randomSubdivisionBoxColor[i] = FColor::MakeRandomColor();
	}
}

FVector UPrimitiveShapeRenderer::RotateBox(FVector boxOrigin, FVector fromLocation, float angle)
{
	float s = sin(angle);
	float c = cos(angle);

	fromLocation.X -= boxOrigin.X;
	fromLocation.Y -= boxOrigin.Y;

	float xnew = fromLocation.X * c - fromLocation.Y * s;
	float ynew = fromLocation.X * s + fromLocation.Y * c;

	fromLocation.X = xnew + boxOrigin.X;
	fromLocation.Y = ynew + boxOrigin.Y;

	return FVector(fromLocation.X, fromLocation.Y, fromLocation.Z);
}

void UPrimitiveShapeRenderer::UpdateTool()
{
	UpdateBoundingBox();
	if (Properties->splitBox)
	{
		UpdateBoxSubdivisions();
		SetRandomSubdivisionColors();
	}
}

#pragma endregion PrimitiveShapeRenderer

#pragma region Properties

UPrimitiveShapeRendererProperties::UPrimitiveShapeRendererProperties()
{
	InitializeDataTable();
	bool propertiesLoaded = ImportProperties();
	if (!propertiesLoaded)
	{
		UE_LOG(LogShapeRenderer, Warning, TEXT("Using Default properties as backup"));
		DefaultProperties();
	}
}


void UPrimitiveShapeRendererProperties::ExportProperties()
{
	FPSRSettingsTable table;
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
	table.sceneCapture2D = sceneCapture2D;

	propertiesAsTable->AddRow(TEXT("Settings"), table);
	propertiesAsTable->MarkPackageDirty();

}

bool UPrimitiveShapeRendererProperties::ImportProperties()
{
	TArray<FName> rowNames = propertiesAsTable->GetRowNames();

	for (int i = 0; i < rowNames.Num(); i++)
	{
		FPSRSettingsTable* table = propertiesAsTable->FindRow<FPSRSettingsTable>(rowNames[i], "");

		if (table == nullptr)
		{
			continue;
		}

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
		sceneCapture2D = table->sceneCapture2D;
		renderBox = true;

		if (splitBox)
		{
			renderSubdividedBoxes = true;
		}

		UE_LOG(LogShapeRenderer, Display, TEXT("Properties Loaded"));
		return true;
	}
	UE_LOG(LogShapeRenderer, Warning, TEXT("Properties Not Found"));
	return false;
}

void UPrimitiveShapeRendererProperties::DefaultProperties()
{
	primitiveShape = EPrimitiveShapeType::RectangleFilled;

	renderBox = true;
	boxTransform = FVector::Zero();
	rotation = 0.0f;
	boxExtent = FVector(100.0, 100.0, 100.0);
	boxColor = FColor::Red;
	boxThickness = 2.0f;

	splitBox = false;
	renderSubdividedBoxes = true;
	subdivisionCount = FIntVector(3, 3, 3);
	randomSubdivisionColor = false;
	subdivisionColor = FColor::Blue;
	subdivisionThickness = 1.f;
}

void UPrimitiveShapeRendererProperties::InitializeDataTable()
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

#pragma region Start Generation

void UPrimitiveShapeRendererProperties::StartGeneration()
{
	if (tool == nullptr)
	{
		UE_LOG(LogShapeRenderer, Error, TEXT("Can't start Generation : Tool not found!"));
	}
	tool->StartProceduralGeneration();

}

void UPrimitiveShapeRendererProperties::SetRandomSeed()
{
	randomSeedValue = UKismetMathLibrary::RandomInteger(INT32_MAX);
}

#pragma endregion Start Generation

FRandomStream GenerationUtilities::randomSeedRNG;

TArray<UEnhancedBox*> GenerationUtilities::results = TArray<UEnhancedBox*>();

void GenerationUtilities::Subdivide(UEnhancedBox* bounds, UEnhancedBox* boxToSubdivide, int iterations, ESubdivisionType subdivisionType, bool deleteSubdividedBounds)
{
	iterations--;
	//Exit Condition
	if (iterations == 0)
	{
		GenerationUtilities::results.Add(boxToSubdivide);
		return;
	}

	//Subdivision size constraint
	int minSubdivisionWidth = 0.3f * boxToSubdivide->Width();
	int maxSubdivisionWidth = 0.7f * boxToSubdivide->Width();

	TArray<UEnhancedBox*> subdivisions = GenerationUtilities::Split(*bounds, *boxToSubdivide, UKismetMathLibrary::RandomFloatInRangeFromStream(minSubdivisionWidth, maxSubdivisionWidth, GenerationUtilities::randomSeedRNG) / bounds->Width(), subdivisionType);
	
	for (int i = 0; i < subdivisions.Num(); i++)
	{
		subdivisionType = subdivisions[i]->HeightRatio() >= 1.125f ? ESubdivisionType::Vertical : ESubdivisionType::Horizontal;
		
		GenerationUtilities::Subdivide(subdivisions[i], subdivisions[i], iterations, subdivisionType, deleteSubdividedBounds);
	}
}

TArray<UEnhancedBox*> GenerationUtilities::Split(UEnhancedBox bounds, UEnhancedBox boxToSubdivide, float splitLocationFromAxis, ESubdivisionType subdivisionType)
{
	//Debug Offset (should not be used usually)
	FVector offset = FVector(0, 0, 0);

	//Subdivided rectangles generated
	TArray<UEnhancedBox*> subdivisions;

	//Horizontal
	if (subdivisionType == ESubdivisionType::Horizontal) 
	{
		
		FVector box1Center = FVector(
			boxToSubdivide.TopRight().X + (boxToSubdivide.Width() * splitLocationFromAxis),
			boxToSubdivide.box.GetCenter().Y,
			bounds.box.GetCenter().Z) + offset;

		FVector box1Extends = FVector(
			boxToSubdivide.extent.X * splitLocationFromAxis,
			boxToSubdivide.extent.Y,
			boxToSubdivide.extent.Z);
		
		UEnhancedBox* box1 = new UEnhancedBox(box1Center, box1Extends);

		//////////////////////////////////////////////////////////////////////////

		FVector box2Center = FVector(
			boxToSubdivide.TopLeft().X + boxToSubdivide.Width() + (boxToSubdivide.Width() * splitLocationFromAxis),
			boxToSubdivide.box.GetCenter().Y,
			bounds.box.GetCenter().Z) + offset;
		
		FVector box2Extends = FVector(
			boxToSubdivide.extent.X * (1.0f - splitLocationFromAxis),
			boxToSubdivide.extent.Y,
			boxToSubdivide.extent.Z);
		
		UEnhancedBox* box2 = new UEnhancedBox(box2Center, box2Extends);
		
		
		box1->color = FColor::MakeRandomColor();
		box2->color = FColor::MakeRandomColor();
		
		subdivisions.Add(box1);	
		subdivisions.Add(box2);

	}
	else //Vertical
	{
		FVector box1Center = FVector(
			boxToSubdivide.box.GetCenter().X,
			boxToSubdivide.TopRight().Y - splitLocationFromAxis * (boxToSubdivide.Length()),
			bounds.box.GetCenter().Z) + offset;
		

		FVector box1Extends = FVector(
			boxToSubdivide.extent.X,
			boxToSubdivide.extent.Y * splitLocationFromAxis,
			boxToSubdivide.extent.Z);
		

		UEnhancedBox* box1 = new UEnhancedBox(box1Center, box1Extends);

		//////////////////////////////////////////////////////////////////////////


		FVector box2Center = FVector(
			boxToSubdivide.box.GetCenter().X,
			boxToSubdivide.TopLeft().Y + boxToSubdivide.Length() - (boxToSubdivide.Length() * splitLocationFromAxis),
			bounds.box.GetCenter().Z) + offset;
		

		FVector box2Extends = FVector(
			boxToSubdivide.extent.X,
			boxToSubdivide.extent.Y * (1.0f - splitLocationFromAxis),
			boxToSubdivide.extent.Z);

		UEnhancedBox* box2 = new UEnhancedBox(box2Center, box2Extends);



		box1->color = FColor::MakeRandomColor();		
		box2->color = FColor::MakeRandomColor();

		subdivisions.Add(box1);
		subdivisions.Add(box2);
	}

	return subdivisions;

}

void GenerationUtilities::MergeBoxes()
{

}

TArray<AActor*> GenerationUtilities::CreateWall(UEnhancedBox box, UPrimitiveShapeRenderer* renderer)
{
	FWorldContext* world = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);

	TArray<AActor*> allWalls = TArray<AActor*>();
	//ADynamicMeshWall* wall = world->World()->SpawnActor<ADynamicMeshWall>(renderer->dynamicWallBP, box.Center(), FRotator());
	//bool DEBUG_RenderAsOneWall = false;
	//
	//if (DEBUG_RenderAsOneWall)
	//{
	//	wall->wallSize = box.extent;
	//	wall->bHasWindow = true;
	//	wall->windowSize = FVector(box.extent.X * 1.8f, box.extent.Y * 1.8f, box.extent.Z + 150);
	//	wall->windowLocation = FVector(0, 0, 0);
	//	return;
	//}
	//wall->windowSize = FVector(box.extent.X * 1.8f, box.extent.Y * 1.8f, box.extent.Z + 150);
	//wall->windowLocation = FVector(0, 0, 0);

	float wallThickness = 10.f;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	FVector boxSizeY = FVector(box.extent.X, wallThickness, box.extent.Z);

	FVector yPos = box.YPosPoint() - FVector(0, wallThickness, 0);
	FVector yNeg = box.YNegPoint() + FVector(0, wallThickness, 0);
	
	ADynamicMeshWall* wall = world->World()->SpawnActor<ADynamicMeshWall>(renderer->dynamicWallBP, yPos, FRotator(0,0,0));
	wall->wallSize = boxSizeY;
	wall->bHasWindow = false;
	
	ADynamicMeshWall* wall2 = world->World()->SpawnActor<ADynamicMeshWall>(renderer->dynamicWallBP, yNeg, FRotator(0, 0, 0));
	wall2->wallSize = boxSizeY;
	wall2->bHasWindow = false;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	FVector boxSizeX = FVector(box.extent.Y, wallThickness, box.extent.Z);

	FVector xPos = box.XPosPoint() - FVector(wallThickness, 0, 0);
	FVector xNeg = box.XNegPoint() + FVector(wallThickness, 0, 0);

	ADynamicMeshWall* wall3 = world->World()->SpawnActor<ADynamicMeshWall>(renderer->dynamicWallBP, xPos, FRotator(0, 90, 0));
	wall3->wallSize = boxSizeX;
	wall3->bHasWindow = false;
	ADynamicMeshWall* wall4 = world->World()->SpawnActor<ADynamicMeshWall>(renderer->dynamicWallBP, xNeg, FRotator(0, 90, 0));
	wall4->wallSize = boxSizeX;
	wall4->bHasWindow = false;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	allWalls.Add(wall);
	allWalls.Add(wall2);
	allWalls.Add(wall3);
	allWalls.Add(wall4);

	return allWalls;
}

void GenerationUtilities::LinkBoxes(bool bAllowMultipleConnections)
{

}

void GenerationUtilities::ExportResults()
{

}

UTexture2D* GenerationUtilities::CreateFromSceneCapture2D(USceneCaptureComponent2D* sceneCapture, UObject* Outer)
{
	UTextureRenderTarget2D* target = sceneCapture->TextureTarget;
	UTexture2D* newTexture = target->ConstructTexture2D(Outer, "TempRenderTexture", EObjectFlags::RF_NoFlags, CTF_DeferCompression);
	newTexture->CompressionSettings = TextureCompressionSettings::TC_Grayscale;
	newTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	newTexture->SRGB = 0;
	newTexture->UpdateResource();
	return newTexture;
}
