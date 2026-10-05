// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Mover.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEON_API UMover : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMover();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
	float MoveTime = 5.f;

	UPROPERTY(EditAnywhere)
	FVector MoveOffset;

	UPROPERTY(VisibleAnywhere)
	bool ReachedTargetLocation = false;

	FVector StartLocation;
	FVector TargetLocation;

	bool GetShouldMove();
	void SetShouldMove(bool NewShouldMove);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	bool ShouldMove = false; //decide whether the mover should move or not
};
