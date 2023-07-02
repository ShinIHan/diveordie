// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../game.h"
#include "GameFramework/Character.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "DiveCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FPlayerDelegte);

class DivePlayerController;
UCLASS()
class GAME_API ADiveCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADiveCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;

	void SetEnableInput(bool canMove = true, bool canTurn = true);

	FPlayerDelegte OnPlayerDieCheck;

	bool depthMove = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Instance, meta = (AllowPrivateAccess = true))
	bool bIsUnderwater = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Instance, meta = (AllowPrivateAccess = true))
	bool bCanJump = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Instance, meta = (AllowPrivateAccess = true))
	bool bIsZKey = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Instance, meta = (AllowPrivateAccess = true))
	float bIsZKeyTime = 0.0f;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Instance, meta = (AllowPrivateAccess = true))
	bool _bCanMove = true;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Instance, meta = (AllowPrivateAccess = true))
	bool _bCanTurn = true;
	

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	bool OnMove();

	UFUNCTION(BlueprintCallable)
	float GetMaxHP();

	UFUNCTION(BlueprintCallable)
	float GetCurrentHP();

	UFUNCTION(BlueprintCallable)
	float GetMaxOxygen();

	UFUNCTION(BlueprintCallable)
	float GetCurrentOxygen();


private:
	void GamePause();

	void Interaction();

	bool _bGamePause = false;
	
	class UUserWidget* PauseMenu_WG = nullptr;
	
	TSubclassOf<UUserWidget> PauseMenu_WGBP = nullptr;
	
	void OxygenConsume();

	FTimerHandle OxygenTimer;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool _bOnMove = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool _bOnJump = false;
	
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particles", meta = (AllowPrivateAccess = true))
	UParticleSystem* particles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particles", meta = (AllowPrivateAccess = true))
	UNiagaraComponent* NiagaraSystem1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particles", meta = (AllowPrivateAccess = true))
	UNiagaraComponent* NiagaraSystem2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particles", meta = (AllowPrivateAccess = true))
	UNiagaraComponent* CharacterNiagaraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = true))
	UNiagaraSystem* ExplosionSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Particles", meta = (AllowPrivateAccess = true))
	UNiagaraSystem* LightingSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class UDiveCharacterWidget* DiveCharacter_WG = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<UDiveCharacterWidget> DiveCharacter_WGBP = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = true))
	UAudioComponent* AudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = true))
	USoundCue* SwimCue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = true))
	USoundCue* ElectronicCue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = true))
	USoundCue* DamageCue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = true))
	USoundCue* ShootCue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = true))
	USoundWave* GoldRingWave;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = true))
	USoundWave* DieCharcterWave;;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy Shield", meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* SphereMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy Shield", meta = (AllowPrivateAccess = true))
	UMaterialInterface* m_Dynamic;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy Shield", meta = (AllowPrivateAccess = true))
	UMaterialInstanceDynamic* MaterialInstance;

	FOnTimelineFloat DissolveInterpFunction;
	FOnTimelineEvent DissolveTimelineFinish;

	UPROPERTY()
	UTimelineComponent* DissolveTimeline;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* DissolveCurveFloat;

public:
	UFUNCTION(BlueprintCallable)
	void StartSwim(FVector waterBodyPos);

	UFUNCTION(BlueprintCallable)
	void StopSwim();

	UFUNCTION(BlueprintCallable)
	int GetDepth();
	
	UFUNCTION(BlueprintCallable)
	void UpdateScore(int Points);

	UFUNCTION(BlueprintCallable)
	void UpdateTrashCount();

	void ReceiveOxygenDamage(float damage);
	
	void ReceiveAnyDamage(float damage);

	void Heal(float amount);

	void Unbeatable();

	void beatable();

	UFUNCTION()
	void DissolveInterpReturn(float Value);

	UFUNCTION()
	void DissolveFinish();

	UFUNCTION()
	void TimelineSetting();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector _WaterBodyPos; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int _iDepth = 0;

	
public:
	void Restraint(float time);
	void RestraintEnd();

	UFUNCTION(Server, Reliable)
	void ServerRestraintEnd(ADiveCharacter* DiveCharacter);

	bool GetRestraint();
		
	void Stern(float time);
	void SternEnd();

	void SlowDown(float time);
	void SlowDownEnd();


private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Restraint, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool _bOnRestraint = false;

	UFUNCTION()
	void OnRep_Restraint();

	FTimerHandle RestraintTimer;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool _bOnStern = false;

	FTimerHandle SternTimer;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool _bOnShield = false;

	FTimerHandle ShieldTimer;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool _bOnSlowDown = false;

	FTimerHandle SlowDownTimer;


	UPROPERTY()
	class UDiveCharacterAnimInstance* DiveCharacterAnim;
	

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* FollowCamera;


protected:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	void SetOnFishTrue();

	void SetOnFishFalse();

	void SetOnBulletTrue();

	void SetOnBulletFalse();

	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	virtual void Jump() override;

	virtual void StopJumping() override;

	void Die();

	void DieEnd();

	void StartZKeyPress();

	void EndZKeyPress();
	
	void DestroyNearbyCannedActors();

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
