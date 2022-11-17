// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicMeshWallBase.h"
#include "GeometryScript/CreateNewAssetUtilityFunctions.h"
#include "GeometryScript/MeshAssetFunctions.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "GeometryScript/MeshModelingFunctions.h"
#include "GeometryScript/MeshBooleanFunctions.h"
#include "GeometryScript/MeshUVFunctions.h"
#include "Kismet/KismetSystemLibrary.h"


DEFINE_LOG_CATEGORY_STATIC(LogDynamicMesh, Log, All);


void ADynamicMeshWallBase::GenerateStaticMesh()
{
	auto mesh = GetDynamicMeshComponent()->GetDynamicMesh();
	FString uniqueAssetPathAndName;
	FGeometryScriptUniqueAssetNameOptions options;
	options.UniqueIDDigits = meshIdDigits;
	
	//Discarded
	FString uniqueAssetName;
	TEnumAsByte<EGeometryScriptOutcomePins> outcome = TEnumAsByte<EGeometryScriptOutcomePins>();
	//
	UGeometryScriptLibrary_CreateNewAssetFunctions::CreateUniqueNewAssetPathName
	(assetFolderPath,
		UKismetSystemLibrary::GetDisplayName(this),
		uniqueAssetPathAndName,
		uniqueAssetName,
		options,
		outcome);

	if (outcome.GetValue() == Failure)
	{
		UE_LOG(LogDynamicMesh, Error, TEXT("Could not create unique asset path and name"));
		return;
	}

	FGeometryScriptCreateNewStaticMeshAssetOptions staticMeshOptions;

	generatedMesh =	UGeometryScriptLibrary_CreateNewAssetFunctions::CreateNewStaticMeshAssetFromMesh
	(mesh,
		uniqueAssetPathAndName, 
		staticMeshOptions,
		outcome);

	if (outcome.GetValue() == Failure)
	{
		UE_LOG(LogDynamicMesh, Error, TEXT("Could not create static mesh from dynamic mesh"));
		return;
	}
	if (generatedMesh != nullptr)
	{
		for (int i = 0; i < GetDynamicMeshComponent()->GetMaterials().Num(); i++)
		{
			generatedMesh->SetMaterial(i, GetDynamicMeshComponent()->GetMaterials()[i]);
		}
	}

	UE_LOG(LogDynamicMesh, Display, TEXT("Static Mesh Generated !"));


}

void ADynamicMeshWallBase::UpdateStaticMesh()
{
	auto mesh = GetDynamicMeshComponent()->GetDynamicMesh();
	FGeometryScriptCopyMeshToAssetOptions options;
	FGeometryScriptMeshWriteLOD targetLOD;
	TEnumAsByte<EGeometryScriptOutcomePins> Outcome = TEnumAsByte<EGeometryScriptOutcomePins>();

	UGeometryScriptLibrary_StaticMeshFunctions::CopyMeshToStaticMesh(mesh, generatedMesh, options, targetLOD, Outcome);

	if (Outcome.GetValue() == Failure)
	{
		UE_LOG(LogDynamicMesh, Error, TEXT("Could not update generated mesh"));
		return;
	}

	if (generatedMesh != nullptr)
	{
		for (int i = 0; i < GetDynamicMeshComponent()->GetMaterials().Num(); i++)
		{
			generatedMesh->SetMaterial(i, GetDynamicMeshComponent()->GetMaterials()[i]);
		}
	}
	UE_LOG(LogDynamicMesh, Display, TEXT("Static Mesh Updated !"));

}



void ADynamicMeshWallBase::GenerateBaseWall(FVector size)
{
	auto mesh = DynamicMeshComponent->GetDynamicMesh();
	FGeometryScriptPrimitiveOptions primitiveOptions;
	FTransform transform = FTransform();
	FVector doubleWallSize = size * 2.f;
	float zPosition = size.Z - doubleWallSize.Z;
	transform.SetLocation(FVector(0, 0, zPosition));
	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendBox(mesh, primitiveOptions, transform, doubleWallSize.X, doubleWallSize.Y, doubleWallSize.Z);
}

