// Fill out your copyright notice in the Description page of Project Settings.


#include "BinPackerComponent.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogPackerComponent);

void UBinPackerComponent::ForceInitialize()
{
	if (settings == nullptr)
	{
		UE_LOG(LogPackerComponent, Warning, L"Settings not found ! Cancelling Packing");
	}
}

void UBinPackerComponent::StartPacking()
{
	ForceInitialize();
	packerInstance->bins.Reset();
	packerInstance->bins.Add(bin);

	for (int i = 0; i < maxItems; i++)
	{
		TArray<FPackerItemSettings*> rows;
		settings->GetAllRows<FPackerItemSettings>(L"", rows);
		
		int itemToSpawnIndex = UKismetMathLibrary::RandomIntegerInRange(0, rows.Num());

		GenerateItem(rows[itemToSpawnIndex]);
	}
}

void UBinPackerComponent::GenerateItem(FPackerItemSettings* row)
{
	URectangleItem* newItem = NewObject<URectangleItem>();

	if (row->useActor)
	{
		newItem->linkedActor = row->actor;
		newItem->extent = FVector(100, 100, 100);
	}
	if (packerInstance->PackToBin(bin, newItem))
	{
		AActor* actor = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(),newItem->origin, FRotator());
		UE_LOG(LogPackerComponent, Warning, L"Actor Spawned");
	}
}
