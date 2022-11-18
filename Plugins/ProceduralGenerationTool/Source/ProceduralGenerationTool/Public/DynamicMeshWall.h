// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicMeshWallBase.h"
#include "DynamicMeshWall.generated.h"

/**
 * 
 */
UCLASS()
class PROCEDURALGENERATIONTOOL_API ADynamicMeshWall : public ADynamicMeshWallBase
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicWall|Wall")
	FVector wallSize;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicWall|Wall")
	FVector2D wallUVScale;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicWall|Wall")
	UMaterial* wallMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicWall|Window")
	bool bHasWindow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicWall|Window", meta = (EditCondition = "bHasWindow"))
	FVector windowSize;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicWall|Window", meta = (EditCondition = "bHasWindow"))
	FVector windowLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicWall|Window", meta = (EditCondition = "bHasWindow"))
	bool bRoundedWindow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicWall|Window", meta = (EditCondition = "bHasWindow"))
	float cornerRadius;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DynamicWall|Window", meta = (EditCondition = "bHasWindow"))
	int roundPrecision;


};
