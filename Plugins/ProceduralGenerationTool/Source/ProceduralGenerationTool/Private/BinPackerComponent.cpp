// Fill out your copyright notice in the Description page of Project Settings.


#include "BinPackerComponent.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogPackerComponent);

void UBinPackerComponent::ForceInitialize()
{
	if (settings == nullptr)
	{
		UE_LOG(LogPackerComponent, Warning, L"Settings not found !");
	}
}

void UBinPackerComponent::StartPacking()
{
	bin.extent = FVector(1000, 1000, 1000);
	ForceInitialize();
	packerInstance.bins.Reset();
	packerInstance.items.Reset();
	packerInstance.bins.Add(bin);

	for (int i = 0; i < maxItems; i++)
	{
		TArray<FPackerItemSettings*> rows;
		settings->GetAllRows<FPackerItemSettings>(L"", rows);
		
		int itemToSpawnIndex = UKismetMathLibrary::RandomIntegerInRange(0, rows.Num() - 1);

		GenerateItem(rows[itemToSpawnIndex]);
	}
}

void UBinPackerComponent::TestFunction()
{
	UE_LOG(LogTemp, Warning, L"Works");
}

void UBinPackerComponent::GenerateItem(FPackerItemSettings* row)
{
	URectangleItem* newItem = new URectangleItem();

	newItem->linkedActor = row->actor;
	newItem->extent = FVector(100, 100, 100);
	//newItem->MakeFromStaticMesh(row->mesh, newItem->origin);
	bin.items.Add(newItem);

	if (packerInstance.PackToBin(&bin, newItem))
	{
		FActorSpawnParameters spawnParams = FActorSpawnParameters();
		spawnParams.Owner = this->GetOwner();
		spawnParams.Instigator = this->GetOwner()->GetInstigator();


		GetWorld()->SpawnActor<AActor>(newItem->linkedActor, newItem->origin, FRotator(), spawnParams);
		UE_LOG(LogPackerComponent, Display, L"Actor Spawned");
	}
	else
	{
		UE_LOG(LogPackerComponent, Warning, L"Actor not spawned");
	}
}
