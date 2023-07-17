// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../game.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "TrashBagB.generated.h"

UCLASS()
class GAME_API ATrashBagB : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrashBagB();

	void TrashBagBDestroy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* TrashBagBMesh;

	UPROPERTY(EditAnywhere)
	UBoxComponent* TrashBagBBox;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* WidgetComponent;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
