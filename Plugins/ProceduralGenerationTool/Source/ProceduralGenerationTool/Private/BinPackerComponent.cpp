// Fill out your copyright notice in the Description page of Project Settings.


#include "BinPackerComponent.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogPackerComponent);

void UBinPackerComponent::StartPacking()
{
	//Reset all items
	packerInstance.bins.Reset();
	packerInstance.items.Reset();
	
	//Create bin
	URectangleBin* bin = new URectangleBin();
	bin->extent = FVector(1000, 1000, 1000);

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
	
	//TODO: Sort items (by size)

	//


	//Pack items
	for (int i = 0; i < packerInstance.items.Num(); i++)
	{
		URectangleItem* item = packerInstance.items[i];

		if (packerInstance.PackToBin(packerInstance.bins[0], item))
		{
			FActorSpawnParameters spawnParams = FActorSpawnParameters();
			spawnParams.Owner = this->GetOwner();
			spawnParams.Instigator = this->GetOwner()->GetInstigator();
			GetWorld()->SpawnActor<AActor>(item->linkedActor, item->origin, FRotator(), spawnParams);
			UE_LOG(LogPackerComponent, Display, L"Actor Spawned");
			packerInstance.bins[0]->items.Add(item);
			packerInstance.items.Remove(item);
		}
		else
		{
			packerInstance.unfitItems.Add(item);
			packerInstance.items.Remove(item);
			UE_LOG(LogPackerComponent, Warning, L"Actor not spawned");
		}
		i--;
	}
}

void UBinPackerComponent::GenerateItem(FPackerItemSettings* row)
{
	URectangleItem* newItem = new URectangleItem();
	newItem->linkedActor = row->actor;
	newItem->extent = FVector(100, 100, 100);
	newItem->origin = FVector(0, 0, 0);

	newItem->box = FBox::BuildAABB(newItem->origin, newItem->extent);


	//newItem->MakeFromStaticMesh(row->mesh, newItem->origin);
	packerInstance.items.Add(newItem);
}
