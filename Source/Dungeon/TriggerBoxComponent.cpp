// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerBoxComponent.h"

UTriggerBoxComponent::UTriggerBoxComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	UE_LOG(LogTemp, Display, TEXT("Constructor Placed"));
	
}

void UTriggerBoxComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Actor) //if Actor is not a nullptr returns true alternative to (Actor != nullptr)
	{
		Mover = Actor->FindComponentByClass<UMover>();
		if (Mover) //if Mover is not a nullptr returns true alternative to (Mover != nullptr)
		{
			//Mover->ShouldMove = true;
		}
		else 
		{
			UE_LOG(LogTemp, Display, TEXT(" failed to find mover actor "));
		}
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Mover actor is nullptr"));
	}
	if (IsPressurePlate)
	{
		OnComponentBeginOverlap.AddDynamic(this,&UTriggerBoxComponent::OnOverlapBegin);
		OnComponentEndOverlap.AddDynamic(this,&UTriggerBoxComponent::OnOverlapEnd);
	}
	 
}

void UTriggerBoxComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTriggerBoxComponent::Trigger(bool newTriggerValue) // parameter newTriggerValue is passed on OnoverlapBegin and OnOverlapEnd functions to set the IsTriggered variable of the TriggerBoxComponent
{
	IsTriggered = newTriggerValue;
	if (Mover) // since it(mover) is a pointer we need to check if it is not a nullptr before using it
	{
		Mover->SetShouldMove(newTriggerValue); //accessing a private variable of mover class through a public function
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("%s doesnt have a mover to trigger"), *GetOwner()->GetActorNameOrLabel()); 
	}
}

void UTriggerBoxComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("PressurePlateActivator")) // shortcircuit evaluation, if OtherActor is nullptr it will not check the second condition
	{
		ActivatorCount++;

		if (!IsTriggered)
		{
			Trigger(true);
		}
		
	}
}

void UTriggerBoxComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->ActorHasTag("PressurePlateActivator")) // shortcircuit evaluation, if OtherActor is nullptr it will not check the second condition
	{
		ActivatorCount--;

		if (IsTriggered && (ActivatorCount==0))
		{
			Trigger(false);
		}
		
	}
}

