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
	ATurtle();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere)
	UBoxComponent* TurtleBox;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Turtlemesh;

public:
	virtual void Tick(float DeltaTime) override;
};