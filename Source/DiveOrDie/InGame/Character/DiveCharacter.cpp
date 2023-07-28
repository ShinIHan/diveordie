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
#include "DiveOrDie/InGame/Object/WarShip.h"
#include "DiveOrDie/InGame/Object/Can.h"
#include "DiveOrDie/InGame/Object/Canned.h"
#include "DiveOrDie/InGame/Object/Cup.h"
#include "DiveOrDie/InGame/Object/TrashBagA.h"
#include "DiveOrDie/InGame/Object/TrashBagB.h"
#include "DiveOrDie/InGame/Object/TrashBagC.h"
#include "DiveOrDie/InGame/Object/SwimTriggerVolume.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "Net/UnrealNetwork.h"
#include "Materials/MaterialInstanceDynamic.h"


// Sets default values
ADiveCharacter::ADiveCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Character"));

	BaseTurnRate = 40.0f;
	BaseLookUpRate = 40.0f;

	_fMaxHp = 500.0f;
	_fCurrentHp = _fMaxHp;
	_fMaxOxygen = 700.0f;
	_fCurrentOxygen = _fMaxOxygen;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 450.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->MaxSwimSpeed = 495.0f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxOutOfWaterStepHeight = 0.0f;
	GetCharacterMovement()->OutofWaterZ = 0.0f;
	GetCharacterMovement()->JumpOutOfWaterPitch = -1.0f;

	//Character Mesh File
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SOLDIER_SK(TEXT("/Game/Meshes/Soldier/Soldier_T_Pose_.Soldier_T_Pose_"));
    if (SOLDIER_SK.Succeeded())
    {
	    GetMesh()->SetSkeletalMesh(SOLDIER_SK.Object);
    }
	//Character Mesh Animation
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> SOLDIER_ANIM(TEXT("/Game/Blueprints/DiveCharacterAnimBP.DiveCharacterAnimBP_C"));
	if (SOLDIER_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(SOLDIER_ANIM.Class);
	}

	//HUD
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

	//Audio
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
	
	static ConstructorHelpers::FObjectFinder<USoundCue> ShootAsset(TEXT("/Game/Sounds/Cannon_Cue.Cannon_Cue"));
	if (ShootAsset.Succeeded())
	{
		ShootCue = ShootAsset.Object;
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

	//VFX
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

	//Shield Material Sphere
	SphereMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMeshAsset.Succeeded())
	{
		SphereMeshComponent->SetStaticMesh(SphereMeshAsset.Object);
	}
	SphereMeshComponent->SetWorldScale3D(FVector(2.f, 2.f, 2.f));
	SphereMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	SphereMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);

	//Shield Material
	static ConstructorHelpers::FObjectFinder<UMaterial> MAT(TEXT("/Game/VFX/MT_EnergeyShield.MT_EnergeyShield"));
	if (MAT.Succeeded())
	{
		m_Dynamic = (UMaterial*)MAT.Object;
	}

	//Shield Material Dissolve Amount Timeline
	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimelineFront"));
	DissolveInterpFunction.BindUFunction(this, FName("DissolveInterpReturn"));
	DissolveTimelineFinish.BindUFunction(this, FName("DissolveFinish"));

	//Niagara System - Bubble Effect
	CharacterNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CharacterNiagaraComponent"));
	CharacterNiagaraComponent->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemAsset(TEXT("/Game/VFX/FX_Bubble_2.FX_Bubble_2"));
	if (NiagaraSystemAsset.Succeeded())
	{
		UNiagaraSystem* FX_BubbleSystem = NiagaraSystemAsset.Object;
		CharacterNiagaraComponent->SetAsset(FX_BubbleSystem);

		CharacterNiagaraComponent->SetVariableBool("IsVisible", bIsUnderwater);
	}

	//AudioComponent
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	AudioComponent->SetupAttachment(GetMesh());

	bIsUnderwater = false;
	_bOnShield = false;
	bCanJump = true;
	bIsDashKey = false, bIsDashTime = 0.0f;
	bIsWKey = false, bIsWKeyTime = 0.0f;
	bIsZKey = false, bIsZKeyTime = 0.0f;
	NaturallyDecreaseOxygen = 10.f;
	bRandomItemOxygen = false;
	bIsSerialButtonBD = false, bIsSerialButtonBDTime = 0.0f;
	bIsBaTime = 0.0f;
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

