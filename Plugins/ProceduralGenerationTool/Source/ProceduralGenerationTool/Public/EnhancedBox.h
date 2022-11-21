// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <ToolContextInterfaces.h>

DECLARE_LOG_CATEGORY_EXTERN(LogEnhancedBox, Log, All);

UENUM(Blueprintable)
enum class EBoxRotationType : uint8
{
	LWH, // Length - Width - Height
	HLW, // Height - Length - Width
	HWL, // Height - Width - Length
	WHL, // Width - Height - Length
	WLH, // Width - Length - Height
	LHW, // Length - Height - Width 
	ALL
};



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

		TArray<UEnhancedBox> subdivisions;

		FColor color;

		EBoxRotationType rotationType;

		//Generates all the vertices of the Box
		void GenerateVertices(const UEnhancedBox& _centralBox);

		void GenerateVertices();

		void DrawBox(IToolsContextRenderAPI* RenderAPI, const FColor& _color = FColor::Red, float thickness = 2.f);

		void DrawLine(IToolsContextRenderAPI* RenderAPI, const FVector& start, const FVector& end, const FColor& _color = FColor::Red, float thickness = 2.f);

		FVector RotateBox(const FVector& boxOrigin, FVector fromLocation, float angle);
		
		FORCEINLINE FString ToString() { return FString::Printf(TEXT("Origin : %s - Extent : %s"), *origin.ToString(), *extent.ToString()); }

		FORCEINLINE float Area2D() { return extent.X * extent.Y; }

		FORCEINLINE float Volume() { return extent.X * extent.Y * extent.Z; }

		FORCEINLINE float Perimeter2D() { return 2 * extent.X + 2 * extent.Y; }

		FORCEINLINE float Width() { return extent.X; }

		FORCEINLINE float Height() { return extent.Y; }
		
		FORCEINLINE float Length() { return extent.Z; }

		FORCEINLINE float WidthRatio() { return Height() == 0.0f ? 0.0f : Width() / Height(); }

		FORCEINLINE float HeightRatio() { return Width() == 0.0f ? 0.0f : Height() / Width(); }
		
		FORCEINLINE FVector Center() { return box.GetCenter(); }

		FORCEINLINE FVector TopLeft() { return box.Min; }

		FORCEINLINE FVector TopRight() { return FVector(box.Min.X, box.Max.Y, box.Min.Z); }

		FORCEINLINE FVector XPosPoint() { return FVector(Center().X + extent.X, Center().Y, Center().Z); }

		FORCEINLINE FVector XNegPoint() { return FVector(Center().X - extent.X, Center().Y, Center().Z); }

		FORCEINLINE FVector YPosPoint() { return FVector(Center().X, Center().Y + extent.Y, Center().Z); }

		FORCEINLINE FVector YNegPoint() { return FVector(Center().X, Center().Y - extent.Y, Center().Z); }

		FORCEINLINE FVector ZPosPoint() { return FVector(Center().X, Center().Y, Center().Z + extent.Z); }

		FORCEINLINE FVector ZNegPoint() { return FVector(Center().X, Center().Y, Center().Z - extent.Z); }

		//Returns a Vector containing the Length - Width - Height (order moving dependingon rotation axis)
		FORCEINLINE FVector GetDimensionByRotationAxis() 
		{
			switch (rotationType)
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
				return FVector(0,0,0);
			}
		}

		FORCEINLINE FVector GetDimensionByRotationAxis(const EBoxRotationType& rotation)
		{
			switch (rotation)
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

};


//Bin class used for the Rectangle Packing (Bin Packing) algorithm
class PROCEDURALGENERATIONTOOL_API URectangleBin : public UEnhancedBox
{
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	int maxCapacity = -1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	TArray<UEnhancedBox> boxesInBin;

	UFUNCTION(BlueprintPure)
	float GetFillingRatio();

	bool PlaceItem(UEnhancedBox& boxToPlace);

	TArray<EBoxRotationType> CanPlaceItemWithRotation(UEnhancedBox& boxToPlace);



};

