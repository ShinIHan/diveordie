// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../game.h"
#include "GameFramework/Actor.h"
#include "Turtle.generated.h"

UCLASS()
class GAME_API ATurtle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurtle();

	UPROPERTY(EditAnywhere)
	UBoxComponent* TurtleBox;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Turtlemesh;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector TurtleInitialLocation;
};
