// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "StageManagerActor.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnEndOverlapCheck);

UENUM()
enum EStage
{
	TUTORIAL,
	STAGE_1,
	STAGE_2,
	STAGE_3
};

UCLASS()
class GAME_API AStageManagerActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AStageManagerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Client, Reliable)
	void StageClear();

	UPROPERTY(EditAnywhere)
	int iStage = STAGE_1;

	bool bIsOnline = false;

	FOnEndOverlapCheck OnEndCheck;

private:

	UPROPERTY(EditAnywhere)
	UBoxComponent* box;
};