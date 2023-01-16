// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SettingsExporterImporter.h"
#include <Components/BoxComponent.h>
#include "PropBuilderComponentBase.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Abstract)
class PROCEDURALGENERATIONTOOL_API UPropBuilderComponentBase : public UBoxComponent
{
	GENERATED_BODY()

public:	
	UPropBuilderComponentBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTagSelector> TagsToInculde;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTagSelector selector;

	TArray<UEnhancedBox*> items;
	
	Packer packerInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* settings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName settingsRow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int maxItems = 250;

	UFUNCTION(BlueprintCallable, CallInEditor, meta = (CallableWithoutWorldContext))
	virtual void StartPlacing() {};
	
	UFUNCTION(BlueprintCallable, CallInEditor, meta = (CallableWithoutWorldContext))
	virtual bool PlaceActor();

	void GenerateItem(FPackerItemSettings* row)
	{
		URectangleItem* newItem = new URectangleItem();
		newItem->linkedActor = row->actor;
		newItem->MakeFromStaticMesh(row->mesh, newItem->origin);
		newItem->box = FBox::BuildAABB(newItem->origin, newItem->extent / 2.0f);
		newItem->linkedMesh = row->mesh;
		packerInstance.items.Add(newItem);
	}


public:
	bool IsEditorOnly() const override;

};