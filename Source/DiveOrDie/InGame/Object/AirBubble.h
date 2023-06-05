// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../game.h"
#include "GameFramework/Actor.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "AirBubble.generated.h"

UCLASS()
class GAME_API AAirBubble : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAirBubble();



	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Sound;*/

	/*UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);*/
	UFUNCTION()
		void OnParticleBeginOverlap(UNiagaraComponent* NiagaraComponent, int32 ParticleIndex, FHitResult HitResult);

	/*UFUNCTION()
		void OnParticleCollide(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);*/
protected:
	UPROPERTY(EditAnywhere)
		UBoxComponent* box;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
		int OxygenAmount = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particles", meta = (AllowPrivateAccess = true))
		UNiagaraComponent* NiagaraSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		USoundCue* BubbleCue;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
