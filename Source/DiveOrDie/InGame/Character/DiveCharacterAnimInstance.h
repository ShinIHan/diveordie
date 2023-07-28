// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include  "../game.h"
#include "Animation/AnimInstance.h"
#include "DiveCharacterAnimInstance.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FDiveCharacterDelegate)

UCLASS()
class GAME_API UDiveCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UDiveCharacterAnimInstance();
	
private:
	UFUNCTION()
	void AnimNotify_Die();
	
	class ADiveCharacter* Character;

public:
	FDiveCharacterDelegate OnDieCheck;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	bool bOnMove;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	bool bOnSwim;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	bool bOnJump;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	bool bOnDie;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	bool bOnFish;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	bool bOnJelly;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	bool bOnNet;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	bool bOnBullet;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	bool bOnTrash;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	bool bOnSerialMoveL;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	bool bOnSerialMoveR;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	bool bOnSerialAbove;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	bool bOnSerialUnder;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	float direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Instance)
	float speed;

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
