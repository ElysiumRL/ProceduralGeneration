// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/PrimitiveShapeRenderer.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

#include "SettingsExporterImporter.h"

DEFINE_LOG_CATEGORY(LogShapeRenderer);


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
}

TArray<FVector> UPrimitiveShapeRenderer::GetAllBoxVertices(FBox _Box,FBox _centralBox)
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


void UPrimitiveShapeRenderer::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	FString propertyName = Property->GetFName().ToString();
	SelectUpdateMethod();
	if (Properties->randomSubdivisionColor)
	{
		SetRandomSubdivisionColors();
	}
	//UpdateBoundingBox();
	//if (Properties->splitBox)
	//{
	//	UpdateBoxSubdivisions();
	//
	//	if (propertyName == TEXT("modifySubdivisionColor") ||
	//		propertyName == TEXT("randomSubdivisionColor") ||
	//		propertyName == TEXT("subdivisionCount"))
	//	{
	//		UpdateBoxSubdivisions();
	//		SetRandomSubdivisionColors();
	//	}
	//}
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
	//centralBox.box = FBox::BuildAABB(Properties->boxTransform, Properties->boxExtent);
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

				//FBox _box;
				//_box.Init();
				//int index = subdivisionBoxes.Add(_box);
				//subdivisionBoxes[index] = FBox::BuildAABB(subdivTransform, subdivExtent);
			}
			relK = 0;
		}
		relJ = 0;
	}

}

void UPrimitiveShapeRenderer::SetRandomSubdivisionColors()
{
	randomSubdivisionBoxColor.SetNum(subdivisionBoxes.Num(),true);
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

UInteractiveTool* UPrimitiveShapeRendererToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UPrimitiveShapeRenderer* NewTool = NewObject<UPrimitiveShapeRenderer>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}

UEnhancedBox::UEnhancedBox(const FVector& origin,const FVector& extent,float _rotation,const FIntVector& _relativeLocation)
{
	box = FBox::BuildAABB(origin, extent);
	relativeLocation = _relativeLocation;
	rotation = _rotation;
	GenerateVertices();
}

UEnhancedBox::UEnhancedBox(const FVector& origin, const FVector& extent,float _rotation, const FIntVector& _relativeLocation, const UEnhancedBox& centralBox)
{
	box = FBox::BuildAABB(origin, extent);
	relativeLocation = _relativeLocation;
	rotation = _rotation;
	GenerateVertices(centralBox);
}

void UEnhancedBox::GenerateVertices(const UEnhancedBox& _centralBox)
{
	TArray<FVector> allPoints = TArray<FVector>();
	allPoints.Add(box.Min);																// 1
	allPoints.Add(FVector(box.Max.X, box.Min.Y, box.Min.Z));	// 2
	allPoints.Add(FVector(box.Max.X, box.Max.Y, box.Min.Z));	// 3
	allPoints.Add(FVector(box.Min.X, box.Max.Y, box.Min.Z));	// 4
	allPoints.Add(FVector(box.Max.X, box.Min.Y, box.Max.Z));	// 5
	allPoints.Add(FVector(box.Min.X, box.Min.Y, box.Max.Z));	// 6
	allPoints.Add(FVector(box.Min.X, box.Max.Y, box.Max.Z));	// 7
	allPoints.Add(box.Max);																// 8

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

void UEnhancedBox::DrawBox(IToolsContextRenderAPI* RenderAPI, const FColor& color /*= FColor::Red*/, float thickness /*= 2.f*/)
{
	// Vertices must be precisely at in the format of GetAllBoxVertices() in order to work
	DrawLine(RenderAPI, vertices[5], vertices[4], color, thickness);
	DrawLine(RenderAPI, vertices[6], vertices[7], color, thickness);
	DrawLine(RenderAPI, vertices[0], vertices[1], color, thickness);
	DrawLine(RenderAPI, vertices[3], vertices[2], color, thickness);
	DrawLine(RenderAPI, vertices[0], vertices[5], color, thickness);
	DrawLine(RenderAPI, vertices[1], vertices[4], color, thickness);
	DrawLine(RenderAPI, vertices[3], vertices[6], color, thickness);
	DrawLine(RenderAPI, vertices[2], vertices[7], color, thickness);
	DrawLine(RenderAPI, vertices[4], vertices[7], color, thickness);
	DrawLine(RenderAPI, vertices[1], vertices[2], color, thickness);
	DrawLine(RenderAPI, vertices[5], vertices[6], color, thickness);
	DrawLine(RenderAPI, vertices[0], vertices[3], color, thickness);

}

void UEnhancedBox::DrawLine(IToolsContextRenderAPI* RenderAPI, const FVector& start, const FVector& end, const FColor& color /*= FColor::Red*/, float thickness /*= 2.f*/)
{
	//From the InteractiveTool demo
	auto PDI = RenderAPI->GetPrimitiveDrawInterface();
	// draw a thin line that shows through objects
	PDI->DrawLine(start, end, color, SDPG_Foreground, thickness, 0.0f, true);
	// draw a thicker line that is depth-tested
	PDI->DrawLine(start, end, color, SDPG_World, thickness, 0.0f, true);

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
