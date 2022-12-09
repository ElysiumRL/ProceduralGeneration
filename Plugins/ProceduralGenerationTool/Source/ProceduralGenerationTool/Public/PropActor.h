// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SettingsExporterImporter.h"

#include "PropActor.generated.h"

UCLASS()
class PROCEDURALGENERATIONTOOL_API APropActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APropActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TArray<FTag> propTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UStaticMesh* mesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default")
	UStaticMeshComponent* meshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool bEnablePhysics = true;



};
