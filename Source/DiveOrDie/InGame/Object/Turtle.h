// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../game.h"
#include "GameFramework/Actor.h"
#include "Turtle.generated.h"

class TurtleCalLocationTask;

UCLASS()
class GAME_API ATurtle : public AActor
{
	GENERATED_BODY()

public:
	ATurtle();

	UPROPERTY(EditAnywhere)
	UBoxComponent* TurtleBox;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Turtlemesh;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	void TurtleCalLocationAsync(float DeltaTime);

private:
	TurtleCalLocationTask* TurtleCalculateLocationTask;
};

class TurtleCalLocationTask : public FRunnable
{
public:
	TurtleCalLocationTask(ATurtle* InTurtle, float InTurtleDeltaTime);

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	ATurtle* Turtle;
	float DeltaTime;
	FThreadSafeBool bIsRunning;
};