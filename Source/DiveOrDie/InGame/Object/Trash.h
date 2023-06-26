// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../game.h"
#include "GameFramework/Actor.h"
#include "Trash.generated.h"

UCLASS()
class GAME_API ATrash : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATrash();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int PointValue = 10;

	UPROPERTY(EditAnywhere)
		UCapsuleComponent* capsule;

	UPROPERTY(EditAnywhere)
		URotatingMovementComponent* movement;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* mesh;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
