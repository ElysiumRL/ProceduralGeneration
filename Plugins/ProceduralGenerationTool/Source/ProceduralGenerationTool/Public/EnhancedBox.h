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
	HWL = 1, // Height - Width  - Length
	HLW = 2, // Height - Length - Width
	LHW = 3, // Length - Height - Width 
	LWH = 4, // Length - Width  - Height
	WLH = 5, // Width  - Length - Height
	ALL = 6  // Index for loops
};

UENUM(Blueprintable)
enum class EBoxAxis : uint8
{
	WIDTH = 0, //X
	HEIGHT = 1, //Z
	DEPTH = 2, //Y
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
		
		//Generates all the vertices of the Box
		void GenerateVertices();

		void GenerateVertices(const UEnhancedBox& _centralBox);
		
		void DrawBox(IToolsContextRenderAPI* RenderAPI, const FColor& _color = FColor::Red, float thickness = 2.f);

		void DrawLine(IToolsContextRenderAPI* RenderAPI, const FVector& start, const FVector& end, const FColor& _color = FColor::Red, float thickness = 2.f);

		FVector RotateBox(const FVector& boxOrigin, FVector fromLocation, float angle);
	
		FORCEINLINE const float Area2D()	  const { return extent.X * extent.Y; }
		FORCEINLINE const float Volume()	  const { return extent.X * extent.Y * extent.Z; }
		FORCEINLINE const float Perimeter2D() const { return 2.0f * extent.X + 2.0f * extent.Y; }
		FORCEINLINE const float Width()		  const	{ return extent.X; }
		FORCEINLINE const float Length()      const { return extent.Y; }
		FORCEINLINE const float Height()      const	{ return extent.Z; }
		FORCEINLINE const float WidthRatio()  const	{ return Length() == 0.0f ? 0.0f : Width() / Length(); }
		FORCEINLINE const float HeightRatio() const { return Width() == 0.0f ? 0.0f : Length() / Width(); }
		FORCEINLINE const FVector Center()    const { return box.GetCenter(); }
		FORCEINLINE const FVector TopLeft()   const { return box.Min; }
		FORCEINLINE const FVector TopRight()  const { return FVector(box.Min.X, box.Max.Y, box.Min.Z); }
		FORCEINLINE const FVector XPosPoint() const { return FVector(Center().X + extent.X, Center().Y, Center().Z); }
		FORCEINLINE const FVector XNegPoint() const { return FVector(Center().X - extent.X, Center().Y, Center().Z); }
		FORCEINLINE const FVector YPosPoint() const { return FVector(Center().X, Center().Y + extent.Y, Center().Z); }
		FORCEINLINE const FVector YNegPoint() const { return FVector(Center().X, Center().Y - extent.Y, Center().Z); }
		FORCEINLINE const FVector ZPosPoint() const { return FVector(Center().X, Center().Y, Center().Z + extent.Z); }
		FORCEINLINE const FVector ZNegPoint() const { return FVector(Center().X, Center().Y, Center().Z - extent.Z); }
		
		//Returns a Vector containing the Length - Width - Height (order moving depending on rotation axis)
		FORCEINLINE const FVector GetDimensionByRotationAxis() const { return GetDimensionByRotationAxis(rotationType); }
		
		const FVector GetDimensionByRotationAxis(const EBoxRotationType& _rotation) const;

		const FRotator GetRotationFromAxis(const EBoxRotationType& _rotation) const;

		const FRotator GetRotationAligned() const;

		//Returns correct Euler Angles
		static FORCEINLINE FRotator FromEuler(float roll, float pitch, float yaw) { return FRotator(pitch, yaw, roll); }

		//Makes a box from the static mesh approx size
		static FORCEINLINE UEnhancedBox MakeFromStaticMesh(UStaticMesh* mesh, FVector origin = FVector(0.0f, 0.0f, 0.0f), float rotation = 0.0f)
		{
			return UEnhancedBox(origin, mesh->GetPositiveBoundsExtension(), rotation, FIntVector(0, 0, 0));
		}


		FORCEINLINE const FString ToString() const { return FString::Printf(TEXT("Origin : %s - Extent : %s"), *origin.ToString(), *extent.ToString()); }
};

class PROCEDURALGENERATIONTOOL_API URectangleItem : public UEnhancedBox
{

public:

	float weight = 0.0f;

	TSubclassOf<AActor> linkedActor;
	
	UStaticMesh* linkedMesh;

	//Makes a box from the static mesh approx size
	FORCEINLINE void MakeFromStaticMesh(UStaticMesh* mesh, FVector _origin = FVector(0, 0, 0), float _rotation = 0.0f);

	friend bool operator<(const URectangleItem& l, const URectangleItem& r)
	{
		return l.extent.SizeSquared() > r.extent.SizeSquared();
	}
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


	URectangleBin* singularBin;

	bool PackToBin_V2(URectangleBin* bin, URectangleItem* item);


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