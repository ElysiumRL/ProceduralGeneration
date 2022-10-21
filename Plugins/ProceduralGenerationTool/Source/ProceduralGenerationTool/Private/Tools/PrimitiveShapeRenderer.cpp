// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/PrimitiveShapeRenderer.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

#include "SettingsExporterImporter.h"

DEFINE_LOG_CATEGORY(LogShapeRenderer);

UInteractiveTool* UPrimitiveShapeRendererToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UPrimitiveShapeRenderer* NewTool = NewObject<UPrimitiveShapeRenderer>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

#pragma region PrimitiveShapeRenderer

UPrimitiveShapeRenderer::UPrimitiveShapeRenderer()
{
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
		GenerationUtilities::results[i].DrawBox(RenderAPI, GenerationUtilities::results[i].color, 5.f);
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
	GenerationUtilities::results.Empty();

	GenerationUtilities::Subdivide(centralBox, centralBox, 5, ESubdivisionType::Vertical);

	UE_LOG(LogShapeRenderer, Warning, TEXT("Generation Done"));


	for (int i = 0; i < GenerationUtilities::results.Num(); i++)
	{
		UE_LOG(LogShapeRenderer, Warning, TEXT("%s"), *GenerationUtilities::results[i].ToString());

	}




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


void UPrimitiveShapeRendererProperties::StartGeneration()
{
	if (tool == nullptr)
	{
		UE_LOG(LogShapeRenderer, Error, TEXT("Can't start Generation : Tool not found!"));
	}
	tool->StartProceduralGeneration();
}

#pragma endregion Properties

#pragma region EnhancedBox

UEnhancedBox::UEnhancedBox(const FVector& origin, const FVector& extent, float _rotation, const FIntVector& _relativeLocation)
{
	subdivisions = TArray<UEnhancedBox>();
	box = FBox::BuildAABB(origin, extent);
	this->origin = origin;
	this->extent = extent;
	relativeLocation = _relativeLocation;
	rotation = _rotation;
	GenerateVertices();
}

UEnhancedBox::UEnhancedBox(const FVector& origin, const FVector& extent, float _rotation, const FIntVector& _relativeLocation, const UEnhancedBox& centralBox)
{
	subdivisions = TArray<UEnhancedBox>();
	box = FBox::BuildAABB(origin, extent);
	relativeLocation = _relativeLocation;
	this->origin = origin;
	this->extent = extent;
	rotation = _rotation;
	GenerateVertices(centralBox);
}

void UEnhancedBox::GenerateVertices(const UEnhancedBox& _centralBox)
{
	TArray<FVector> allPoints = TArray<FVector>();
	allPoints.Add(box.Min);										// 1
	allPoints.Add(FVector(box.Max.X, box.Min.Y, box.Min.Z));	// 2
	allPoints.Add(FVector(box.Max.X, box.Max.Y, box.Min.Z));	// 3
	allPoints.Add(FVector(box.Min.X, box.Max.Y, box.Min.Z));	// 4
	allPoints.Add(FVector(box.Max.X, box.Min.Y, box.Max.Z));	// 5
	allPoints.Add(FVector(box.Min.X, box.Min.Y, box.Max.Z));	// 6
	allPoints.Add(FVector(box.Min.X, box.Max.Y, box.Max.Z));	// 7
	allPoints.Add(box.Max);										// 8

	for (int i = 0; i < allPoints.Num(); i++)
	{
		allPoints[i] = RotateBox(_centralBox.box.GetCenter(), allPoints[i], rotation * UKismetMathLibrary::GetPI() / 180.f);
	}

	vertices = allPoints;
}

void UEnhancedBox::GenerateVertices()
{
	TArray<FVector> allPoints = TArray<FVector>();
	allPoints.Add(box.Min);										// 1
	allPoints.Add(FVector(box.Max.X, box.Min.Y, box.Min.Z));	// 2
	allPoints.Add(FVector(box.Max.X, box.Max.Y, box.Min.Z));	// 3
	allPoints.Add(FVector(box.Min.X, box.Max.Y, box.Min.Z));	// 4
	allPoints.Add(FVector(box.Max.X, box.Min.Y, box.Max.Z));	// 5
	allPoints.Add(FVector(box.Min.X, box.Min.Y, box.Max.Z));	// 6
	allPoints.Add(FVector(box.Min.X, box.Max.Y, box.Max.Z));	// 7
	allPoints.Add(box.Max);										// 8

	for (int i = 0; i < allPoints.Num(); i++)
	{
		allPoints[i] = RotateBox(box.GetCenter(), allPoints[i], rotation * UKismetMathLibrary::GetPI() / 180.f);
	}

	vertices = allPoints;

}

void UEnhancedBox::DrawBox(IToolsContextRenderAPI* RenderAPI, const FColor& _color /*= FColor::Red*/, float thickness /*= 2.f*/)
{
	// Vertices must be precisely at in the format of GetAllBoxVertices() in order to work
	DrawLine(RenderAPI, vertices[5], vertices[4], _color, thickness);
	DrawLine(RenderAPI, vertices[6], vertices[7], _color, thickness);
	DrawLine(RenderAPI, vertices[0], vertices[1], _color, thickness);
	DrawLine(RenderAPI, vertices[3], vertices[2], _color, thickness);
	DrawLine(RenderAPI, vertices[0], vertices[5], _color, thickness);
	DrawLine(RenderAPI, vertices[1], vertices[4], _color, thickness);
	DrawLine(RenderAPI, vertices[3], vertices[6], _color, thickness);
	DrawLine(RenderAPI, vertices[2], vertices[7], _color, thickness);
	DrawLine(RenderAPI, vertices[4], vertices[7], _color, thickness);
	DrawLine(RenderAPI, vertices[1], vertices[2], _color, thickness);
	DrawLine(RenderAPI, vertices[5], vertices[6], _color, thickness);
	DrawLine(RenderAPI, vertices[0], vertices[3], _color, thickness);

}

void UEnhancedBox::DrawLine(IToolsContextRenderAPI* RenderAPI, const FVector& start, const FVector& end, const FColor& _color /*= FColor::Red*/, float thickness /*= 2.f*/)
{
	//From the InteractiveTool demo
	auto PDI = RenderAPI->GetPrimitiveDrawInterface();
	// draw a thin line that shows through objects
	PDI->DrawLine(start, end, _color, SDPG_Foreground, thickness, 0.0f, true);
	// draw a thicker line that is depth-tested
	PDI->DrawLine(start, end, _color, SDPG_World, thickness, 0.0f, true);

}

FVector UEnhancedBox::RotateBox(const FVector& boxOrigin, FVector fromLocation, float angle)
{
	float sinAngle = sin(angle);
	float cosAngle = cos(angle);

	fromLocation.X -= boxOrigin.X;
	fromLocation.Y -= boxOrigin.Y;

	float xnew = fromLocation.X * cosAngle - fromLocation.Y * sinAngle;
	float ynew = fromLocation.X * sinAngle + fromLocation.Y * cosAngle;

	fromLocation.X = xnew + boxOrigin.X;
	fromLocation.Y = ynew + boxOrigin.Y;

	return FVector(fromLocation.X, fromLocation.Y, fromLocation.Z);

}

#pragma endregion EnhancedBox



GenerationUtilities::GenerationUtilities()
{
}

GenerationUtilities::~GenerationUtilities()
{

}

TArray<UEnhancedBox> GenerationUtilities::results = TArray<UEnhancedBox>();

void GenerationUtilities::Subdivide(UEnhancedBox bounds, UEnhancedBox boxToSubdivide, int iterations, ESubdivisionType subdivisionType)
{
	// exit conditions
	//boxToSubdivide.Area();

	int minSubdivisionWidth = 0.2f * boxToSubdivide.Width();
	int maxSubdivisionWidth = 0.5f * boxToSubdivide.Width();
	int minSubdivisionHeight = 0.2f * boxToSubdivide.Height();
	int maxSubdivisionHeight = 0.5f * boxToSubdivide.Height();

	iterations--;

	if (iterations == 0 || UKismetMathLibrary::RandomFloat() <= 0.25f)
	{
		return;
	}

	//If the building is touching a horizontal edge, vertical subdivisions
	//will not cut anything off. If the building is touching both
	//vertical edges, one subdivision can be made.
	
	if (subdivisionType == ESubdivisionType::Vertical)
	//if (!(boxToSubdivide.TopLeft().Y == bounds.TopLeft().Y) || boxToSubdivide.TopLeft().Y + boxToSubdivide.Height() == bounds.Height())
	{
		//if (bounds.Width() == boxToSubdivide.Width())
		{
			TArray<UEnhancedBox> subdivisions =	GenerationUtilities::Split(bounds, boxToSubdivide, UKismetMathLibrary::RandomFloatInRange(minSubdivisionWidth, bounds.Width() - minSubdivisionWidth) / bounds.Width(), ESubdivisionType::Vertical);

			for (int i = 0; i < subdivisions.Num(); i++)
			{
				GenerationUtilities::Subdivide(subdivisions[i], subdivisions[i], iterations, ESubdivisionType::Horizontal);
			}
		}
		return;
	}
	else
	{
		TArray<UEnhancedBox> subdivisions = GenerationUtilities::Split(bounds, boxToSubdivide, UKismetMathLibrary::RandomFloatInRange(maxSubdivisionHeight, bounds.Height() - maxSubdivisionHeight) / bounds.Height(), ESubdivisionType::Horizontal);

		for (int i = 0; i < subdivisions.Num(); i++)
		{
			GenerationUtilities::Subdivide(subdivisions[i], subdivisions[i], iterations, ESubdivisionType::Vertical);
		}
		return;
	}


	//if not (b.position.y == 0 or (b.position.y + b.height) == cityBlock.height) {
	//	if b.width == cityBlock.width{
	//		// do one subdivision and recurse
	//		splitBuilding(vertical, randomDouble(minSubdivisionWidth, cityBlock.width - minSubdivisionWidth)
	//		for subBuilding in b.subBuildings {
	//		  subdivide(horizontal, subBuilding)
	//		}
	//		return
	//			}
	//	else { return }
	//}

	// get # subdivisions
	//minSubdivisionWidth = minSize / b.height // ensures that subdivisionWidth * b.height <= minSize
	//	maxSubdivisions = floor(b.width / minSubdivisionWidth)
	//	subdivisions = randomInt(2, maxSubdivisions)
	//
	//	// get subdivision widths
	//	widths[] // This will be the widths of our subdivided buildings
	//	freeWidth = b.width - minSubdivisionWidth * subdivisions
	//	weights[] // randomly assigned weight for free space
	//	sumWeight
	//
	//	for i = 1 to subdivisions{
	//	  randWeight = random()
	//	  weights[i] = randWeight
	//	  sumWeight += randWeight
	//	}
	//
	//		for i = 1 to subdivisions{
	//		  widths[i] = minSubdivisionWidth + (weights[i] / sumWeight) * freeWidth
	//		}
	//
	//			// transform individual widths into coordinates for building split
	//			cumulativeWidth = 0
	//
	//			for i = 1 to(subdivisions - 1) {
	//				cumulativeWidth += widths[i]
	//					splitBuilding(vertical, cumulativeWidth)
	//			}
	//
	//// recurse
	//for subBuilding in b.subBuildings{
	//  subdivide(horizontal, subBuilding)
	//}
}

TArray<UEnhancedBox> GenerationUtilities::Split(UEnhancedBox bounds, UEnhancedBox boxToSubdivide, float splitLocationFromAxis, ESubdivisionType subdivisionType)
{
	FVector offset = FVector(10, 10, 0);

	TArray<UEnhancedBox> subdivisions;

	//Horizontal
	if (subdivisionType != ESubdivisionType::Horizontal) 
	{
		FVector box1Center = FVector(
			boxToSubdivide.TopRight().X + (boxToSubdivide.Width() * splitLocationFromAxis),
			boxToSubdivide.box.GetCenter().Y,
			bounds.box.GetCenter().Z) + offset;

		UE_LOG(LogTemp, Error, TEXT("Box 1 Origin : %s"), *box1Center.ToString());


		FVector box1Extends = FVector(
			boxToSubdivide.extent.X * splitLocationFromAxis,
			boxToSubdivide.extent.Y,
			boxToSubdivide.extent.Z);
		
		UE_LOG(LogTemp, Error, TEXT("Box 1 Extends : %s"), *box1Extends.ToString());

		UEnhancedBox box1 = UEnhancedBox(box1Center, box1Extends);

		FVector box2Center = FVector(
			boxToSubdivide.TopLeft().X + boxToSubdivide.Width() + (boxToSubdivide.Width() * splitLocationFromAxis),
			boxToSubdivide.box.GetCenter().Y,
			bounds.box.GetCenter().Z) + offset;
		
		UE_LOG(LogTemp, Error, TEXT("Box 2 Origin : %s"), *box2Center.ToString());

		FVector box2Extends = FVector(
			boxToSubdivide.extent.X * (1.0f - splitLocationFromAxis),
			boxToSubdivide.extent.Y,
			boxToSubdivide.extent.Z);
		
		UE_LOG(LogTemp, Error, TEXT("Box 2 Extends : %s"), *box2Extends.ToString());

		UEnhancedBox box2 = UEnhancedBox(box2Center, box2Extends);
		
		box1.color = FColor::MakeRandomColor();
		box2.color = FColor::MakeRandomColor();

		GenerationUtilities::results.Add(box1);
		
		GenerationUtilities::results.Add(box2);
		
		subdivisions.Add(box1);
		
		subdivisions.Add(box2);

	}
	else //Vertical
	{
		FVector box1Center = FVector(
			boxToSubdivide.box.GetCenter().X,
			boxToSubdivide.TopRight().Y - splitLocationFromAxis * (boxToSubdivide.Height()),
			bounds.box.GetCenter().Z) + offset;
		
		UE_LOG(LogTemp, Error, TEXT("Box 1 Origin : %s"), *box1Center.ToString());

		FVector box1Extends = FVector(
			boxToSubdivide.extent.X,
			boxToSubdivide.extent.Y * splitLocationFromAxis,
			boxToSubdivide.extent.Z);
		
		UE_LOG(LogTemp, Error, TEXT("Box 1 Extends : %s"), *box1Extends.ToString());

		UEnhancedBox box1 = UEnhancedBox(box1Center, box1Extends);

		FVector box2Center = FVector(
			boxToSubdivide.box.GetCenter().X,
			boxToSubdivide.TopLeft().Y + boxToSubdivide.Height() - (boxToSubdivide.Height() * splitLocationFromAxis),
			bounds.box.GetCenter().Z) + offset;
		
		UE_LOG(LogTemp, Error, TEXT("Box 2 Origin : %s"), *box2Center.ToString());

		FVector box2Extends = FVector(
			boxToSubdivide.extent.X,
			boxToSubdivide.extent.Y * (1.0f - splitLocationFromAxis),
			boxToSubdivide.extent.Z);

		UE_LOG(LogTemp, Error, TEXT("Box 2 Extends : %s"), *box2Extends.ToString());

		UEnhancedBox box2 = UEnhancedBox(box2Center, box2Extends);

		box1.color = FColor::MakeRandomColor();
		box2.color = FColor::MakeRandomColor();

		GenerationUtilities::results.Add(box1);
		
		GenerationUtilities::results.Add(box2);
		
		subdivisions.Add(box1);
		
		subdivisions.Add(box2);
	}

	return subdivisions;

}
