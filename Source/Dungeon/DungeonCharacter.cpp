// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CollectableItem.h"
#include "Lock.h"
#include "Dungeon.h"

ADungeonCharacter::ADungeonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
}

void ADungeonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ADungeonCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ADungeonCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADungeonCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADungeonCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ADungeonCharacter::LookInput);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ADungeonCharacter::Interact);
	}
	else
	{
		UE_LOG(LogDungeon, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ADungeonCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void ADungeonCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void ADungeonCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ADungeonCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void ADungeonCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void ADungeonCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

void ADungeonCharacter::Interact()
{
	//UE_LOG(LogTemp, Display, TEXT("Interact Pressed!"));
	// Getting Camera's Start Location
	FVector Start = FirstPersonCameraComponent->GetComponentLocation();

	// Getting Facing Direction of the Camera
	FVector CameraDirection = FirstPersonCameraComponent->GetForwardVector();

	// Calculating the End Location of the Line Trace by multiplying the Camera Direction with the Determined Distance and adding it to the Start Location
	FVector End = (CameraDirection * DeterminedDistance) + Start;
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.0f);

	// creating a Sphere Collision Shape with the Determined Radius
	FCollisionShape Sphere = FCollisionShape::MakeSphere(DeterminedRadius);

	//visualizing the Sphere Collision Shape by drawing a debug sphere at the End Locations of the Line Trace
	DrawDebugSphere(GetWorld(), End, DeterminedRadius, 12, FColor::Emerald, false, 5.0f);

	/* Pass by reference example 
	// creating a new vector
	FVector MyVector = FVector(1.0f, 2.0f, 3.0f); 
	// creating a function that takes a reference Of an Myvector
	void ReferenceDemo(FVector& NewMyVector); 
	// called before modifying the vector through its reference
	UE_LOG(LogTemp, Display, TEXT("VectorBeforeRef : %s"), *MyVector.ToCompactString());
	//calling the function to modify the vector through its reference
	ReferenceDemo(MyVector);
	UE_LOG(LogTemp, Display, TEXT("VectorAfterRef : %s"), *MyVector.ToCompactString());
	// This function demonstrates how to modify a FVector passed by reference
	void ADungeonCharacter::ReferenceDemo(FVector & NewMyVector) // giving a new name to the reference of the old vector
	{
		// modifying the values of the FVector through its reference name
		NewMyVector.X = 10.0f;
		NewMyVector.Y = 20.0f;
		NewMyVector.Z = 30.0f;
	} Refer this to remember pass by reference */
	
	// we create a FHitResult variable to store the result of the sweep test and also it is a reference variable that will be modified by the SweepSingleByChannel. 
	FHitResult HitResult;

	// return value of the SweepSingleByChannel function is stored in a boolean variable HasHit. If the sweep test hits an object, HasHit will be true, otherwise it will be false.
	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult, 
		Start, End, 
		FQuat::Identity, // FQuat::Identity is used to represent no rotation
		ECC_GameTraceChannel2, // ECC_GameTraceChannel2 is a custom collision channel that you can define in your project settings.
		Sphere);

	if (HasHit)
	{
		AActor* HitActor = HitResult.GetActor();

		//Always use * to print the FString in UE_LOG, as it dereferences the FString to a const TCHAR* which is what UE_LOG expects.
		UE_LOG(LogTemp, Display, TEXT("Hit Actor : %s"), *HitActor->GetActorNameOrLabel());

		if ((HitActor->ActorHasTag("CollectableItem"))) 
		{
			// perform a cast to ACollectableItem to access its properties and methods
			ACollectableItem* CollectableItem = Cast<ACollectableItem>(HitActor); //stored as ptr since it returns as a ptr
			if (CollectableItem)
			{
				ItemList.Add(CollectableItem->ItemName); // Add the item name to the ItemList array
				CollectableItem->Destroy(); // Destroy the collectable item actor after it has been collected
			}
			
		}
		else if (HitActor->ActorHasTag("Lock"))
		{
			ALock* LockActor = Cast<ALock>(HitActor); 
			if(LockActor)
			{
				//1 - Is the Lock Empty? 
				if (!LockActor->GetIsKeyPlaced()) // check if the lock already has a key placed on it
				{
					//2 - Do we have the KeyItemname in out ItemList? Remove the key item from the ItemList if it exists
					int32 ItemRemoved =ItemList.RemoveSingle(LockActor->KeyItemName); 
					if (ItemRemoved)
					{
						UE_LOG(LogTemp, Display, TEXT("Item Removed From ItemList"));
						// set the lock's IsKeyPlaced variable to true, which will trigger the lock to open
						LockActor->SetIsKeyPlaced(true); 
					}
					else
					{
						UE_LOG(LogTemp, Display, TEXT("Item Not Found In ItemList"));
					}
				}
				else
				{
					ItemList.Add(LockActor->KeyItemName);
					LockActor->SetIsKeyPlaced(false);
				}
				
			
			}
			
		}

	}
	else {
		UE_LOG(LogTemp, Display, TEXT("No Actor Hit!"));
	}
}
