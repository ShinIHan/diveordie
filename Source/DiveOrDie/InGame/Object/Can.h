// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../game.h"
#include "GameFramework/Actor.h"
#include "Can.generated.h"

UCLASS()
class GAME_API ACan : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACan();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CanMesh;

	UPROPERTY(EditAnywhere)
	UBoxComponent* CanBox;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
