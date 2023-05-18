// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../game.h"
#include "GameFramework/Actor.h"
#include "JellyFish.generated.h"

UCLASS()
class GAME_API AJellyFish : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJellyFish();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* mesh;
	
	UPROPERTY(EditAnywhere)
	UCapsuleComponent* capsule;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 초기 위치 및 이동 관련 변수 설정
	FVector InitialLocation;
	bool bMovingUp;
	float MaxVerticalOffset;
};
