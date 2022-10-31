// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <ToolContextInterfaces.h>


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

		//Generates all the vertices of the Box
		void GenerateVertices(const UEnhancedBox& _centralBox);

		void GenerateVertices();

		void DrawBox(IToolsContextRenderAPI* RenderAPI, const FColor& _color = FColor::Red, float thickness = 2.f);

		void DrawLine(IToolsContextRenderAPI* RenderAPI, const FVector& start, const FVector& end, const FColor& _color = FColor::Red, float thickness = 2.f);

		FVector RotateBox(const FVector& boxOrigin, FVector fromLocation, float angle);

		FORCEINLINE float Area2D() { return extent.X * extent.Y; }

		FORCEINLINE float Area3D() { return extent.X * extent.Y * extent.Z; }

		FORCEINLINE float Perimeter2D() { return 2 * extent.X + 2 * extent.Y; }

		FORCEINLINE float Width() { return extent.X; }

		FORCEINLINE float Height() { return extent.Y; }

		FORCEINLINE float WidthRatio() { return Height() == 0.0f ? 0.0f : Width() / Height(); }

		FORCEINLINE float HeightRatio() { return Width() == 0.0f ? 0.0f : Height() / Width(); }

		FORCEINLINE FVector TopLeft() { return box.Min; }

		FORCEINLINE FVector TopRight() { return FVector(box.Min.X, box.Max.Y, box.Min.Z); }

		FORCEINLINE FString ToString() { return FString::Printf(TEXT("Origin : %s - Extent : %s"), *origin.ToString(), *extent.ToString()); }
};
