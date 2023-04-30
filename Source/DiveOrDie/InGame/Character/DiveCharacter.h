// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../game.h"
#include "GameFramework/Character.h"
#include "DiveCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FPlayerDelegte);

class DivePlayerController;
UCLASS()
class GAME_API ADiveCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADiveCharacter();

	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;

	void SetEnableInput(bool canMove = true, bool canTurn = true);

	FPlayerDelegte OnPlayerDieCheck;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Instance, meta = (AllowPrivateAccess = true))
	bool _bCanMove = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Instance, meta = (AllowPrivateAccess = true))
	bool _bCanTurn = true;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	bool OnMove();

	bool OnSwim();

	UFUNCTION(BlueprintCallable)
	float GetMaxHP();

	UFUNCTION(BlueprintCallable)
	float GetCurrentHP();

	UFUNCTION(BlueprintCallable)
	float GetMaxOxygen();

	UFUNCTION(BlueprintCallable)
	float GetCurrentOxygen();

private:
	void OxygenConsume();

	FTimerHandle OxygenTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool _bOnMove = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool _bOnJump = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool _bOnSwim = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float _fMaxHp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float _fCurrentHp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float _fMaxOxygen;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float _fCurrentOxygen;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int _iScore = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UParticleSystem* particles;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class UDiveCharacterWidget* DiveCharacter_WG = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<UDiveCharacterWidget> DiveCharacter_WGBP = nullptr;

public:
	UFUNCTION(BlueprintCallable)
	void StartSwim(FVector waterBodyPos);

	UFUNCTION(BlueprintCallable)
	void StopSwim();

	UFUNCTION(BlueprintCallable)
	int GetDepth();

	UFUNCTION(BlueprintCallable)
	void UpdateScore(int Points);

	void ReceiveAnyDamage(float damage);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector _WaterBodyPos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int _iDepth = 0;


	// ----------------------- CC기 ----------------------- //
public:
	// 속박
	void Restraint(float time);

	void RestraintEnd();

	// 기절
	void Stern(float time);

	void SternEnd();

	// 둔화
	void SlowDown(float time);

	void SlowDownEnd();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool _bOnRestraint = false;

	FTimerHandle RestraintTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool _bOnStern = false;

	FTimerHandle SternTimer;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool _bOnSlowDown = false;

	FTimerHandle SlowDownTimer;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* FollowCamera;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};