UDynamicMesh* ADynamicMeshWallBase::GenerateBox(FVector size, FVector location)
{
	auto mesh = DynamicMeshComponent->GetDynamicMesh();
	UDynamicMesh* newMesh = AllocateComputeMesh();

	FGeometryScriptPrimitiveOptions primitiveOptions;
	FTransform transform = FTransform();
	FQuat rotation = FQuat::MakeFromEuler(FVector(90, 0, 0));
	transform.SetRotation(rotation);
	newMesh = UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendRectangleXY(newMesh, primitiveOptions, transform, size.X, size.Z);

	return newMesh;
}

UDynamicMesh* ADynamicMeshWallBase::GenerateBoxHole(FVector size, FVector location)
{
	auto mesh = DynamicMeshComponent->GetDynamicMesh();
	UDynamicMesh* newMesh = AllocateComputeMesh();

	FGeometryScriptPrimitiveOptions primitiveOptions;
	FTransform transform = FTransform();
	FQuat rotation = FQuat::MakeFromEuler(FVector(90, 0, 0));
	transform.SetRotation(rotation);
	newMesh = UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendRectangleXY(newMesh, primitiveOptions, transform, size.X, size.Z);
	FGeometryScriptMeshExtrudeOptions options;
	options.bSolidsToShells = true;
	options.ExtrudeDirection = FVector(0, 1, 0);
	options.ExtrudeDistance = size.Y;
	options.UVScale = 0.001f;

	FTransform booleanTransform = FTransform();
	FVector holeLocation = FVector(location.X, location.Y - size.Y / 2.f, location.Z);
	booleanTransform.SetLocation(holeLocation);
	FGeometryScriptMeshBooleanOptions booleanOptions;
	newMesh = UGeometryScriptLibrary_MeshModelingFunctions::ApplyMeshExtrude(newMesh, options);
	mesh = UGeometryScriptLibrary_MeshBooleanFunctions::ApplyMeshBoolean(DynamicMeshComponent->GetDynamicMesh(), FTransform(), newMesh, booleanTransform, EGeometryScriptBooleanOperation::Subtract, booleanOptions);

	return newMesh;
}

UDynamicMesh* ADynamicMeshWallBase::GenerateRoundedBoxHole(FVector size, FVector location, int precision, float _cornerRadius)
{
	auto mesh = DynamicMeshComponent->GetDynamicMesh();

	UDynamicMesh* newMesh = AllocateComputeMesh();

	FGeometryScriptPrimitiveOptions primitiveOptions;
	FTransform transform = FTransform();
	FQuat rotation = FQuat::MakeFromEuler(FVector(90, 0, 0));
	transform.SetRotation(rotation);
	newMesh = UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendRoundRectangleXY(newMesh, primitiveOptions, transform, size.X, size.Z, _cornerRadius, 0, 0, precision);

	FGeometryScriptMeshExtrudeOptions options;
	options.bSolidsToShells = true;
	options.ExtrudeDirection = FVector(0, 1, 0);
	options.ExtrudeDistance = size.Y;
	options.UVScale = 0.001f;

	FTransform booleanTransform = FTransform();
	FVector holeLocation = FVector(location.X, location.Y - size.Y / 2.f, location.Z);
	booleanTransform.SetLocation(holeLocation);
	FGeometryScriptMeshBooleanOptions booleanOptions;
	newMesh = UGeometryScriptLibrary_MeshModelingFunctions::ApplyMeshExtrude(newMesh, options);
	mesh = UGeometryScriptLibrary_MeshBooleanFunctions::ApplyMeshBoolean(DynamicMeshComponent->GetDynamicMesh(), FTransform(), newMesh, booleanTransform, EGeometryScriptBooleanOperation::Subtract, booleanOptions);


	return newMesh;
}

void ADynamicMeshWallBase::AlignUVScale(UDynamicMesh* target, FVector2D newScale)
{
	target = UGeometryScriptLibrary_MeshUVFunctions::ScaleMeshUVs(target, 0, newScale, FVector2D(1, 1));
}

void ADynamicMeshWallBase::SetMaterial(UDynamicMeshComponent* mesh, UMaterialInterface* material)
{
	mesh->SetMaterial(0, material);
}
