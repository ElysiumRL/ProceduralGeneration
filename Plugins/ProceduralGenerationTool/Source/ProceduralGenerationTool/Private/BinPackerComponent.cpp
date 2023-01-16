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
	
	packerInstance.items.Sort();

	//Pack items
	for (int i = 0; i < packerInstance.items.Num(); i++)
	{
		URectangleItem* item = packerInstance.items[i];

		if (packerInstance.PackToBin_V2(packerInstance.bins[0], item))
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

void UBinPackerComponent::GenerateItem(FPackerItemSettings* row)
{
	URectangleItem* newItem = new URectangleItem();
	newItem->linkedActor = row->actor;	
	newItem->MakeFromStaticMesh(row->mesh, newItem->origin);
	newItem->box = FBox::BuildAABB(newItem->origin, newItem->extent / 2.0f);
	newItem->linkedMesh = row->mesh;
	packerInstance.items.Add(newItem);
}