void ADiveCharacter::SetMessage(const FString& Message)
{
	WarningMessage = Message;
	GetWorldTimerManager().SetTimer(MessageTimerHandle, this, &ADiveCharacter::ClearMessage, 3.0f, false);
}

void ADiveCharacter::ClearMessage()
{
	WarningMessage.Empty();
}

void ADiveCharacter::UpdateCurrentSelectedActor(AActor* NewSelectedActor)
{
	if (CurrentSelectedActor)
	{
		TArray<UStaticMeshComponent*> MeshComponents;
		CurrentSelectedActor->GetComponents<UStaticMeshComponent>(MeshComponents);
		for (UStaticMeshComponent* MeshComponent : MeshComponents)
		{
			MeshComponent->SetRenderCustomDepth(false);
		}
	}

	// 새로운 선택된 액터 업데이트
	CurrentSelectedActor = NewSelectedActor;

	// 새로운 선택된 액터의 외곽선 보이기
	if (CurrentSelectedActor)
	{
		TArray<UStaticMeshComponent*> MeshComponents;
		CurrentSelectedActor->GetComponents<UStaticMeshComponent>(MeshComponents);
		for (UStaticMeshComponent* MeshComponent : MeshComponents)
		{
			MeshComponent->SetRenderCustomDepth(true);
		}
	}
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

	if (bResult)
	{
		for (auto Hit : HitResults)
		{
			if (Hit.GetActor())
			{
				FDamageEvent DamageEvent;
				ADiveCharacter* DiveCharacter = Cast<ADiveCharacter>(Hit.GetActor());
				if (!DiveCharacter) continue;

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

	if (_bOnShield) return;

	if (_fCurrentOxygen - NaturallyDecreaseOxygen <= 0.f)
	{
		_fCurrentOxygen = 0;
	}
	else
	{
		_fCurrentOxygen -= NaturallyDecreaseOxygen;
	}

	if (_fCurrentOxygen <= 0.0f)
	{
		_fCurrentHp = 0.f, _fCurrentOxygen = 0.f;
		Die();
	}
}

void ADiveCharacter::StartSwim(FVector waterBodyPos)
{
	bIsUnderwater = true;
	bCanJump = false;

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

void ADiveCharacter::UpdateTrashCount()
{
	ADiveGameState* GameState = Cast<ADiveGameState>(GetWorld()->GetGameState());

	GameState->iTrash += 1;

	USoundWave* Sound = GoldRingWave;
	FVector SoundLocation = GetActorLocation();
	UGameplayStatics::PlaySoundAtLocation(this, Sound, SoundLocation, FRotator::ZeroRotator, 1.f, 1.f, 0.f, nullptr, nullptr, this);

	if (GameState->iTrash == GameState->iTotalTrash)
	{
		_fCurrentHp = _fMaxHp;
		_fCurrentOxygen = _fMaxOxygen;
		GetCharacterMovement()->MaxSwimSpeed *= 2.0f;
		GetWorldTimerManager().SetTimer(MaxSwimSpeedTimerHandle, this, &ADiveCharacter::RestoreMaxSwimSpeed, 10.0f, false);
	}
	else if (GameState->iTrash % 10 == 0)
	{
		_fMaxHp += 10.f;
		_fMaxOxygen += 10.f;
		_fCurrentHp += 10.f;
		_fCurrentOxygen += 10.f;
	}
	else if (GameState->iTrash % 5 == 0)
	{
		UpdateTrashItem();
	}
}

void ADiveCharacter::UpdateTrashItem()
{
	FRandomStream RandomStream(FMath::Rand());
	int32 RandomItem = RandomStream.RandRange(1, 5);

	if (RandomItem == 1)
	{
		_fCurrentHp = _fMaxHp;
		SetMessage("Trash collect1 : Max HP");
	}
	else if (RandomItem == 2)
	{
		_fCurrentOxygen = _fMaxOxygen;
		SetMessage("Trash collect2 : Max Oxygen");
	}
	else if (RandomItem == 3)
	{
		GetCharacterMovement()->MaxSwimSpeed *= 1.5f;
		SetMessage("Trash collect1 : Swim Speed * 1.5");
		GetWorldTimerManager().SetTimer(MaxSwimSpeedTimerHandle, this, &ADiveCharacter::RestoreMaxSwimSpeed, 10.0f, false);
	}
	else if (RandomItem == 4)
	{
		bRandomItemOxygen = true;
		NaturallyDecreaseOxygen = 0.f;
		SetMessage("Trash collect1 : No Decrease Oxygen");
		GetWorldTimerManager().SetTimer(DecreaseOxygenTimerHandle, this, &ADiveCharacter::RestoreDecreaseOxygen, 10.0f, false);
	}
}

void ADiveCharacter::RestoreMaxSwimSpeed()
{
	GetCharacterMovement()->MaxSwimSpeed /= 1.5f;
}

void ADiveCharacter::RestoreDecreaseOxygen()
{
	bRandomItemOxygen = false;
	NaturallyDecreaseOxygen = 10.f;	
}

void ADiveCharacter::ReceiveOxygenDamage(float damage)
{
	if (_bOnShield) return;

	SetOnFishTrue();

	if (_fCurrentOxygen - damage <= 0.f)
	{
		_fCurrentOxygen = 0;
	}
	else
	{
		_fCurrentOxygen -= damage;

		USoundCue* Sound = DamageCue;

		FVector SoundLocation = GetActorLocation();
		UGameplayStatics::PlaySoundAtLocation(this, Sound, SoundLocation, FRotator::ZeroRotator, 1.f, 1.f, 0.f, nullptr, nullptr, this);
	}

	if (_fCurrentOxygen <= 0.0f)
	{
		_fCurrentHp = 0.f, _fCurrentOxygen = 0.f;
		Die();
	}
}

void ADiveCharacter::ReceiveAnyDamage(float damage)
{
	if (_bOnShield) return;

	UDiveGameInstance* DiveGameInstance = Cast<UDiveGameInstance>(GetWorld()->GetGameInstance());

	if(damage == 10.f || damage == 50.f)
	{
		SetOnFishTrue();
	}
	else if (damage == 150.f)
	{
		UNiagaraSystem* NiagaraSystemAsset = ExplosionSystem; 
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
		if (DiveGameInstance->GetDifficulty() == 0) 
			damage *= 0.8f;
		else if (DiveGameInstance->GetDifficulty() == 2)
			damage *= 1.2f; 
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
		_fCurrentHp = 0.f, _fCurrentOxygen = 0.f;
		Die();
	}
}

void ADiveCharacter::Heal(float amount)
{
	if (_fCurrentHp >= _fMaxHp)
	{
		_fCurrentHp = _fMaxHp;
	}
	else
	{
		_fCurrentHp += amount;
	}
}

void ADiveCharacter::Unbeatable()
{
	_bOnShield = true;

	if (MaterialInstance)
	{
		SphereMeshComponent->SetMaterial(0, MaterialInstance);

		DissolveTimeline->Stop();
		DissolveTimeline->SetNewTime(0.f);
		DissolveTimeline->Play();
	}

	GetWorld()->GetTimerManager().SetTimer(ShieldTimer, this, &ADiveCharacter::beatable, 10, false);
}

void ADiveCharacter::beatable()
{
	_bOnShield = false;
}
void ADiveCharacter::DissolveInterpReturn(float Value)
{
	if (MaterialInstance)
	{
		MaterialInstance->SetScalarParameterValue(TEXT("DissolveAmount"), Value);

		float DissolveAmountValue;
		MaterialInstance->GetScalarParameterValue(TEXT("DissolveAmount"), DissolveAmountValue);
	}
}

void ADiveCharacter::DissolveFinish()
{
	MaterialInstance->SetScalarParameterValue(TEXT("DissolveAmount"), 4.f);
}

void ADiveCharacter::TimelineSetting()
{
	if (DissolveCurveFloat)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurveFloat, DissolveInterpFunction);
		DissolveTimeline->SetTimelineFinishedFunc(DissolveTimelineFinish);
		DissolveTimeline->SetLooping(false);
	}
}

void ADiveCharacter::Restraint(float time)
{
	if (_bOnShield) return;

	if (_bOnRestraint) return;

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
	if (_bOnShield) return;

	if (_bOnStern) return;


	SetMessage("Stern (2.5 sec)");
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

	SetEnableInput();
}

void ADiveCharacter::SlowDown(float time)
{
	if (_bOnShield) return;

	if (_bOnSlowDown) return;

	_bOnSlowDown = true;

	SetMessage("Slow (10 sec)");
	GetCharacterMovement()->MaxSwimSpeed *= 0.5f;
	GetCharacterMovement()->MaxWalkSpeed *= 0.5f;

	GetWorld()->GetTimerManager().SetTimer(SlowDownTimer, this, &ADiveCharacter::SlowDownEnd, time, false);
}

void ADiveCharacter::SlowDownEnd()
{
	_bOnSlowDown = false;
	
	GetCharacterMovement()->MaxSwimSpeed *= 2.0f;
	GetCharacterMovement()->MaxWalkSpeed *= 2.0f;
}

void ADiveCharacter::OnRep_Restraint()
{
	RestraintEnd();
}

void ADiveCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (m_Dynamic)
	{
		MaterialInstance = UMaterialInstanceDynamic::Create(m_Dynamic, this);
	}

	if (MaterialInstance)
	{
		SphereMeshComponent->SetMaterial(0, MaterialInstance);
		MaterialInstance->SetScalarParameterValue(TEXT("DissolveAmount"), 3.f);
	}

	TimelineSetting();
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
	if (!_bCanMove || DiveCharacterAnim->bOnTrash == true) return;
	
	if ((Controller != NULL) && (Value != 0.0f))
	{
		FVector NewLocation;

		if (GetCharacterMovement()->IsSwimming())
		{
			NewLocation = GetActorLocation() + FollowCamera->GetForwardVector() * Value;
		}
		else
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			NewLocation = GetActorLocation() + Direction * Value;
		}
		float DistanceToWaterBody = FVector::Dist(NewLocation, FVector(NewLocation.X, NewLocation.Y, _WaterBodyPos.Z));

		if (GetCharacterMovement()->IsSwimming())
		{
			if (DistanceToWaterBody > 1.25f)
			{
				return;
			}
		}

		AddMovementInput(NewLocation - GetActorLocation());
	}
}

