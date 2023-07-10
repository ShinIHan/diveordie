// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../game.h"
#include "GameFramework/Actor.h"
#include "Megalodon.generated.h"

class FAsyncCalculateLocationTask;

UCLASS()
class GAME_API AMegalodon : public AActor
{
	GENERATED_BODY()

public:
	AMegalodon();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	virtual void Tick(float DeltaTime) override;
	void CalculateLocationAsync(float DeltaTime);

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* mesh;

	UPROPERTY(EditAnywhere)
	UBoxComponent* box;

	FAsyncCalculateLocationTask* CalculateLocationTask;
};

class FAsyncCalculateLocationTask : public FRunnable
{
public:
	FAsyncCalculateLocationTask(AMegalodon* InMegalodon, float InDeltaTime);

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	AMegalodon* Megalodon;
	float DeltaTime;
	FThreadSafeBool bIsRunning;
};