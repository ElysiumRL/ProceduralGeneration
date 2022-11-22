// Fill out your copyright notice in the Description page of Project Settings.


#include "EnhancedBox.h"
#include <Kismet/KismetMathLibrary.h>

DEFINE_LOG_CATEGORY(LogEnhancedBox);

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

UEnhancedBox UEnhancedBox::MakeFromStaticMesh(UStaticMesh* mesh,FVector origin = FVector(0,0,0))
{
	UEnhancedBox newBox = UEnhancedBox()
	mesh->PositiveBoundsExtension
}

float URectangleBin::GetFillingRatio()
{
	if (Volume() == 0.0f)
	{
		return 0.0f;
	}

	float allBoxesVolume = 0.0f;

	for (int i = 0; i < boxesInBin.Num(); i++)
	{
		allBoxesVolume += boxesInBin[i].Volume();
	}

	return allBoxesVolume / Volume();
}

bool URectangleBin::PlaceItem(UEnhancedBox& boxToPlace)
{









}

TArray<EBoxRotationType> URectangleBin::CanPlaceItemWithRotation(UEnhancedBox& boxToPlace)
{
	bool canFit = false;
	FVector pivot;
	TArray<EBoxRotationType> fittingRotations = TArray<EBoxRotationType>();

	for (int i = 0; i < (int)EBoxRotationType::ALL; i++)
	{
		FVector dimensions = this->GetDimensionByRotationAxis((EBoxRotationType)i);

		if (pivot.X + dimensions.X <= Width()
			&& pivot.Y + dimensions.Y < Height()
			&& pivot.Z + dimensions.Z < Length())
		{
			canFit = true;

			for (int j = 0; j < boxesInBin.Num(); j++)
			{
				if (boxesInBin[j].box.Intersect(boxToPlace.box))
				{
					canFit = false;
					boxToPlace.origin = FVector(0, 0, 0);
					break;
				}
			}
			if (canFit)
			{
				fittingRotations.Add((EBoxRotationType)i);
			}
		}
	}
	return fittingRotations;
}

void URectangleBin::PackBin()
{
	bool fitted = false;

	


}