void ADiveCharacter::MoveRight(float Value)
{
	if (!_bCanMove || GetCharacterMovement()->IsSwimming() || DiveCharacterAnim->bOnTrash == true) return;
	
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
	if (!_bOnMove || !bCanJump || DiveCharacterAnim->bOnTrash == true) return;
	
	if (bIsUnderwater == false)
	{
		Super::Jump();
	}
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

void ADiveCharacter::StartZKeyPress()
{
	bIsZKey = true;
	LOG_SCREEN("true");
}

void ADiveCharacter::EndZKeyPress()
{
	bIsZKey = false;
	LOG_SCREEN("false");
}

void ADiveCharacter::StartDashPress()
{
	bIsDashKey = true;
}

void ADiveCharacter::EndDashPress()
{
	bIsDashKey = false;
}

void ADiveCharacter::TurnOnNearObjectOutline()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	float ClosestDistanceSquared = 275.f * 275.f;
	AActor* SelectorActor = nullptr;

	for (AActor* Actor : OverlappingActors)
	{
		if (!IsValid(Actor))
			continue;

		// 오버랩된 액터가 조건을 만족하는 경우에만 처리
		if (Actor->IsA<ACanned>() || Actor->IsA<ACan>() || Actor->IsA<ACup>() || Actor->IsA<ATrashBagA>() || Actor->IsA<ATrashBagB>() || Actor->IsA<ATrashBagC>())
		{
			FVector CharacterLocation = GetActorLocation();
			FVector ActorLocation = Actor->GetActorLocation();
			float DistanceSquared = FVector::DistSquared(CharacterLocation, ActorLocation);

			// 가장 가까운 액터를 선택
			if (DistanceSquared < ClosestDistanceSquared)
			{
				ClosestDistanceSquared = DistanceSquared;
				SelectorActor = Actor;
			}
		}
	}

	if (CurrentSelectedActor && CurrentSelectedActor != SelectorActor)
	{
		TArray<UStaticMeshComponent*> MeshComponents;
		CurrentSelectedActor->GetComponents<UStaticMeshComponent>(MeshComponents);
		for (UStaticMeshComponent* MeshComponent : MeshComponents)
		{
			MeshComponent->SetRenderCustomDepth(false);
		}
	}

	// 가장 가까운 액터를 SelectorActor에 할당
	if (SelectorActor)
	{
		CurrentSelectedActor = SelectorActor;
		TArray<UStaticMeshComponent*> MeshComponents;
		SelectorActor->GetComponents<UStaticMeshComponent>(MeshComponents);
		for (UStaticMeshComponent* MeshComponent : MeshComponents)
		{
			MeshComponent->SetRenderCustomDepth(true);
		}
	}
}

