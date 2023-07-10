// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../game.h"
#include "GameFramework/Actor.h"
#include "SmallFishes.generated.h"

class SmallFishesCalLocationTask;

UCLASS()
class GAME_API ASmallFishes : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASmallFishes();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere)
	TArray<UStaticMeshComponent*> SmallFishesMesh;

	SmallFishesCalLocationTask* CalculateLocationTask;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SmallFishesCalLocationAsync(float DeltaTime);
};

class SmallFishesCalLocationTask : public FRunnable
{
public:
	SmallFishesCalLocationTask(ASmallFishes* InSmallFishes, float InDeltaTime);

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	ASmallFishes* SmallFishes;
	float DeltaTime;
	FThreadSafeBool bIsRunning;
};

