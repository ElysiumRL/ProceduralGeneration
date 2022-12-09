// Fill out your copyright notice in the Description page of Project Settings.


#include "PropActor.h"

// Sets default values
APropActor::APropActor()
{
	PrimaryActorTick.bCanEverTick = true;

	meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	
	meshComponent->SetStaticMesh(mesh);

}

// Called when the game starts or when spawned
void APropActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APropActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