void ADiveCharacter::TurnNearTrash()
{
	TArray<AActor*> OverlappingActors;
	FVector CharacterLocation = GetActorLocation();
	float MaxTriggerDistanceSquared = 275.f * 275.f;

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		OverlappingActors.Add(Actor);
	}

	OverlappingActors.Sort([CharacterLocation](const AActor& ActorA, const AActor& ActorB)
		{
			FVector LocationA = ActorA.GetActorLocation();
			FVector LocationB = ActorB.GetActorLocation();
			float DistanceSquaredA = FVector::DistSquared(CharacterLocation, LocationA);
			float DistanceSquaredB = FVector::DistSquared(CharacterLocation, LocationB);
			return DistanceSquaredA < DistanceSquaredB;
		});

	for (AActor* Actor : OverlappingActors)
	{
		FVector ActorLocation = Actor->GetActorLocation();
		float DistanceSquared = FVector::DistSquared(CharacterLocation, ActorLocation);

		if (DistanceSquared <= MaxTriggerDistanceSquared &&
			(Actor->IsA<ACanned>() || Actor->IsA<ACan>() || Actor->IsA<ACup>() || Actor->IsA<ATrashBagA>() || Actor->IsA<ATrashBagB>() || Actor->IsA<ATrashBagC>()))
		{
			
			SetMessage("Press \"Z\" Button to Destroy Trash");

			//외곽선 표시를 위한 CustomDepth 활성화
			TArray<UStaticMeshComponent*> MeshComponents;
			Actor->GetComponents<UStaticMeshComponent>(MeshComponents);
			for (UStaticMeshComponent* MeshComponent : MeshComponents)
			{
				MeshComponent->SetRenderCustomDepth(true);

				/*UMaterialInstanceDynamic* DynMaterial = MeshComponent->CreateDynamicMaterialInstance(0);
				if (DynMaterial)
				{
					DynMaterial->SetScalarParameterValue(TEXT("CustomDepthStencilValue"), 1.0f);
				}*/
			}

			FRotator NewRotation = (ActorLocation - CharacterLocation).Rotation();
			SetActorRotation(NewRotation);

			DiveCharacterAnim->bOnTrash = true;

			break; 
		}
		else if (DistanceSquared >= MaxTriggerDistanceSquared &&
			(Actor->IsA<ACanned>() || Actor->IsA<ACan>() || Actor->IsA<ACup>() || Actor->IsA<ATrashBagA>() || Actor->IsA<ATrashBagB>() || Actor->IsA<ATrashBagC>()))
		{
			TArray<UStaticMeshComponent*> MeshComponents;
			Actor->GetComponents<UStaticMeshComponent>(MeshComponents);
			for (UStaticMeshComponent* MeshComponent : MeshComponents)
			{
				MeshComponent->SetRenderCustomDepth(false);
			}
		}
	}

}

