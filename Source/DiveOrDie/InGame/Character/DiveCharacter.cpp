// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveCharacter.h"

#include "DiveCharacterAnimInstance.h"
#include "DivePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "DiveOrDie/Core/DiveGameMode.h"
#include "DiveOrDie/Core/DiveGameInstance.h"
#include "DiveOrDie/Core/DiveGameState.h"
#include "DiveOrDie/InGame/Character/DiveCharacterAnimInstance.h"
#include "DiveOrDie/InGame/UI/DiveCharacterWidget.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ADiveCharacter::ADiveCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Character"));

	BaseTurnRate = 40.0f;
	BaseLookUpRate = 40.0f;

	_fMaxHp = 400.0f;
	_fCurrentHp = _fMaxHp;
	_fMaxOxygen = 500.0f;
	_fCurrentOxygen = _fMaxOxygen;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 450.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->MaxSwimSpeed = 1000.0f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxOutOfWaterStepHeight = 0.0f;
	GetCharacterMovement()->OutofWaterZ = 0.0f;
	GetCharacterMovement()->JumpOutOfWaterPitch = -1.0f;
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SOLDIER_SK(TEXT("/Game/Meshes/Soldier/Soldier_T_Pose_.Soldier_T_Pose_"));
    if (SOLDIER_SK.Succeeded())
    {
	    GetMesh()->SetSkeletalMesh(SOLDIER_SK.Object);
    }

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> SOLDIER_ANIM(TEXT("/Game/Blueprints/DiveCharacterAnimBP.DiveCharacterAnimBP_C"));
	if (SOLDIER_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(SOLDIER_ANIM.Class);
	}

	static ConstructorHelpers::FClassFinder<UDiveCharacterWidget> CHARACTER_WG(TEXT("/Game/Blueprints/HUD_Main.HUD_Main_C"));
    if (CHARACTER_WG.Succeeded())
    {
	    DiveCharacter_WGBP = CHARACTER_WG.Class;
    }

	static ConstructorHelpers::FClassFinder<UUserWidget> PAUSEMENU_WG(TEXT("/Game/Blueprints/PauseMenu_WGBP.PauseMenu_WGBP_C"));
	if (PAUSEMENU_WG.Succeeded())
	{
		PauseMenu_WGBP = PAUSEMENU_WG.Class;
	}	

	static ConstructorHelpers::FObjectFinder<USoundCue> SwimAsset(TEXT("/Game/Sounds/Swimming_Cue.Swimming_Cue"));
	if (SwimAsset.Succeeded())
	{
		SwimCue = SwimAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> ElectronicAsset(TEXT("/Game/Sounds/Electric_Cue.Electric_Cue"));
	if (ElectronicAsset.Succeeded())
	{
		ElectronicCue = ElectronicAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> DamageAsset(TEXT("/Game/Sounds/Damage_Cue.Damage_Cue"));
	if (DamageAsset.Succeeded())
	{
		DamageCue = DamageAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundWave> GoldRingAsset(TEXT("/Game/Sounds/MP_Ding.MP_Ding"));
	if (GoldRingAsset.Succeeded())
	{
		GoldRingWave = GoldRingAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundWave> DieCharcterAsset(TEXT("/Game/Sounds/MP_Blood_Squirts.MP_Blood_Squirts"));
	if (DieCharcterAsset.Succeeded())
	{
		DieCharcterWave = DieCharcterAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ExplosionSystemAsset(TEXT("/Game/VFX/explosion.explosion"));
	if (ExplosionSystemAsset.Succeeded())
	{
		ExplosionSystem = ExplosionSystemAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> LightingSystemAsset(TEXT("/Game/VFX/lighting.lighting"));
	if (LightingSystemAsset.Succeeded())
	{
		LightingSystem = LightingSystemAsset.Object;
	}

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	AudioComponent->SetupAttachment(GetMesh());
}

void ADiveCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADiveCharacter, _bOnRestraint)
}

void ADiveCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
}

void ADiveCharacter::UnPossessed()
{
	Super::UnPossessed();

	if (DiveCharacter_WG)
	{
		UWidgetLayoutLibrary::RemoveAllWidgets(this);
	}
}

void ADiveCharacter::SetEnableInput(bool canMove, bool canTurn)
{
	_bCanMove = canMove;
	_bCanTurn = canTurn;
}

bool ADiveCharacter::OnMove()
{
	return _bOnMove;
}

float ADiveCharacter::GetMaxHP()
{
	return _fMaxHp;
}

float ADiveCharacter::GetCurrentHP()
{
	return _fCurrentHp;
}

float ADiveCharacter::GetMaxOxygen()
{
	return _fMaxOxygen;
}

float ADiveCharacter::GetCurrentOxygen()
{
	return _fCurrentOxygen;
}

void ADiveCharacter::GamePause()
{
	UDiveGameInstance* GameInstance = Cast<UDiveGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		if (!GameInstance->bIsOnline)
		{
			//LOG_SCREEN("Pause Game");
			if (PauseMenu_WGBP) PauseMenu_WG = CreateWidget(GetWorld(), PauseMenu_WGBP);
			
			if (PauseMenu_WG)
			{
				if (!PauseMenu_WG->IsInViewport())
				{
					PauseMenu_WG->AddToViewport();
					APlayerController* PlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
					if (PlayerController)
					{
						PlayerController->SetPause(true);
						PlayerController->SetShowMouseCursor(true);
					}
				}
			}
			
		}
	}
}

void ADiveCharacter::Interaction()
{
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * 200.0f,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel3,
		FCollisionShape::MakeSphere(50.0f),
		Params);

// #if ENABLE_DRAW_DEBUG
//
// 	FVector TraceVec = GetActorForwardVector() * 500.0f;
// 	FVector Center = GetActorLocation() + TraceVec * 0.5f;
// 	float HalfHeight = 500.0f * 0.5f + 50.0f;
// 	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
// 	FColor DrawColor = bResult ? FColor::Green :FColor::Red;
// 	float DebugLifeTime = 5.0f;
//
// 	DrawDebugCapsule(GetWorld(),
// 		Center,
// 		HalfHeight,
// 		50.0f,
// 		CapsuleRot,
// 		DrawColor,
// 		false,
// 		DebugLifeTime);
//
// #endif

	if (bResult)
	{
		for (auto Hit : HitResults)
		{
			if (Hit.GetActor())
			{
				FDamageEvent DamageEvent;
				ADiveCharacter* DiveCharacter = Cast<ADiveCharacter>(Hit.GetActor());
				if (!DiveCharacter) continue;
				//LOG_SCREEN("Trace Character : %s", *DiveCharacter->GetName());

				ServerRestraintEnd(DiveCharacter);
			}
		}
	}
	
}

