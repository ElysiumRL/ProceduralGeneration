// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/PrimitiveShapeRenderer.h"
#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"

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

}

void UPrimitiveShapeRenderer::OnClicked(const FInputDeviceRay& ClickPos)
{
	// Do Nothing
}

void UPrimitiveShapeRenderer::Render(IToolsContextRenderAPI* RenderAPI)
{
	UpdateBoundingBox();

	DrawBox(GetAllBoxVertices(box), Properties->boxColor, Properties->boxThickness, RenderAPI);

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
	DrawLine(vertices[5], vertices[4], Properties->boxColor, Properties->boxThickness, RenderAPI);
	DrawLine(vertices[6], vertices[7], Properties->boxColor, Properties->boxThickness, RenderAPI);
	DrawLine(vertices[0], vertices[1], Properties->boxColor, Properties->boxThickness, RenderAPI);
	DrawLine(vertices[3], vertices[2], Properties->boxColor, Properties->boxThickness, RenderAPI);
	DrawLine(vertices[0], vertices[5], Properties->boxColor, Properties->boxThickness, RenderAPI);
	DrawLine(vertices[1], vertices[4], Properties->boxColor, Properties->boxThickness, RenderAPI);
	DrawLine(vertices[3], vertices[6], Properties->boxColor, Properties->boxThickness, RenderAPI);
	DrawLine(vertices[2], vertices[7], Properties->boxColor, Properties->boxThickness, RenderAPI);
	DrawLine(vertices[4], vertices[7], Properties->boxColor, Properties->boxThickness, RenderAPI);
	DrawLine(vertices[1], vertices[2], Properties->boxColor, Properties->boxThickness, RenderAPI);
	DrawLine(vertices[5], vertices[6], Properties->boxColor, Properties->boxThickness, RenderAPI);
	DrawLine(vertices[0], vertices[3], Properties->boxColor, Properties->boxThickness, RenderAPI);
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

void UPrimitiveShapeRenderer::UpdateTool()
{
	UpdateBoundingBox();
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
}

UInteractiveTool* UPrimitiveShapeRendererToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UPrimitiveShapeRenderer* NewTool = NewObject<UPrimitiveShapeRenderer>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}