void ADiveCharacter::DestroyNearbyCannedActors()
{
	TArray<AActor*> OverlappingActors;
	FVector CharacterLocation = GetActorLocation();
	float MaxTriggerDistanceSquared = 275.f * 275.f;

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		OverlappingActors.Add(Actor);
	}

	OverlappingActors.Sort([CharacterLocation](const AActor& ActorA, const AActor& ActorB)
		{
			FVector LocationA = ActorA.GetActorLocation();
			FVector LocationB = ActorB.GetActorLocation();
			float DistanceSquaredA = FVector::DistSquared(CharacterLocation, LocationA);
			float DistanceSquaredB = FVector::DistSquared(CharacterLocation, LocationB);
			return DistanceSquaredA < DistanceSquaredB;
		});

	bool bDestroyedNearbyActor = false;
	for (AActor* Actor : OverlappingActors)
	{
		if (bDestroyedNearbyActor)
		{
			break;
		}

		FVector ActorLocation = Actor->GetActorLocation();
		float DistanceSquared = FVector::DistSquared(CharacterLocation, ActorLocation);
		
		if (DistanceSquared <= MaxTriggerDistanceSquared)
		{
			if (Actor->IsA<ACanned>())
			{
				ACanned* CannedActor = Cast<ACanned>(Actor);
				CannedActor->CannedDestroy();
				bDestroyedNearbyActor = true;
			}
			else if (Actor->IsA<ACan>())
			{
				ACan* CanActor = Cast<ACan>(Actor);
				CanActor->CanDestroy();
				bDestroyedNearbyActor = true;
			}
			else if (Actor->IsA<ACup>())
			{
				ACup* CupActor = Cast<ACup>(Actor);
				CupActor->CupDestroy();
				bDestroyedNearbyActor = true;
			}
			else if (Actor->IsA<ATrashBagA>())
			{
				ATrashBagA* TrashBagAActor = Cast<ATrashBagA>(Actor);
				TrashBagAActor->TrashBagADestroy();
				bDestroyedNearbyActor = true;
			}
			else if (Actor->IsA<ATrashBagB>())
			{
				ATrashBagB* TrashBagBActor = Cast<ATrashBagB>(Actor);
				TrashBagBActor->TrashBagBDestroy();
				bDestroyedNearbyActor = true;
			}
			else if (Actor->IsA<ATrashBagC>())
			{
				ATrashBagC* TrashBagCActor = Cast<ATrashBagC>(Actor);
				TrashBagCActor->TrashBagCDestroy();
				bDestroyedNearbyActor = true;
			}
		}		
	}

	if (bDestroyedNearbyActor)
	{
		UpdateTrashCount();
	}
}

void ADiveCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GetVelocity().Size() > 0)
		_bOnMove = true;
	else
		_bOnMove = false;

	if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::W) && GetCharacterMovement()->IsSwimming())
	{
		bIsWKeyTime += DeltaTime;

		if (bIsWKeyTime > 2.f)
		{
			GetCharacterMovement()->MaxSwimSpeed = 495.0f;

			if (_fCurrentHp - 5.f <= 0.f)
			{
				_fCurrentHp = 0.f, _fCurrentOxygen = 0.f, bIsWKeyTime = 0.f;
				Die();
			}
			else
			{
				bIsWKeyTime = 0.f, _fCurrentHp -= 5.f;
			}
		}
		else
		{
			GetCharacterMovement()->MaxSwimSpeed = 1350.0f;
			
			if (_fCurrentOxygen - NaturallyDecreaseOxygen <= 0.f)
			{
				_fCurrentHp = 0.f, _fCurrentOxygen = 0.f;
				Die();
			}
			else
				NaturallyDecreaseOxygen = 15.f;
		}		
	}
	else if(!GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::W) && GetCharacterMovement()->IsSwimming())
	{
		bIsWKeyTime = 0.f;
		GetCharacterMovement()->MaxSwimSpeed = 495.0f;

		if(bRandomItemOxygen == false)
			NaturallyDecreaseOxygen = 10.f;
	}
		
	if (GetCharacterMovement()->IsSwimming() && _fCurrentHp > 0.f && _fCurrentOxygen > 0.f)
	{
		FVector NextLocation = GetActorLocation() + GetActorForwardVector();

		if (FVector::Dist(GetActorLocation(), FVector(GetActorLocation().X, GetActorLocation().Y, NextLocation.Z)) > 1.25f)
		{
			return;
		}

		AddMovementInput(GetActorForwardVector(), 1.f);
		GetCharacterMovement()->AddInputVector(FVector(0.f, 0.f, -0.075f));
	}

	if (!_bOnJump && GetCharacterMovement()->IsFalling())
		_bOnJump = true;
	else if (_bOnJump && !GetCharacterMovement()->IsFalling())
		_bOnJump = false;

	TurnOnNearObjectOutline();

	if (bIsZKey == true)
	{
		TurnNearTrash();

		bIsZKeyTime += DeltaTime;
		
		if (bIsZKeyTime >= 1.f)
		{
			DestroyNearbyCannedActors();
			//bIsZKey = false;
			bIsZKeyTime = 0.0f;
			DiveCharacterAnim->bOnTrash = false;
		}
	}
	/*if (bIsZKey == true)
	{
		TurnNearTrash();

		bIsZKeyTime += DeltaTime;

		if (bIsZKeyTime >= 1.f)
		{
			DestroyNearbyCannedActors();
			bIsZKey = false;
			bIsZKeyTime = 0.0f;
			DiveCharacterAnim->bOnTrash = false;
		}
	}*/
	else
	{
		bIsZKeyTime = 0.0f;
		bIsZKey = false;
		DiveCharacterAnim->bOnTrash = false;
	}

	if (GetCharacterMovement()->IsSwimming())
	{
		if (DiveCharacterAnim->bOnNet == true || bIsZKey == true)	{	}
		else if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::Z))
		{

		}
		else if ((!GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::Z)) && (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::A) || GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::S) || GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::D) || GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::W)))
		{
			if (GetCharacterMovement()->IsMovingOnGround())	{	}
			else
			{
				if(bIsZKey == false)
					GetCharacterMovement()->AddInputVector(FVector(0.f, 0.f, -0.075f));
			}
		}
		else
		{
			if (GetCharacterMovement()->IsMovingOnGround())	{	}
			else
			{
				if ((FVector::Dist(GetActorLocation(), FVector(GetActorLocation().X, GetActorLocation().Y, _WaterBodyPos.Z)) > 2.f))
				{
					if (bIsZKey == false)
						GetCharacterMovement()->AddInputVector(FVector(0.f, 0.f, 0.075f));
				}
			}		
		}

		depthMove = true;
	}

	if (AVcount == 62 && Bx != NULL && By != NULL)
	{
		if (Ba == 0 && Bb == 0 && Bc == 0 && Bd == 0)
		{
			GamePause();
		}

		if (Ba == 1)
		{
			bIsBaTime = 0.f;
			GetCharacterMovement()->MaxSwimSpeed = 495.0f;

			if (bRandomItemOxygen == false)
				NaturallyDecreaseOxygen = 10.f;
		}

		if (Ba == 1 && Bb == 1 && Bc == 0 && Bd == 1)
		{
			TurnNearTrash();

			bIsSerialButtonBDTime += DeltaTime;

			if (bIsSerialButtonBDTime >= 1.f)
			{
				DestroyNearbyCannedActors();
				bIsSerialButtonBDTime = 0.0f;
				DiveCharacterAnim->bOnTrash = false;
			}
		}
		else
		{
			bIsSerialButtonBDTime = 0.0f;
			DiveCharacterAnim->bOnTrash = false;
		}

		if (Ba == 0 && Bb == 1 && Bc == 1 && Bd == 1)
		{
			if (!GetCharacterMovement()->IsSwimming())
			{
				FVector SerialNextLocation = GetActorLocation() + GetActorForwardVector();

				if (FVector::Dist(GetActorLocation(), FVector(GetActorLocation().X, GetActorLocation().Y, SerialNextLocation.Z)) > 1.25f)
				{
					return;
				}

				AddMovementInput(GetActorForwardVector(), 1.f);
			}	
			else if (!GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::W) && GetCharacterMovement()->IsSwimming())
			{
				bIsBaTime += DeltaTime;

				if (bIsBaTime > 2.f)
				{
					GetCharacterMovement()->MaxSwimSpeed = 495.0f;

					if (_fCurrentHp - 5.f <= 0.f)
					{
						_fCurrentHp = 0.f, _fCurrentOxygen = 0.f, bIsBaTime = 0.f;
						Die();
					}
					else
					{
						bIsBaTime = 0.f, _fCurrentHp -= 5.f;
					}
				}
				else
				{
					GetCharacterMovement()->MaxSwimSpeed = 1350.0f;

					if (_fCurrentOxygen - NaturallyDecreaseOxygen <= 0.f)
					{
						_fCurrentHp = 0.f, _fCurrentOxygen = 0.f;
						Die();
					}
					else
						NaturallyDecreaseOxygen = 15.f;
				}
			}
		}

		if (Ba == 1 && Bb == 0 && Bc == 1 && Bd == 1)
		{
			if (GetCharacterMovement()->IsSwimming())
			{
				DiveCharacterAnim->bOnSerialAbove = true;
				GetCharacterMovement()->AddInputVector(FVector(0.f, 0.f, 1.f));		
			}	
		}
		else
		{
			DiveCharacterAnim->bOnSerialAbove = false;
		}

		if (Ba == 1 && Bb == 1 && Bc == 1 && Bd == 0)
		{
			if (GetCharacterMovement()->IsSwimming())
			{
				DiveCharacterAnim->bOnSerialUnder = true;
				GetCharacterMovement()->AddInputVector(FVector(0.f, 0.f, -1.f));
			}
		}
		else
		{
			DiveCharacterAnim->bOnSerialUnder = false;
		}

		if ((float)Bx > AvRx && Ba == 0 && Bb == 1 && Bc == 0 && Bd == 1)
		{
			LOG_SCREEN("ax : %d, AvRx : %f", Bx, AvRx);
			AddControllerYawInput(-0.75f);
		}
		else if ((float)Bx < AvRx && Ba == 0 && Bb == 1 && Bc == 0 && Bd == 1)
		{
			LOG_SCREEN("ax : %d, AvRx : %f", Bx, AvRx);
			AddControllerYawInput(0.75f);
		}

		if ((float)By < AvRy && Ba == 1 && Bb == 0 && Bc == 1 && Bd == 0)
		{
			LOG_SCREEN("ax : %d, AvRx : %f", Bx, AvRx);
			DiveCharacterAnim->bOnSerialMoveL = true;
			AddMovementInput(GetActorRightVector(), -1.f);
		}
		else if ((float)By > AvRy && Ba == 1 && Bb == 0 && Bc == 1 && Bd == 0)
		{
			LOG_SCREEN("ax : %d, AvRx : %f", Bx, AvRx);
			DiveCharacterAnim->bOnSerialMoveL = false;
			AddMovementInput(GetActorRightVector(), 1.f);
		}
		else
		{
			DiveCharacterAnim->bOnSerialMoveL = false;
		}

		if (GetCharacterMovement()->IsSwimming())
		{
			if (depthMove == false)
			{
				if (Ba == 0 || Bb == 0 || Bc == 0 || Bd == 0)
				{
					if (GetCharacterMovement()->IsMovingOnGround())	{	}
					else
					{
						if (!GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::Z))
						{
							GetCharacterMovement()->AddInputVector(FVector(0.f, 0.f, -0.075f));
							depthMove = true;
						}						
					}
				}
				else
				{
					if (GetCharacterMovement()->IsMovingOnGround())	{	}
					else
					{
						if ((FVector::Dist(GetActorLocation(), FVector(GetActorLocation().X, GetActorLocation().Y, _WaterBodyPos.Z)) > 2.f))
						{
							if (!GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::Z))
							{
								GetCharacterMovement()->AddInputVector(FVector(0.f, 0.f, 0.075f));
								depthMove = true;
							}
						}
					}
				}
			}
		}
	}

	Bx = NULL, By = NULL;
	depthMove = false;

	CharacterNiagaraComponent->SetVariableBool("IsVisible", bIsUnderwater);
}

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

	InputComponent->BindAction("ZKey", IE_Pressed, this, &ADiveCharacter::StartZKeyPress);
	InputComponent->BindAction("ZKey", IE_Released, this, &ADiveCharacter::EndZKeyPress);

	InputComponent->BindAction("Dash", IE_Pressed, this, &ADiveCharacter::StartDashPress);
	InputComponent->BindAction("Dash", IE_Released, this, &ADiveCharacter::EndDashPress);
}