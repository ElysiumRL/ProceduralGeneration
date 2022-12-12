// Fill out your copyright notice in the Description page of Project Settings.


#include "PropBuilderComponentBase.h"

UPropBuilderComponentBase::UPropBuilderComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPropBuilderComponentBase::BeginPlay()
{
	Super::BeginPlay();	
}

void UPropBuilderComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UPropBuilderComponentBase::PlaceActor()
{
	return true;
}

bool UPropBuilderComponentBase::IsEditorOnly() const
{
	return true;
}