void ADiveCharacter::OxygenConsume()
{
	if ((FVector::Dist(GetActorLocation(), FVector(GetActorLocation().X, GetActorLocation().Y, _WaterBodyPos.Z)) < 100.0f))
	{
		_fCurrentOxygen = FMath::Clamp(_fCurrentOxygen + 50.0f, 0.0f, _fMaxOxygen);
		return;
	}

	if (_fCurrentOxygen - 10.f <= 0.f)
	{
		_fCurrentOxygen = 0;
	}
	else
	{
		_fCurrentOxygen -= 10.f;
	}

	if (_fCurrentOxygen <= 0.0f)
	{
		ReceiveAnyDamage(300.0f);
	}
}

void ADiveCharacter::StartSwim(FVector waterBodyPos)
{
	_WaterBodyPos = waterBodyPos;
	
	AudioComponent->SetSound(SwimCue);
	AudioComponent->Play();

	GetWorld()->GetTimerManager().SetTimer(OxygenTimer, this, &ADiveCharacter::OxygenConsume, 1.0f, true);

	GetCharacterMovement()->RotationRate = FRotator(360.0f, 360.0f, 360.0f);
}

void ADiveCharacter::StopSwim()
{
	AudioComponent->Stop();

	GetWorld()->GetTimerManager().ClearTimer(OxygenTimer);
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
	SetActorRotation(FRotator(0.0f, GetActorRotation().Yaw, 0.0f));
}

