// Fill out your copyright notice in the Description page of Project Settings.


#include "Mover.h"
#include "Math/UnrealMathUtility.h"

// Sets default values for this component's properties
UMover::UMover()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UMover::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	StartLocation = Owner->GetActorLocation();
	SetShouldMove(ShouldMove); // set the target location to the start location in start of the game
}

// Called every frame
void UMover::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	FVector CurrentLocation = GetOwner()->GetActorLocation(); // get the current location of the actor
	ReachedTargetLocation = CurrentLocation.Equals(TargetLocation); // check if the current location is equal to the target location
		
	if (!ReachedTargetLocation) // if the current location is not equal to the target location, move the actor towards the target location
	{
		float Speed = MoveOffset.Length() / MoveTime; // calculate the speed of the actor based on the distance to move and the time to move(distance = speed * time formula)
		FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, Speed); // calculate the new location of the actor based on the current location, target location, delta time, and speed
		GetOwner()->SetActorLocation(NewLocation);
	}
	
}
bool UMover::GetShouldMove()
{
	return ShouldMove;
}

void UMover::SetShouldMove(bool NewShouldMove) // Function called by the TriggerBoxComponent to set the ShouldMove variable of the Mover component
{
	ShouldMove = NewShouldMove; 
	if (ShouldMove) //== true not included beacause ShouldMove is already a bool
	{
		// TargetLocation is  StartLocation + MoveOffset;
		TargetLocation = StartLocation + MoveOffset;
	}
	else
	{
		// TargetLocation return to StartLocation;
		TargetLocation = StartLocation;
	}
}


