// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PropBuilderComponentBase.h"
#include "OrderedPlacementBuilder.generated.h"

UENUM(BlueprintType)
enum class EPlacementType : uint8
{
	Linear,
	Cylinder,
	Cubic,
};


/**
 * Symetrical placement for props
 */
UCLASS()
class PROCEDURALGENERATIONTOOL_API UOrderedPlacementBuilder : public UPropBuilderComponentBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	EPlacementType placementType;

	void StartPlacing() override;

	bool PlaceActor() override;

	bool PlaceLinear();

	bool PlaceCylinder();

	bool PlaceCubic();



};
