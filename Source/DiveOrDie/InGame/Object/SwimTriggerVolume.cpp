// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/SwimTriggerVolume.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"
#include "Components/PrimitiveComponent.h"

void ASwimTriggerVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
    ADiveCharacter* Character = Cast<ADiveCharacter>(OtherActor);
    if (Character)
    {
        Character->bIsUnderwater = true;
        LOG_SCREEN("Input");
    }
}

void ASwimTriggerVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
    ADiveCharacter* Character = Cast<ADiveCharacter>(OtherActor);
    if (Character)
    {
        Character->bIsUnderwater = false;
        LOG_SCREEN("Output");
    }
}