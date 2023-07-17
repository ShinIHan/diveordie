// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../game.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "TrashBagA.generated.h"

UCLASS()
class GAME_API ATrashBagA : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrashBagA();

	void TrashBagADestroy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* TrashBagAMesh;

	UPROPERTY(EditAnywhere)
	UBoxComponent* TrashBagABox;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* WidgetComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
