// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveCharacterAnimInstance.h"
#include "DiveCharacter.h"

UDiveCharacterAnimInstance::UDiveCharacterAnimInstance()
{
	bOnMove = false;
	bOnJump = false;
	bOnSwim = false;
	bOnDie = false;
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
			bOnSwim = false;
		
		bOnJump = Character->GetMovementComponent()->IsFalling();

		//direction = CalculateDirection(Character->GetVelocity(), Character->GetActorRotation());
		direction = FVector::DotProduct(Character->GetVelocity().GetSafeNormal(), Character->GetActorForwardVector());
		speed = Character->GetVelocity().Size();
		
	}
}