int ADiveCharacter::GetDepth()
{
	_iDepth = int(FVector::Dist(GetActorLocation(), FVector(GetActorLocation().X, GetActorLocation().Y, _WaterBodyPos.Z))) / 100;
	
	return GetCharacterMovement()->IsSwimming() ? _iDepth : 0;
}

void ADiveCharacter::UpdateScore(int Points)
{
	ADiveGameState* GameState = Cast<ADiveGameState>(GetWorld()->GetGameState());

	GameState->iScore += Points;

	if (Points > 0)
	{
		USoundWave* Sound = GoldRingWave;
		FVector SoundLocation = GetActorLocation();
		UGameplayStatics::PlaySoundAtLocation(this, Sound, SoundLocation, FRotator::ZeroRotator, 1.f, 1.f, 0.f, nullptr, nullptr, this);
		Points = 0;
	}
}


void ADiveCharacter::ReceiveAnyDamage(float damage)
{
	UDiveGameInstance* DiveGameInstance = Cast<UDiveGameInstance>(GetWorld()->GetGameInstance());

	if(damage == 25.f)
	{
		SetOnFishTrue();
	}
	else if (damage == 150.f)
	{
		UNiagaraSystem* NiagaraSystemAsset = ExplosionSystem; // 액터1에 대한 파티클 시스템의 경로와 파일명을 지정해야 합니다.
		if (NiagaraSystemAsset)
		{
			NiagaraSystem2 = NewObject<UNiagaraComponent>(this);
			NiagaraSystem2->SetAsset(NiagaraSystemAsset);
			NiagaraSystem2->RegisterComponent();
			NiagaraSystem2->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
	else if (damage == 200.f)
	{
		SetOnBulletTrue();
		UNiagaraSystem* NiagaraSystemAsset = ExplosionSystem;
		if (NiagaraSystemAsset)
		{
			NiagaraSystem2 = NewObject<UNiagaraComponent>(this);
			NiagaraSystem2->SetAsset(NiagaraSystemAsset);
			NiagaraSystem2->RegisterComponent();
			NiagaraSystem2->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		}
	}

	if (DiveGameInstance)
	{
		if (DiveGameInstance->GetDifficulty() == 0) damage *= 0.8f;
		else if (DiveGameInstance->GetDifficulty() == 2) damage *= 1.2f; 
	}

	if (_fCurrentHp - damage <= 0.f)
	{
		_fCurrentHp = 0;
	}
	else
	{
		_fCurrentHp -= damage;
		USoundCue* Sound = DamageCue;

		FVector SoundLocation = GetActorLocation();
		UGameplayStatics::PlaySoundAtLocation(this, Sound, SoundLocation, FRotator::ZeroRotator, 1.f, 1.f, 0.f, nullptr, nullptr, this);
	}

	if (_fCurrentHp <= 0.0f)
	{
		Die();
	}
}

void ADiveCharacter::Heal(float amount)
{
	if (_fCurrentHp >= 400.f)
	{
		_fCurrentHp = 400.f;
	}
	else
	{
		_fCurrentHp += amount;
	}
}

void ADiveCharacter::Restraint(float time)
{
	if (_bOnRestraint) return;

	//LOG_SCREEN("Restraint!");
	_bOnRestraint = true;
	DiveCharacterAnim->bOnNet = true;

	GetMovementComponent()->StopMovementImmediately();
	
	SetEnableInput(false);
	
	GetWorld()->GetTimerManager().SetTimer(RestraintTimer, this, &ADiveCharacter::RestraintEnd, time, false);
}

void ADiveCharacter::RestraintEnd()
{
	_bOnRestraint = false;
	DiveCharacterAnim->bOnNet = false;
	GetWorld()->GetTimerManager().ClearTimer(RestraintTimer);
	//LOG_SCREEN("Restraint End!");
	SetEnableInput();
}

void ADiveCharacter::ServerRestraintEnd_Implementation(ADiveCharacter* DiveCharacter)
{
	DiveCharacter->RestraintEnd();
}

bool ADiveCharacter::GetRestraint()
{
	return _bOnRestraint;
}

void ADiveCharacter::Stern(float time)
{
	if (_bOnStern) return;

	//LOG_SCREEN("Stern!");
	_bOnStern = true;
	DiveCharacterAnim->bOnJelly = true;

	USoundCue* Sound = ElectronicCue;

	FVector SoundLocation = GetActorLocation();
	UGameplayStatics::PlaySoundAtLocation(this, Sound, SoundLocation, FRotator::ZeroRotator, 1.f, 1.f, 0.f, nullptr, nullptr, this);
	
	UNiagaraSystem* NiagaraSystemAsset = LightingSystem; 
	if (NiagaraSystemAsset)
	{
		NiagaraSystem1 = NewObject<UNiagaraComponent>(this);
		NiagaraSystem1->SetAsset(NiagaraSystemAsset);
		NiagaraSystem1->RegisterComponent();
		NiagaraSystem1->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}
	
	GetMovementComponent()->StopMovementImmediately();

	SetEnableInput(false, false);

	GetWorld()->GetTimerManager().SetTimer(SternTimer, this, &ADiveCharacter::SternEnd, time, false);
}

void ADiveCharacter::SternEnd()
{
	_bOnStern = false;
	DiveCharacterAnim->bOnJelly = false;

	//LOG_SCREEN("Stern End!");

	SetEnableInput();
}

void ADiveCharacter::SlowDown(float time)
{
	if (_bOnSlowDown) return;

	//LOG_SCREEN("SlowDown!");
	_bOnSlowDown = true;

	GetCharacterMovement()->MaxSwimSpeed *= 0.5f;
	GetCharacterMovement()->MaxWalkSpeed *= 0.5f;

	GetWorld()->GetTimerManager().SetTimer(SlowDownTimer, this, &ADiveCharacter::SlowDownEnd, time, false);
}

void ADiveCharacter::SlowDownEnd()
{
	_bOnSlowDown = false;

	//LOG_SCREEN("SlowDown End!");
	
	GetCharacterMovement()->MaxSwimSpeed *= 2.0f;
	GetCharacterMovement()->MaxWalkSpeed *= 2.0f;
}

void ADiveCharacter::OnRep_Restraint()
{
	RestraintEnd();
}

// Called when the game starts or when spawned
void ADiveCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ADiveCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	DiveCharacterAnim = Cast<UDiveCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	if (DiveCharacterAnim != nullptr)
	{
		DiveCharacterAnim->OnDieCheck.AddUObject(this, &ADiveCharacter::DieEnd);
	}
}

void ADiveCharacter::SetOnFishTrue()
{
	FTimerHandle FishTimerHandle;

	DiveCharacterAnim->bOnFish = true;

	GetWorldTimerManager().SetTimer(FishTimerHandle, this, &ADiveCharacter::SetOnFishFalse, 0.65f, false);
}

void ADiveCharacter::SetOnFishFalse()
{
	DiveCharacterAnim->bOnFish = false;
}

void ADiveCharacter::SetOnBulletTrue()
{
	FTimerHandle BulletTimerHandle;

	DiveCharacterAnim->bOnBullet = true;

	GetWorldTimerManager().SetTimer(BulletTimerHandle, this, &ADiveCharacter::SetOnBulletFalse, 0.5f, false);
}

void ADiveCharacter::SetOnBulletFalse()
{
	DiveCharacterAnim->bOnBullet = false;
}

void ADiveCharacter::MoveForward(float Value)
{
	if (!_bCanMove) return;
	
	if ((Controller != NULL) && (Value != 0.0f))
	{
		if (GetCharacterMovement()->IsSwimming())
		{
			AddMovementInput(FollowCamera->GetForwardVector(), Value);
		}
		else
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Value);
		}
	}
}

