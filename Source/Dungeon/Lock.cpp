// Fill out your copyright notice in the Description page of Project Settings.


#include "Lock.h"

// Sets default values
ALock::ALock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add("Lock");

	// Adding components through C++ code instead of Blueprint 
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp")); //creating a root component that can be visible in BP components tab
	SetRootComponent(RootComp); // Create and setting a default scene component to serve as the root component

	// text is the name of the component that will be visible in the BP components tab
	TriggerComp = CreateDefaultSubobject<UTriggerBoxComponent>(TEXT("TriggerComp"));
	TriggerComp->SetupAttachment(RootComp); // Attach the trigger component to the root component


	KeyItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KeyItemMesh")); 
	KeyItemMesh->SetupAttachment(RootComp); // Attach the key item mesh to the root component
}

// Called when the game starts or when spawned
void ALock::BeginPlay()
{
	Super::BeginPlay();
	SetIsKeyPlaced(false);
}

// Called every frame
void ALock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Display, TEXT("World Time : %f"), GetWorld()->TimeSeconds); get game run time in seconds	
	
}

bool ALock::GetIsKeyPlaced()
{
	return IsKeyPlaced;
}

void ALock::SetIsKeyPlaced(bool NewIskeyPlaced)
{
		IsKeyPlaced = NewIskeyPlaced;
		TriggerComp->Trigger(IsKeyPlaced); // call the Trigger function of the TriggerBoxComponent to set the IsTriggered variable of the TriggerBoxComponent
		KeyItemMesh->SetVisibility(IsKeyPlaced); // set the visibility of the key item mesh based on the whether the key item is placed or not
}


