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

const FVector UEnhancedBox::GetDimensionByRotationAxis(const EBoxRotationType& _rotation) const
{
	switch (_rotation)
	{
	case EBoxRotationType::LWH:
		return FVector(Length(), Width(), Height());
	case EBoxRotationType::HLW:
		return FVector(Height(), Length(), Width());
	case EBoxRotationType::HWL:
		return FVector(Height(), Width(), Length());
	case EBoxRotationType::WHL:
		return FVector(Width(), Height(), Length());
	case EBoxRotationType::WLH:
		return FVector(Width(), Length(), Height());
	case EBoxRotationType::LHW:
		return FVector(Length(), Height(), Width());
	default:
		UE_LOG(LogEnhancedBox, Error, TEXT("Invalid Rotation Type"));
		return FVector(0, 0, 0);
	}
}

const FRotator UEnhancedBox::GetRotationFromAxis(const EBoxRotationType& _rotation) const
{
	switch (_rotation)
	{
	case EBoxRotationType::LWH:
		return FromEuler(0.0f, 0.0f, 0.0f);

	case EBoxRotationType::LHW:
		return FromEuler(90.0f, 0.0f, 0.0f);

	case EBoxRotationType::WLH:
		return FromEuler(0.0f, 0.0f, 90.0f);

	case EBoxRotationType::HWL:
		return FromEuler(0.0f, 90.0, 0.0);

	case EBoxRotationType::WHL:
		return FromEuler(0.0f, 90.0f, 0.0f);

	case EBoxRotationType::HLW:
		return FromEuler(0.0f, 90.0f, 90.0f);

	case EBoxRotationType::ALL:
		return FromEuler(0.0f, 0.0f, 0.0f);

	default:
		return FromEuler(0.0f, 0.0f, 0.0f);

	}
}


const FRotator UEnhancedBox::GetRotationAligned() const
{
#define X Width()
#define Y Length()
#define Z Height()

	if (X >= Y && Y >= Z && X >= Z)
	{
		return FromEuler(0.0f, 0.0f, 0.0f);
	}
	if (X >= Y && X >= Z && Z >= Y)
	{
		return FromEuler(90.0f, 0.0f, 0.0f);
	}
	if (Z >= X && Z >= Y && Y >= X)
	{
		return FromEuler(0.0f, 90.0f, 0.0f);
	}
	if (Z >= X && Z >= Y && X >= Y)
	{
		return FromEuler(90.0f, 0.0f, 90.0f);
	}
	if (Y >= X && Y >= Z && X >= Z)
	{
		return FromEuler(0.0f, 0.0f, 90.0f);
	}
	if (Y >= X && Y >= Z && Z >= X)
	{
		return FromEuler(0.0f, 90.0f, 90.0f);
	}
	UE_LOG(LogEnhancedBox, Warning, L"No Rotation found");

	return FromEuler(0.0f, 0.0f, 0.0f);

#undef X
#undef Y
#undef Z
}

float URectangleBin::GetCurrentCapacity()
{
	float capacity = 0.0f;

	for (int i = 0; i < items.Num(); i++)
	{
		capacity += items[i]->weight;
	}

	return capacity;

}

float URectangleBin::GetFillingRatio()
{
	if (Volume() == 0.0f)
	{
		return 0.0f;
	}

	float allBoxesVolume = 0.0f;

	for (int i = 0; i < items.Num(); i++)
	{
		allBoxesVolume += items[i]->Volume();
	}

	return allBoxesVolume / Volume();
}