void ADiveCharacter::MoveRight(float Value)
{
	if (!_bCanMove || GetCharacterMovement()->IsSwimming()) return;
	
	if ((Controller != NULL) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ADiveCharacter::TurnAtRate(float Rate)
{
	if (!_bCanTurn) return;
	
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ADiveCharacter::LookUpAtRate(float Rate)
{
	if (!_bCanTurn) return;
	
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ADiveCharacter::Jump()
{
	if (!_bOnMove) return;
	
	Super::Jump();
}

void ADiveCharacter::StopJumping()
{
	if (!_bOnMove) return;
	
	Super::StopJumping();
}

void ADiveCharacter::Die()
{
	SetEnableInput(false, false);
	GetCharacterMovement()->Buoyancy = 0.0f;

	USoundWave* Sound = DieCharcterWave;

	FVector SoundLocation = GetActorLocation();
	UGameplayStatics::PlaySoundAtLocation(this, Sound, SoundLocation, FRotator::ZeroRotator, 1.f, 1.f, 0.f, nullptr, nullptr, this);

	DiveCharacterAnim->bOnDie = true;
}

void ADiveCharacter::DieEnd()
{
	OnPlayerDieCheck.Broadcast();
	Destroy();
}

// Called every frame
void ADiveCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetVelocity().Size() > 0)
		_bOnMove = true;
	else
		_bOnMove = false;

	if (!_bOnJump && GetCharacterMovement()->IsFalling())
		_bOnJump = true;
	else if (_bOnJump && !GetCharacterMovement()->IsFalling())
		_bOnJump = false;

	if (GetCharacterMovement()->IsSwimming())
	{
		if (DiveCharacterAnim->bOnNet == true)
		{

		}
		else if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::A) || GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::S) || GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::D) || GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::W))
		{
			if (GetCharacterMovement()->IsMovingOnGround())
			{
			}
			else
			{
				GetCharacterMovement()->AddInputVector(FVector(0.f, 0.f, -0.2f));
			}
		}
		else
		{
			GetCharacterMovement()->AddInputVector(FVector(0.f, 0.f, 0.2f));
		}

		depthMove = true;
	}


	if (count == 72 && Bx != NULL && By != NULL)
	{
		if (Ba == 0)
		{
			LOG_SCREEN("buttonA", Ba);
			FVector forwardVector = GetActorForwardVector();

			AddMovementInput(forwardVector, 1.f);
		}

		if (Bb == 0)
		{
			AddControllerPitchInput(1.f);
		}

		if (Bc == 0)
		{
			LOG_SCREEN("buttonC");
			FVector backwardVector = -GetActorForwardVector();
			AddMovementInput(backwardVector, 1.f);
		}

		if (Bd == 0)
		{
			AddControllerPitchInput(-1.f);
		}

		if ((float)Bx > AvRx)
		{
			LOG_SCREEN("ax : %d, AvRx : %f", Bx, AvRx);
			AddControllerYawInput(-0.5f);
		}
		else if ((float)Bx < AvRx)
		{
			LOG_SCREEN("ax : %d, AvRx : %f", Bx, AvRx);
			AddControllerYawInput(0.5f);
		}

		if (GetCharacterMovement()->IsSwimming())
		{
			if (depthMove == false)
			{
				if (Ba == 0 || Bb == 0 || Bc == 0 || Bd == 0)
				{
					if (GetCharacterMovement()->IsMovingOnGround())
					{

					}
					else
					{
						GetCharacterMovement()->AddInputVector(FVector(0.f, 0.f, -0.2f));
						depthMove = true;
					}
				}
				else
				{
					GetCharacterMovement()->AddInputVector(FVector(0.f, 0.f, 0.2f));
					depthMove = true;
				}
			}
		}
	}

	Bx = NULL, By = NULL;
	depthMove = false;
}

// Called to bind functionality to input
void ADiveCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("UpDown", this, &ADiveCharacter::MoveForward);
	InputComponent->BindAxis("LeftRight", this, &ADiveCharacter::MoveRight);
	InputComponent->BindAxis("Yaw", this, &ADiveCharacter::TurnAtRate);
	InputComponent->BindAxis("Pitch", this, &ADiveCharacter::LookUpAtRate);
	
	InputComponent->BindAction("Jump", IE_Pressed, this, &ADiveCharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ADiveCharacter::StopJumping);

	InputComponent->BindAction("Pause", IE_Pressed, this, &ADiveCharacter::GamePause);
	InputComponent->BindAction("Interaction", IE_Pressed, this, &ADiveCharacter::Interaction);
}