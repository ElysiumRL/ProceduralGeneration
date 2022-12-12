// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SettingsExporterImporter.h"
#include "PropBuilderComponentBase.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Abstract)
class PROCEDURALGENERATIONTOOL_API UPropBuilderComponentBase : public UActorComponent
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

	UFUNCTION(BlueprintCallable, CallInEditor, meta = (CallableWithoutWorldContext))
	virtual void StartPlacing() {};
	
	UFUNCTION(BlueprintCallable, CallInEditor, meta = (CallableWithoutWorldContext))
	virtual bool PlaceActor();


public:
	bool IsEditorOnly() const override;

};