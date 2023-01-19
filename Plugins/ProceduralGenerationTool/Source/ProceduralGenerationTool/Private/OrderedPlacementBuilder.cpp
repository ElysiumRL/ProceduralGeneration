// Fill out your copyright notice in the Description page of Project Settings.


#include "OrderedPlacementBuilder.h"
#include "Kismet/KismetMathLibrary.h"

void UOrderedPlacementBuilder::StartPlacing()
{
	packerInstance.bins.Reset();
	packerInstance.items.Reset();

	//Create bin
	URectangleBin* bin = new URectangleBin();
	bin->extent = GetUnscaledBoxExtent() * 2.0f;

	packerInstance.bins.Add(bin);

	//Get settings
	TArray<FPackerItemSettings*> rows;
	settings->GetAllRows<FPackerItemSettings>(L"", rows);

	//Generate items
	for (int i = 0; i < maxItems; i++)
	{
		int itemToSpawnIndex = UKismetMathLibrary::RandomIntegerInRange(0, rows.Num() - 1);
		GenerateItem(rows[itemToSpawnIndex]);
	}
	bool bShouldSort = true;
	if (bShouldSort)
	{
		packerInstance.items.Sort();
	}
	
	for (int i = 0; i < packerInstance.items.Num(); i++)
	{
		URectangleItem* item = packerInstance.items[i];

		if (PlaceLinear(item))
		{
			FActorSpawnParameters spawnParams = FActorSpawnParameters();
			spawnParams.Owner = this->GetOwner();
			spawnParams.Instigator = this->GetOwner()->GetInstigator();
			spawnParams.bNoFail = false;
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			FVector originFixed = FVector(item->origin.Z, item->origin.Y, item->origin.X) + this->GetComponentLocation();
			FRotator rotation = item->GetRotationFromAxis(item->rotationType);

			AActor* actorCreated = GetWorld()->SpawnActor<AActor>(item->linkedActor, originFixed, rotation, spawnParams);
			if (actorCreated)
			{
				UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(actorCreated->GetComponentByClass(UStaticMeshComponent::StaticClass()));
				mesh->SetStaticMesh(item->linkedMesh);

				packerInstance.bins[0]->items.Add(item);
			}
			packerInstance.items.Remove(item);
			i--;
		}
		else
		{
			packerInstance.unfitItems.Add(item);
			packerInstance.items.Remove(item);
			i--;
		}
	}
}

bool UOrderedPlacementBuilder::PlaceActor()
{
	switch (placementType)
	{
	case EPlacementType::Linear:
		return true;
	case EPlacementType::Cylinder:
		return PlaceCylinder();
	case EPlacementType::Cubic:
		return PlaceCubic();
	}
	return false;
}
#define MIN_OFFSET 0.001


bool UOrderedPlacementBuilder::PlaceLinear(URectangleItem* item)
{

	bool fit = false;

	auto& bin = this->packerInstance.bins[0];

	if (bin->items.Num() == 0)
	{
		if (bin->PlaceItem(item, FVector(0, 0, 0)))
		{
			fit = true;
		}
		return fit;
	}

	auto& lastItem = this->packerInstance.items[packerInstance.items.Num() - 1];

	FVector pivotPoint = FVector(0, 0, 0);

	for (int j = 0; j < bin->items.Num(); j++)
	{
		FVector baseDimension = lastItem->GetDimensionByRotationAxis();

		switch (placementAxis)
		{
		case EBoxAxis::WIDTH:
			pivotPoint = FVector
			(
				lastItem->origin.X + baseDimension.X + MIN_OFFSET,
				lastItem->origin.Y,
				lastItem->origin.Z
			);
			break;
		case EBoxAxis::DEPTH:
			pivotPoint = FVector
			(
				lastItem->origin.X,
				lastItem->origin.Y + baseDimension.Y + MIN_OFFSET,
				lastItem->origin.Z
			);
			break;
		case EBoxAxis::HEIGHT:
			pivotPoint = FVector
			(
				lastItem->origin.X,
				lastItem->origin.Y,
				lastItem->origin.Z + baseDimension.Z + MIN_OFFSET
			);
			break;
		case EBoxAxis::ALL:
		default:
			UE_LOG(LogEnhancedBox, Warning, L"Invalid Box Axis");
			break;
		}
		//UE_LOG(LogEnhancedBox, Warning, TEXT("%s"), *pivotPoint.ToString());


		FVector validPosition = pivotPoint;
		item->origin = pivotPoint;
		FBox originalBox = item->box;
		FVector originalExtent = item->extent;
		//item->extent = item->box.GetExtent();

		for (int i = 0; i < (int)EBoxRotationType::ALL; i++)
		{
			item->rotationType = (EBoxRotationType)i;
			FVector dimension = item->GetDimensionByRotationAxis((EBoxRotationType)i) / 2;

			if (bin->Width()  < pivotPoint.X + dimension.X ||
				bin->Length() < pivotPoint.Y + dimension.Y ||
				bin->Height() < pivotPoint.Z + dimension.Z)
			{
				continue;
			}

			item->box = FBox::BuildAABB(pivotPoint, dimension);
			item->origin = pivotPoint;
			item->extent = item->box.GetExtent() * 2.0f;


			fit = true;
			for (int k = 0; k < items.Num(); k++)
			{
				//UE_LOG(LogEnhancedBox, Warning,
				//	TEXT("[ item : O - %s E - %s ] ///// [ Items(j) : O - %s E - %s ]"),
				//	*item->origin.ToString(),
				//	*item->extent.ToString(),
				//	*items[j]->origin.ToString(),
				//	*items[j]->extent.ToString());

				if (item->box.Intersect(items[k]->box))
				{
					//UE_LOG(LogEnhancedBox, Error, L"Intersection found !");
					fit = false;
					break;
				}
			}

			//Capacity Check (currently ignored)
			if (fit)
			{
				//if (this->GetCurrentCapacity() + item->weight > maxCapacity)
				//{
				//	fit = false;
				//	return fit;
				//}
				return fit;
			}

			if (!fit)
			{
				item->box = originalBox;
				item->extent = originalExtent;
				item->origin = validPosition;
			}
			return fit;
		}
		if (!fit)
		{
			item->origin = validPosition;
			item->box = originalBox;
			item->extent = originalExtent;
		}

		return fit;



	}
	if (fit)
	{
		return fit;
	}

	return fit;


	return true;
}
#undef MIN_OFFSET

bool UOrderedPlacementBuilder::PlaceCylinder()
{
	return true;
}

bool UOrderedPlacementBuilder::PlaceCubic()
{
	return true;
}
