// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include <ToolContextInterfaces.h>

#include "EnhancedBox.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEnhancedBox, Log, All);

UENUM(Blueprintable)
enum class EBoxRotationType : uint8
{
	WHL = 0, // Width  - Height - Length
	HLW = 1, // Height - Length - Width
	HWL = 2, // Height - Width  - Length
	LHW = 3, // Length - Height - Width 
	LWH = 4, // Length - Width  - Height
	WLH = 5, // Width  - Length - Height
	ALL = 6  // Index for loops
};

UENUM(Blueprintable)
enum class EBoxAxis : uint8
{
	WIDTH,
	HEIGHT,
	DEPTH,
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

		EBoxRotationType rotationType = EBoxRotationType::WHL;

		void GenerateVertices(const UEnhancedBox& _centralBox);
		
		//Generates all the vertices of the Box
		void GenerateVertices();

		void DrawBox(IToolsContextRenderAPI* RenderAPI, const FColor& _color = FColor::Red, float thickness = 2.f);

		void DrawLine(IToolsContextRenderAPI* RenderAPI, const FVector& start, const FVector& end, const FColor& _color = FColor::Red, float thickness = 2.f);

		FVector RotateBox(const FVector& boxOrigin, FVector fromLocation, float angle);
		
		FORCEINLINE FString ToString() { return FString::Printf(TEXT("Origin : %s - Extent : %s"), *origin.ToString(), *extent.ToString()); }

		FORCEINLINE float Area2D() { return extent.X * extent.Y; }

		FORCEINLINE float Volume() { return extent.X * extent.Y * extent.Z; }

		FORCEINLINE float Perimeter2D() { return 2 * extent.X + 2 * extent.Y; }

		FORCEINLINE const float Width() const { return extent.X; }

		FORCEINLINE const float Height() const { return extent.Y; }
		
		FORCEINLINE const float Length() const { return extent.Z; }

		FORCEINLINE float WidthRatio() { return Height() == 0.0f ? 0.0f : Width() / Height(); }

		FORCEINLINE float HeightRatio() { return Width() == 0.0f ? 0.0f : Height() / Width(); }
		
		FORCEINLINE const FVector Center() { return box.GetCenter(); }
		FORCEINLINE const FVector TopLeft() { return box.Min; }
		FORCEINLINE const FVector TopRight() { return FVector(box.Min.X, box.Max.Y, box.Min.Z); }
		FORCEINLINE const FVector XPosPoint() { return FVector(Center().X + extent.X, Center().Y, Center().Z); }
		FORCEINLINE const FVector XNegPoint() { return FVector(Center().X - extent.X, Center().Y, Center().Z); }
		FORCEINLINE const FVector YPosPoint() { return FVector(Center().X, Center().Y + extent.Y, Center().Z); }
		FORCEINLINE const FVector YNegPoint() { return FVector(Center().X, Center().Y - extent.Y, Center().Z); }
		FORCEINLINE const FVector ZPosPoint() { return FVector(Center().X, Center().Y, Center().Z + extent.Z); }
		FORCEINLINE const FVector ZNegPoint() { return FVector(Center().X, Center().Y, Center().Z - extent.Z); }

		FVector GetDimensionByRotationAxis(const EBoxRotationType& _rotation)
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

		//Returns a Vector containing the Length - Width - Height (order moving depending on rotation axis)
		FVector GetDimensionByRotationAxis() 
		{
			return GetDimensionByRotationAxis(rotationType);
		}

		//Makes a box from the static mesh approx size
		FORCEINLINE static UEnhancedBox MakeFromStaticMesh(UStaticMesh* mesh, FVector origin = FVector(0, 0, 0), float rotation = 0.0f);



};

class PROCEDURALGENERATIONTOOL_API URectangleItem : public UEnhancedBox
{

public:

	float weight = 0.0f;

	TSubclassOf<AActor> linkedActor;
	
	//Makes a box from the static mesh approx size
	FORCEINLINE void MakeFromStaticMesh(UStaticMesh* mesh, FVector _origin = FVector(0, 0, 0), float _rotation = 0.0f);

};


//Bin class used for the Rectangle Packing (Bin Packing) algorithm
class PROCEDURALGENERATIONTOOL_API URectangleBin : public UEnhancedBox
{

public:

	float maxCapacity = -1;
	
	TArray<URectangleItem*> items;

	float GetCurrentCapacity();

	float GetFillingRatio();

	bool PlaceItem(URectangleItem* boxToPlace, FVector initialPosition);

	TArray<EBoxRotationType> CanPlaceItemWithRotation(UEnhancedBox& boxToPlace);

};

class PROCEDURALGENERATIONTOOL_API Packer
{

public:

	float maxCapacity = -1;

	TArray<URectangleBin*> bins;

	TArray<URectangleItem*> items;

	TArray<URectangleItem*> unfitItems;

	bool PackToBin(URectangleBin* bin, URectangleItem* item);
};

USTRUCT(BlueprintType,Blueprintable)
struct FPackerItemSettings : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float weight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool useActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> actor;

};



