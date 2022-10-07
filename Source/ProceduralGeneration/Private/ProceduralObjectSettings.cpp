#include "ProceduralObjectSettings.h"

UProceduralObjectSettings::UProceduralObjectSettings()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UProceduralObjectSettings::BeginPlay()
{
	Super::BeginPlay();	
}

void UProceduralObjectSettings::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