bool URectangleBin::PlaceItem(URectangleItem* boxToPlace, FVector initialPosition)
{
	bool fit = false;
	boxToPlace->origin = initialPosition;
	
	FVector validPosition = initialPosition;

	boxToPlace->origin = initialPosition;
	FBox originalBox = boxToPlace->box;
	FVector originalExtent = boxToPlace->extent;
	//boxToPlace->extent = boxToPlace->box.GetExtent();

	for (int i = 0; i < (int)EBoxRotationType::ALL; i++)
	{
		boxToPlace->rotationType = (EBoxRotationType)i;
		FVector dimension = boxToPlace->GetDimensionByRotationAxis((EBoxRotationType)i);

		if (Width() < initialPosition.X + dimension.X ||
			Length() < initialPosition.Y + dimension.Y ||
			Height() < initialPosition.Z + dimension.Z)
		{
			continue;
		}

		boxToPlace->box = FBox::BuildAABB(initialPosition, dimension/2);
		boxToPlace->origin = initialPosition;
		boxToPlace->extent = boxToPlace->box.GetExtent();
		

		fit = true;
		for (int j = 0; j < items.Num(); j++)
		{
			//UE_LOG(LogEnhancedBox, Warning,
			//	TEXT("[ BoxToPlace : O - %s E - %s ] ///// [ Items(j) : O - %s E - %s ]"),
			//	*boxToPlace->origin.ToString(),
			//	*boxToPlace->extent.ToString(),
			//	*items[j]->origin.ToString(),
			//	*items[j]->extent.ToString());
			
			if (boxToPlace->box.Intersect(items[j]->box))
			{
				//UE_LOG(LogEnhancedBox, Error, L"Intersection found !");
				fit = false;
				break;
			}
		}

		//Capacity Check (currently ignored)
		if (fit)
		{
			//if (this->GetCurrentCapacity() + boxToPlace->weight > maxCapacity)
			//{
			//	fit = false;
			//	return fit;
			//}
			return fit;
		}

		if (!fit)
		{
			boxToPlace->box = originalBox;
			boxToPlace->extent = originalExtent;
			boxToPlace->origin = validPosition;
			boxToPlace->box = originalBox;
			boxToPlace->extent = originalExtent;
		}
		return fit;
	}
	if (!fit)
	{
		boxToPlace->origin = validPosition;
		boxToPlace->box = originalBox;
		boxToPlace->extent = originalExtent;
	}

	return fit;
}

TArray<EBoxRotationType> URectangleBin::CanPlaceItemWithRotation(UEnhancedBox& boxToPlace)
{
	return TArray<EBoxRotationType>();
}

#define MIN_OFFSET 0.001f

bool Packer::PackToBin(URectangleBin* bin, URectangleItem* item)
{
	bool fit = false;

	if (bin->items.Num() == 0)
	{
		if (bin->PlaceItem(item, FVector(0,0,0)))
		{
			fit = true;
		}
		return fit;
	}
	FVector pivotPoint = FVector(0,0,0);

	for (int i = 0; i < (int)EBoxAxis::ALL; i++)
	{
		EBoxAxis currentAxis = (EBoxAxis)i;
		for (int j = 0; j < bin->items.Num(); j++)
		{
			FVector dimension = bin->items[j]->GetDimensionByRotationAxis();

			switch (currentAxis)
			{
			case EBoxAxis::WIDTH:
				pivotPoint = FVector
				(
					bin->items[j]->origin.X + dimension.X + MIN_OFFSET,
					bin->items[j]->origin.Y,
					bin->items[j]->origin.Z
				);
				break;
			case EBoxAxis::DEPTH:
				pivotPoint = FVector
				(
					bin->items[j]->origin.X,
					bin->items[j]->origin.Y + dimension.Y + MIN_OFFSET,
					bin->items[j]->origin.Z 
				);
				break;
			case EBoxAxis::HEIGHT:
				pivotPoint = FVector
				(
					bin->items[j]->origin.X,
					bin->items[j]->origin.Y,
					bin->items[j]->origin.Z + dimension.Z + MIN_OFFSET
				);
				break;
			case EBoxAxis::ALL:
			default:
				UE_LOG(LogEnhancedBox, Warning, L"Invalid Box Axis");
				break;
			}
			//UE_LOG(LogEnhancedBox, Warning, TEXT("%s"), *pivotPoint.ToString());

			if (bin->PlaceItem(item, pivotPoint))
			{
				fit = true;
				//delete(dimension);
				break;
			}
		}
		if (fit)
		{
			return fit;
		}
	}
	return fit;
}

bool Packer::PackToBin_V2(URectangleBin* bin, URectangleItem* item)
{
	return false;
}

#undef MIN_OFFSET

FORCEINLINE void URectangleItem::MakeFromStaticMesh(UStaticMesh* mesh, FVector _origin /*= FVector(0, 0, 0)*/, float _rotation /*= 0.0f*/)
{
	origin = _origin;
	extent = FVector(
		mesh->GetBounds().BoxExtent.X * 2.0f,
		mesh->GetBounds().BoxExtent.Y * 2.0f,
		mesh->GetBounds().BoxExtent.Z * 2.0f);

	//UE_LOG(LogEnhancedBox, Display, L"%s", *extent.ToString());

}
