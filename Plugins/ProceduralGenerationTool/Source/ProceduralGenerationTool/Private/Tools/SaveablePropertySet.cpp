// Fill out your copyright notice in the Description page of Project Settings.

#include "Tools/SaveablePropertySet.h"

DEFINE_LOG_CATEGORY(ToolProperties);

USaveablePropertySet::USaveablePropertySet()
{
	propertiesAsTable = nullptr;
}

void USaveablePropertySet::Init(FString path)
{
	dataTablePath = path;
}

void USaveablePropertySet::Load(FTableRowBase tableToCast)
{

}

FTableRowBase USaveablePropertySet::GetSavableProperties()
{
	return FTableRowBase();
}

void USaveablePropertySet::DefaultProperties()
{
}

void USaveablePropertySet::InitializeDataTable(FString path)
{
	UDataTable* DT;
	FSoftObjectPath UnitDataTablePath = FSoftObjectPath(path);
	DT = Cast<UDataTable>(UnitDataTablePath.ResolveObject());
	if (DT)
	{
		propertiesAsTable = DT;
		UE_LOG(ToolProperties, Warning, TEXT("Data Table Loaded"));
	}
	else
	{
		DT = Cast<UDataTable>(UnitDataTablePath.TryLoad());
	}

	if (DT)
	{
		propertiesAsTable = DT;
		UE_LOG(ToolProperties, Warning, TEXT("Data Table Loaded"));
	}
	else
	{
		UE_LOG(ToolProperties, Warning, TEXT("Data Table Not found !"));
	}
}
