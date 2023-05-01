// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../game.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "GameFramework/Actor.h"
#include "SchoolOfFish.generated.h"

UCLASS()
class GAME_API ASchoolOfFish : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASchoolOfFish();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


private:
	UPROPERTY(VisibleAnywhere)
	TArray<UStaticMeshComponent*> fishArr;
	
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere)
	URotatingMovementComponent* RotatingMovementComponent;

	int _iRot;

	FVector _StartLocation;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
