// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../game.h"
#include "GameFramework/Actor.h"
#include "FishA.generated.h"

class FishACalLocationTask;

UCLASS()
class GAME_API AFishA : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFishA();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere)
	TArray<UStaticMeshComponent*> FishAMesh;

	FishACalLocationTask* CalculateLocationTask;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void FishACalLocationAsync(float DeltaTime);

	float FishMaxHeight = 20.f;
	float FishMinHeight = -20.f;
	float FishSpeed = 5.f;
};

class FishACalLocationTask : public FRunnable
{
public:
	FishACalLocationTask(AFishA* InFishA, float InDeltaTime);

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	AFishA* FishA;
	float DeltaTime;
	FThreadSafeBool bIsRunning;
};
