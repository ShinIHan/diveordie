// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../game.h"
#include "DiveOrDie/InGame/UI/TutorialWidget.h"
#include "GameFramework/Actor.h"
#include "TutorialManagerActor.generated.h"

UCLASS()
class GAME_API ATutorialManagerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATutorialManagerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void TutorialEnd();

	FTimerHandle TutorialTimer;
	
private:
	UPROPERTY()
	TSubclassOf<UTutorialWidget> Tutorial_WGBP = nullptr;

	UPROPERTY()
	class UTutorialWidget* Tutorial_WG = nullptr;

};
