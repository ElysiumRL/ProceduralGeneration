// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GeometryActors/GeneratedDynamicMeshActor.h"
#include "DynamicMeshWallBase.generated.h"

UENUM(BlueprintType,Blueprintable)
enum class EMeshGenerationType : uint8
{
	Box,
	BoxHole,
	RoundedBoxHole,
	Count UMETA(Hidden)
};

USTRUCT(BlueprintType, Blueprintable)
struct FMeshGenerationContext
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
	EMeshGenerationType type;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
	FTransform transform;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (EditCondition = "type == EMeshGenerationType::RoundedBoxHole"))
	int precision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation", meta = (EditCondition = "type == EMeshGenerationType::RoundedBoxHole"))
	float cornerRadius;


};




/**
 * Base methods for exporting dynamic meshes
 */
UCLASS()
class PROCEDURALGENERATIONTOOL_API ADynamicMeshWallBase : public AGeneratedDynamicMeshActor
{
	GENERATED_BODY()

public:

	//the Generated Mesh in the Content browser (if there's any)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* generatedMesh;
	
	//Unique ID Digits for the Mesh Name
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int meshIdDigits = 9;
	
	//Path where the asset will be generated
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString assetFolderPath;

	//Contexts used to generate extra meshes in BP with a base definition in C++
	//Note: I have to do this because we can't override the Rebuild event in C++ and modifying the engine/module shouldn't be a requirement
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMeshGenerationContext> customMeshGeneration;

	//Converts the dynamic mesh in the world as a new Static Mesh in the disk
	UFUNCTION(BlueprintCallable, Category = "DynamicMesh|Export")
	void GenerateStaticMesh();
	
	//Updates the generated static mesh (if you did any changes to it)
	UFUNCTION(BlueprintCallable, Category = "DynamicMesh|Export")
	void UpdateStaticMesh();
	
	//Generates a simple wall
	UFUNCTION(BlueprintCallable, Category = "DynamicWall")
	void GenerateBaseWall(FVector size);

	//Generates a box
	UFUNCTION(BlueprintCallable, Category = "DynamicWall")
	UDynamicMesh* GenerateBox(FVector size, FVector location, FQuat rotator = FQuat());

	//Generates a hole in the dynamic mesh
	UFUNCTION(BlueprintCallable, Category = "DynamicWall")
	UDynamicMesh* GenerateBoxHole(FVector size, FVector location);
	
	//Generates a rounded hole in the dynamic mesh
	UFUNCTION(BlueprintCallable, Category = "DynamicWall")
	UDynamicMesh* GenerateRoundedBoxHole(FVector size, FVector location, int precision, float cornerRadius);
	
	//stretches the UV scale by an arbitrary vector to align it with a custom uv scale
	UFUNCTION(BlueprintCallable, Category = "DynamicWall")
	void AlignUVScale(UDynamicMesh* target, FVector2D newScale);

	//Sets the material for the generated mesh
	UFUNCTION(BlueprintCallable, Category = "DynamicWall")
	void SetMaterial(UDynamicMeshComponent* mesh, UMaterialInterface* material);

	//Generates extra meshes in BP from the customMeshGeneration property
	//This function is called after base wall & window generation and before uv scaling
	UFUNCTION(BlueprintCallable, Category = "DynamicWall")
	void RunCustomGeneration();

	//Dynamic mesh sent by the Rebuild event to draw on
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DynamicWall")
	UDynamicMesh* dynamicMesh;
};
