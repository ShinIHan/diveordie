// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include  "../game.h"
#include "Animation/AnimInstance.h"
#include "DiveCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API UDiveCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UDiveCharacterAnimInstance();
	
private:
	class ADiveCharacter* Character;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	bool bOnMove;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	bool bOnSwim;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	bool bOnJump;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	float direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	float speed;

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
