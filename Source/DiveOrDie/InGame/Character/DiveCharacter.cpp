// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveCharacter.h"

#include "DiveCharacterAnimInstance.h"
#include "DivePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "DiveOrDie/Core/DiveGameInstance.h"
#include "DiveOrDie/Core/DiveGameState.h"
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
	_fCurrentOxygen -= 10.0f;
	if (_fCurrentOxygen <= 0.0f)
	{
		ReceiveAnyDamage(300.0f);
	}
}

void ADiveCharacter::StartSwim(FVector waterBodyPos)
{
	_WaterBodyPos = waterBodyPos;

	GetWorld()->GetTimerManager().SetTimer(OxygenTimer, this, &ADiveCharacter::OxygenConsume, 1.0f, true);

	GetCharacterMovement()->RotationRate = FRotator(360.0f, 360.0f, 360.0f);
}

void ADiveCharacter::StopSwim()
{
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
}

void ADiveCharacter::ReceiveAnyDamage(float damage)
{
	UDiveGameInstance* DiveGameInstance = Cast<UDiveGameInstance>(GetWorld()->GetGameInstance());
	if (DiveGameInstance)
	{
		if (DiveGameInstance->GetDifficulty() == 0) damage *= 0.8f;
		else if (DiveGameInstance->GetDifficulty() == 2) damage *= 1.2f; 
	}
	_fCurrentHp -= damage;
	
	if (_fCurrentHp <= 0.0f)
	{
		Die();
	}
}

void ADiveCharacter::Restraint(float time)
{
	if (_bOnRestraint) return;

	//LOG_SCREEN("Restraint!");
	_bOnRestraint = true;

	GetMovementComponent()->StopMovementImmediately();
	
	SetEnableInput(false);
	
	GetWorld()->GetTimerManager().SetTimer(RestraintTimer, this, &ADiveCharacter::RestraintEnd, time, false);
}

void ADiveCharacter::RestraintEnd()
{
	_bOnRestraint = false;
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

	GetMovementComponent()->StopMovementImmediately();

	SetEnableInput(false, false);

	GetWorld()->GetTimerManager().SetTimer(SternTimer, this, &ADiveCharacter::SternEnd, time, false);
}

void ADiveCharacter::SternEnd()
{
	_bOnStern = false;

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



