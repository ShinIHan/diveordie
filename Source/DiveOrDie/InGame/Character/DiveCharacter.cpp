// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveCharacter.h"
#include "DivePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "DiveOrDie/InGame/UI/DiveCharacterWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Navigation/PathFollowingComponent.h"

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
}

void ADiveCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (Cast<APlayerController>(NewController))
	{
		if (!DiveCharacter_WG)
		{
			DiveCharacter_WG = CreateWidget<UDiveCharacterWidget>(GetWorld(), DiveCharacter_WGBP);
			DiveCharacter_WG->Character = this;
			if (!DiveCharacter_WG->IsInViewport())
				DiveCharacter_WG->AddToViewport();
		}
	}
}

void ADiveCharacter::UnPossessed()
{
	Super::UnPossessed();
	if (DiveCharacter_WG)
	{
		if (DiveCharacter_WG->IsInViewport())
		{
			DiveCharacter_WG->RemoveFromParent();
			DiveCharacter_WG = nullptr;
		}
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

bool ADiveCharacter::OnSwim()
{
	return _bOnSwim;
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

void ADiveCharacter::OxygenConsume()
{
	_fCurrentOxygen -= 10.0f;
}

void ADiveCharacter::StartSwim(FVector waterBodyPos)
{
	GetPhysicsVolume()->bWaterVolume = true;
	_bOnSwim = true;

	_WaterBodyPos = waterBodyPos;

	LaunchCharacter(GetActorUpVector() * 2.0f, true, true);

	GetWorld()->GetTimerManager().SetTimer(OxygenTimer, this, &ADiveCharacter::OxygenConsume, 1.0f, true);
}

void ADiveCharacter::StopSwim()
{
	GetPhysicsVolume()->bWaterVolume = false;
	_bOnSwim = false;

	GetWorld()->GetTimerManager().ClearTimer(OxygenTimer);
}

int ADiveCharacter::GetDepth()
{
	_iDepth = int(FVector::Dist(GetActorLocation(), FVector(GetActorLocation().X, GetActorLocation().Y, _WaterBodyPos.Z))) / 100;
	
	return _bOnSwim ? _iDepth : 0;
}

void ADiveCharacter::UpdateScore(int Points)
{
	_iScore += Points;
}

void ADiveCharacter::ReceiveAnyDamage(float damage)
{
	_fCurrentHp -= damage;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), particles, GetActorTransform(), true, EPSCPoolMethod::None, true);
	if (_fCurrentHp <= 0.0f)
	{
		Destroy();
	}
}

void ADiveCharacter::Restraint(float time)
{
	if (_bOnRestraint) return;

	LOG_SCREEN("Restraint!");
	_bOnRestraint = true;

	GetMovementComponent()->StopMovementImmediately();
	
	SetEnableInput(false);
	
	GetWorld()->GetTimerManager().SetTimer(RestraintTimer, this, &ADiveCharacter::RestraintEnd, time, false);
}

void ADiveCharacter::RestraintEnd()
{
	_bOnRestraint = false;
	LOG_SCREEN("Restraint End!");
	SetEnableInput();
}

void ADiveCharacter::Stern(float time)
{
	if (_bOnStern) return;

	LOG_SCREEN("Stern!");
	_bOnStern = true;

	GetMovementComponent()->StopMovementImmediately();

	SetEnableInput(false, false);

	GetWorld()->GetTimerManager().SetTimer(SternTimer, this, &ADiveCharacter::SternEnd, time, false);
}

void ADiveCharacter::SternEnd()
{
	_bOnStern = false;

	LOG_SCREEN("Stern End!");

	SetEnableInput();
}

void ADiveCharacter::SlowDown(float time)
{
	if (_bOnSlowDown) return;

	LOG_SCREEN("SlowDown!");
	_bOnSlowDown = true;

	GetCharacterMovement()->MaxSwimSpeed *= 0.5f;
	GetCharacterMovement()->MaxWalkSpeed *= 0.5f;

	GetWorld()->GetTimerManager().SetTimer(SlowDownTimer, this, &ADiveCharacter::SlowDownEnd, time, false);
}

void ADiveCharacter::SlowDownEnd()
{
	_bOnSlowDown = false;

	LOG_SCREEN("SlowDown End!");
	
	GetCharacterMovement()->MaxSwimSpeed *= 2.0f;
	GetCharacterMovement()->MaxWalkSpeed *= 2.0f;
}

// Called when the game starts or when spawned
void ADiveCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxSwimSpeed = 700.0f;
}

void ADiveCharacter::MoveForward(float Value)
{
	if (!_bCanMove) return;
	
	if ((Controller != NULL) && (Value != 0.0f))
	{
		if (_bOnSwim)
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
	if (!_bCanMove) return;
	
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

// Called every frame
void ADiveCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (_bOnSwim) LOG_SCREEN("%d", GetDepth());
	
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
	
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	

}



