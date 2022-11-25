// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "EnhancedBox.h"
#include "BinPackerComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPackerComponent, Log, All)

UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class PROCEDURALGENERATIONTOOL_API UBinPackerComponent : public UBoxComponent
{
	GENERATED_BODY()
	
public:
	Packer packerInstance;
	
	URectangleBin bin;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* settings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName settingsRow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int maxItems = 99;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "AHelp")
	void ForceInitialize();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "AHelp",meta = (CallableWithoutWorldContext))
	void StartPacking();
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "AHelp")
	void TestFunction();



	void GenerateItem(FPackerItemSettings* row);

};
