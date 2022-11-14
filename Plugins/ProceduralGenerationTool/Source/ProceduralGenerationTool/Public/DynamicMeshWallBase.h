// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeometryActors/GeneratedDynamicMeshActor.h"
#include "GeometryScript/CreateNewAssetUtilityFunctions.h"
#include "GeometryScript/MeshAssetFunctions.h"

#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "GeometryScript/MeshModelingFunctions.h"
#include "GeometryScript/MeshBooleanFunctions.h"
#include "GeometryScript/MeshUVFunctions.h"


#include "Delegates/Delegate.h" 

#include "Kismet/KismetSystemLibrary.h"
#include "DynamicMeshWallBase.generated.h"


/**
 * Base methods for exporting dynamic meshes
 */
UCLASS()
class PROCEDURALGENERATIONTOOL_API ADynamicMeshWallBase : public AGeneratedDynamicMeshActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* generatedMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int meshIdDigits = 9;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString assetFolderPath;

	UFUNCTION(BlueprintCallable, Category = "DynamicMesh|Export")
	void GenerateStaticMesh();
	
	UFUNCTION(BlueprintCallable, Category = "DynamicMesh|Export")
	void UpdateStaticMesh();
	
	UFUNCTION(BlueprintCallable, Category = "DynamicWall")
	void GenerateBaseWall(FVector size);

	UFUNCTION(BlueprintCallable, Category = "DynamicWall")
	UDynamicMesh* GenerateBox(FVector size, FVector location);

	UFUNCTION(BlueprintCallable, Category = "DynamicWall")
	UDynamicMesh* GenerateBoxHole(FVector size, FVector location);

	UFUNCTION(BlueprintCallable, Category = "DynamicWall")
	UDynamicMesh* GenerateRoundedBoxHole(FVector size, FVector location, int precision, float cornerRadius);

	UFUNCTION(BlueprintCallable, Category = "DynamicWall")
	void AlignUVScale(UDynamicMesh* target, FVector2D newScale);

	UFUNCTION(BlueprintCallable, Category = "DynamicWall")
	void SetMaterial(UDynamicMeshComponent* mesh, UMaterialInterface* material);

	DECLARE_DELEGATE(FDynamicMeshActions);

	TArray<FDynamicMeshActions> actions;


};
