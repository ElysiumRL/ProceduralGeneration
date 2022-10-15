// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveTool.h"
#include "Engine/DataTable.h"

#include "SaveablePropertySet.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(ToolProperties, Log, All);

/**
 *  Class used to implement bases for property save.
 *  This, unlike native property set save, will save selected properties to a DataTable
 */
UCLASS(Transient)
class PROCEDURALGENERATIONTOOL_API USaveablePropertySet : public UInteractiveToolPropertySet
{
	GENERATED_BODY()


public:
	
	USaveablePropertySet();

	//Load these functions :
	//   super::Init();
	//	InitializeDataTable(dataTablePath);
	//	LoadPropertiesFromDataTable<T>();
	virtual void Init(FString dataTablePath);

	template<class T>
	void Save()
	{
		FTableRowBase table = GetSavableProperties();
		T tableCasted = Cast<T, FTableRowBase>(table);
		
		if (tableCasted)
		{
			propertiesAsTable->AddRow(TEXT("Exported"), table);
			propertiesAsTable->MarkPackageDirty();
		}
		else
		{
			UE_LOG(ToolProperties, Error, TEXT("Error with specified table type : Could not save properties"));
		}
	}
	
	template<class T>
	bool GetPropertyFromTable()
	{
		TArray<FName> rowNames = propertiesAsTable->GetRowNames();

		for (int i = 0; i < rowNames.Num(); i++)
		{
			T* table = propertiesAsTable->FindRow<T>(rowNames[i], "");

			if (table == nullptr)
			{
				continue;
			}
			Load(table);
			return true;
		}
		return false;
	}

	//Cast the parameter to the selected FTableRowBase and fill in all the properties to load on your class
	virtual void Load(FTableRowBase tableToCast);

	virtual FTableRowBase GetSavableProperties();

	virtual void DefaultProperties();

	void InitializeDataTable(FString path);

	template<class T>
	bool LoadPropertiesFromDataTable()
	{
		bool propertiesLoaded = GetPropertyFromTable<T>();
		if (!propertiesLoaded)
		{
			UE_LOG(ToolProperties, Warning, TEXT("Loading : Properties not loaded : using default properties"));
			DefaultProperties();
		}
		else
		{
			UE_LOG(ToolProperties, Verbose, TEXT("Loading : Properties Successfully loaded"));
		}
	}

protected:

	FString dataTablePath;

	UDataTable* propertiesAsTable;

};
