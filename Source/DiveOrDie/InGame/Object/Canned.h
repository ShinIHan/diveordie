// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../game.h"
#include "GameFramework/Actor.h"
#include "Canned.generated.h"

UCLASS()
class GAME_API ACanned : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACanned();

	void CannedDestroy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CannedMesh;

	UPROPERTY(EditAnywhere)
	UBoxComponent* CannedBox;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
