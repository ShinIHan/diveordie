// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveCharacterAnimInstance.h"
#include "DiveCharacter.h"

UDiveCharacterAnimInstance::UDiveCharacterAnimInstance()
{
	bOnMove = false;
	bOnJump = false;
	bOnSwim = false;
	bOnDie = false;
	bOnFish = false;
	bOnJelly = false;
	bOnNet = false;
	bOnBullet = false;
	bOnTrash = false;
	bOnSerialAbove = false;
	bOnSerialUnder = false;
}

void UDiveCharacterAnimInstance::AnimNotify_Die()
{
	OnDieCheck.Broadcast();
}

void UDiveCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	Character = Cast<ADiveCharacter>(TryGetPawnOwner());
}

void UDiveCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (Character)
	{
		if (Character->OnMove())
			bOnMove = true;
		else
			bOnMove = false;

		if (Character->GetCharacterMovement()->IsSwimming())
			bOnSwim = true;
		else
		{
			if (Character->bIsUnderwater == true)	{	}
			else
				bOnSwim = false;
		}
	
		bOnJump = Character->GetMovementComponent()->IsFalling();

		direction = CalculateDirection(Character->GetVelocity(), Character->GetActorRotation());
		speed = Character->GetVelocity().Size();		
	}
}
