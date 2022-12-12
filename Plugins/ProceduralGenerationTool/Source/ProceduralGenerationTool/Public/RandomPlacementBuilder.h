// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PropBuilderComponentBase.h"
#include "RandomPlacementBuilder.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROCEDURALGENERATIONTOOL_API URandomPlacementBuilder : public UPropBuilderComponentBase
{
	GENERATED_BODY()
	
public:



	void StartPlacing() override;

	bool PlaceActor() override;

};
