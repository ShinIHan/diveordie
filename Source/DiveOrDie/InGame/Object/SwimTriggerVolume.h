// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "SwimTriggerVolume.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API ASwimTriggerVolume : public ATriggerVolume
{
	GENERATED_BODY()
	
protected:
	UFUNCTION()
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION()
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
};
