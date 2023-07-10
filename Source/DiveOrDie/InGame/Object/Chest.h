// Fill out your copyright notice in the Description page of Project Settings.w

#pragma once

#include "../game.h"
#include "GameFramework/Actor.h"
#include "Chest.generated.h"

UCLASS()
class GAME_API AChest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChest();

	UPROPERTY(EditAnywhere)
	UBoxComponent* box;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* OpenAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* CloseAnimation;
	
	FTimerHandle CloseBoxTimerHandle;

	UFUNCTION()
	void PlayOpenAnimation();

	UFUNCTION()
	void PlayCloseAnimation();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
