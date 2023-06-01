// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/SwimTriggerVolume.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

void ASwimTriggerVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	ADiveCharacter* SwimCharacter = Cast<ADiveCharacter>(OtherActor);
    if (SwimCharacter)
    {
        SwimCharacter->bIsUnderwater = true;
    }
}

void ASwimTriggerVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
    ADiveCharacter* SwimCharacter = Cast<ADiveCharacter>(OtherActor);
    if (SwimCharacter)
    {
        SwimCharacter->bIsUnderwater = false;
    }
}
