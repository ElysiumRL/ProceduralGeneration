// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsExporterImporter.h"
#include "Misc/FileHelper.h"
#define PRINT_STR TEXT("%s")


void FSettingsExporterImporter::ExportToCSV(const TCHAR* filename, UDataTable* table)
{
	FString str = table->GetTableAsString();
	UE_LOG(LogTemp,Warning,PRINT_STR,*str)
	FFileHelper::SaveStringToFile(str, filename);
}

void FSettingsExporterImporter::ImportFromCSV(const TCHAR* filename,UDataTable* table)
{
	FString str;
	FFileHelper::LoadFileToString(str, filename);
	UE_LOG(LogTemp, Warning, PRINT_STR, *str);
	TArray<FString> errors = table->CreateTableFromCSVString(str);

	for (int i = 0; i < errors.Num(); i++)
	{
		UE_LOG(LogTemp, Error, TEXT("Importer : %s"), *errors[i]);
	}



}
