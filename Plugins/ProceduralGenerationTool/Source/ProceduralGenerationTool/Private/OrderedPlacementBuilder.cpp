// Fill out your copyright notice in the Description page of Project Settings.


#include "OrderedPlacementBuilder.h"

void UOrderedPlacementBuilder::StartPlacing()
{

}

bool UOrderedPlacementBuilder::PlaceActor()
{
	switch (placementType)
	{
	case EPlacementType::Linear:
		return PlaceLinear();
	case EPlacementType::Cylinder:
		return PlaceCylinder();
	case EPlacementType::Cubic:
		return PlaceCubic();
	}
	return false;
}

bool UOrderedPlacementBuilder::PlaceLinear()
{
	



	return true;
}

bool UOrderedPlacementBuilder::PlaceCylinder()
{
	return true;
}

bool UOrderedPlacementBuilder::PlaceCubic()
{
	return true;
}
