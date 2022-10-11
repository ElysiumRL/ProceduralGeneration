// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/PrimitiveShapeRenderer.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

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
	box.Init();
	subdivisionBoxes = TArray<FBox>();
}

void UPrimitiveShapeRenderer::OnClicked(const FInputDeviceRay& ClickPos)
{
	// Do Nothing
}

void UPrimitiveShapeRenderer::Render(IToolsContextRenderAPI* RenderAPI)
{
	UpdateBoundingBox();
	if (Properties->splitBox)
	{
		UpdateBoxSubdivisions();
	}
	DrawBox(GetAllBoxVertices(box), Properties->boxColor, Properties->boxThickness, RenderAPI);

	for (int i = 0; i < subdivisionBoxes.Num();i++)
	{
		//UE_LOG(LogTemp, Warning, TEXT("AAAAAAAAAAAAAA"));
		UE_LOG(LogTemp,Warning,TEXT("Subdivision %d : %s"),i,*subdivisionBoxes[i].GetCenter().ToString());
		DrawBox(GetAllBoxVertices(subdivisionBoxes[i]), Properties->subdivisionColor, Properties->subdivisionThickness, RenderAPI);
	}

}

void UPrimitiveShapeRenderer::DrawBox(TArray<FVector> vertices, IToolsContextRenderAPI* RenderAPI)
{
	DrawBox(vertices,FColor::Red, RenderAPI);
}

void UPrimitiveShapeRenderer::DrawBox(TArray<FVector> vertices, FColor color, IToolsContextRenderAPI* RenderAPI)
{
	DrawBox(vertices, color, 2.0f, RenderAPI);
}

void UPrimitiveShapeRenderer::DrawBox(TArray<FVector> vertices, FColor color, float thickness, IToolsContextRenderAPI* RenderAPI)
{
	// Vertices must be precisely at in the format of GetAllBoxVertices() in order to work
	DrawLine(vertices[5], vertices[4], color, thickness, RenderAPI);
	DrawLine(vertices[6], vertices[7], color, thickness, RenderAPI);
	DrawLine(vertices[0], vertices[1], color, thickness, RenderAPI);
	DrawLine(vertices[3], vertices[2], color, thickness, RenderAPI);
	DrawLine(vertices[0], vertices[5], color, thickness, RenderAPI);
	DrawLine(vertices[1], vertices[4], color, thickness, RenderAPI);
	DrawLine(vertices[3], vertices[6], color, thickness, RenderAPI);
	DrawLine(vertices[2], vertices[7], color, thickness, RenderAPI);
	DrawLine(vertices[4], vertices[7], color, thickness, RenderAPI);
	DrawLine(vertices[1], vertices[2], color, thickness, RenderAPI);
	DrawLine(vertices[5], vertices[6], color, thickness, RenderAPI);
	DrawLine(vertices[0], vertices[3], color, thickness, RenderAPI);
}

TArray<FVector> UPrimitiveShapeRenderer::GetAllBoxVertices(FBox Box)
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
	return allPoints;
}


void UPrimitiveShapeRenderer::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	UpdateBoundingBox();
	if (Properties->splitBox)
	{
		UpdateBoxSubdivisions();
	}
	Properties->reload = false;
}

FInputRayHit UPrimitiveShapeRenderer::FindRayHit(const FRay& WorldRay, FVector& HitPos)
{
	return FInputRayHit();
}

void UPrimitiveShapeRenderer::UpdateBoundingBox()
{
	box = FBox::BuildAABB(Properties->boxTransform, Properties->boxExtent);
}

void UPrimitiveShapeRenderer::UpdateBoxSubdivisions()
{
	subdivisionBoxes.Empty();
	if (Properties->subdivisionCount.X == 0) { Properties->subdivisionCount.X = 1; }
	if (Properties->subdivisionCount.Y == 0) { Properties->subdivisionCount.Y = 1; }
	if (Properties->subdivisionCount.Z == 0) { Properties->subdivisionCount.Z = 1; }
	
	FVector subdivExtent = FVector(
		(float)Properties->boxExtent.X / (float)Properties->subdivisionCount.X,
		(float)Properties->boxExtent.Y / (float)Properties->subdivisionCount.Y,
		(float)Properties->boxExtent.Z / (float)Properties->subdivisionCount.Z);

	UE_LOG(LogTemp, Warning, TEXT("----------------------------------"));
	int incr = 0;
	for (float i = 0; i < Properties->subdivisionCount.X; i++)
	{
		for (float j = 0; j < Properties->subdivisionCount.Y; j++)
		{
			for (float k = 0; k < Properties->subdivisionCount.Z; k++)
			{

				FVector subdivTransform = FVector(
					i * Properties->boxExtent.X + Properties->boxTransform.X,
					j * Properties->boxExtent.Y + Properties->boxTransform.Y,
					k * Properties->boxExtent.Z + Properties->boxTransform.Z);

				UE_LOG(LogTemp, Warning, TEXT("Vector %d : %s"),incr, *subdivTransform.ToString());

				FBox _box;
				_box.Init();
				int index = subdivisionBoxes.Add(_box);

				//_box.Init();
				subdivisionBoxes[index].BuildAABB(subdivTransform, subdivExtent);
				incr++;
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("----------------------------------"));
}

void UPrimitiveShapeRenderer::UpdateTool()
{
	UpdateBoundingBox();
	if (Properties->splitBox)
	{
		UpdateBoxSubdivisions();
	}
}

void UPrimitiveShapeRenderer::DrawLine(FVector start, FVector end, IToolsContextRenderAPI* RenderAPI)
{
	DrawLine(start, end, FColor::Red, 2.0f, RenderAPI);
}

void UPrimitiveShapeRenderer::DrawLine(FVector start, FVector end, FColor color, IToolsContextRenderAPI* RenderAPI)
{
	DrawLine(start, end, color, 2.0f, RenderAPI);
}

void UPrimitiveShapeRenderer::DrawLine(FVector start, FVector end, FColor color, float thickness, IToolsContextRenderAPI* RenderAPI)
{
	//From the InteractiveTool demo

	auto PDI = RenderAPI->GetPrimitiveDrawInterface();
	// draw a thin line that shows through objects
	PDI->DrawLine(start, end, color, SDPG_Foreground, thickness, 0.0f, true);
	// draw a thicker line that is depth-tested
	PDI->DrawLine(start, end, color, SDPG_World, thickness, 0.0f, true);

}

UPrimitiveShapeRendererProperties::UPrimitiveShapeRendererProperties()
{
	primitiveShape = EPrimitiveShapeType::Rectangle;

	boxExtent = FVector(100.0, 50.0, 100.0);
	boxTransform = FVector::Zero();
	rotation = 0.0f;
	boxColor = FColor::Red;
	boxThickness = 2.0f;

	splitBox = false;
	subdivisionCount = FIntVector(3, 3, 3);
	randomSubdivisionColor = false;
	subdivisionColor = FColor::Blue;
	subdivisionThickness = 1.f;

}

UInteractiveTool* UPrimitiveShapeRendererToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UPrimitiveShapeRenderer* NewTool = NewObject<UPrimitiveShapeRenderer>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}
