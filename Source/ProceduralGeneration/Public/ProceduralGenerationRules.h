// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" 


#include "UObject/NoExportTypes.h"
#include "ProceduralGenerationRules.generated.h"

USTRUCT(BlueprintType)
struct FGenerationType : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString type;
};

UCLASS()
class PROCEDURALGENERATION_API UProceduralGenerationRules : public UObject
{
	GENERATED_BODY()
private:

protected:

public:
	FGenerationType genType;


};
