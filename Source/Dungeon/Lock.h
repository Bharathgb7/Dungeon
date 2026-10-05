// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerBoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Lock.generated.h"

UCLASS()
class DUNGEON_API ALock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
// * used because the CreateDefaultSubobject<>() returns a ptr so to store this we created it as a ptr variable
	USceneComponent* RootComp; 

	UPROPERTY(EditAnywhere)
	UTriggerBoxComponent* TriggerComp;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* KeyItemMesh;

	UPROPERTY(EditAnywhere)
	FString KeyItemName; // Name of the key item that can unlock this lock.

	bool GetIsKeyPlaced();

	void SetIsKeyPlaced(bool NewIskeyPlaced);

private:
	UPROPERTY(VisibleAnywhere)
	bool IsKeyPlaced = false; 
};
