// Fill out your copyright notice in the Description page of Project Settings.


#include "Chest.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AChest::AChest()
{
	PrimaryActorTick.bCanEverTick = true;

	box = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = box;

	box->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	box->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));

	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SKeletalMesh"));
	mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	mesh ->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));

	mesh->SetCollisionProfileName("Object");
	box->SetCollisionProfileName("OverlapAllDynamic");

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ChestMesh(TEXT("/Game/Meshes/Chest/Chest.Chest"));
	if (ChestMesh.Succeeded())
	{
		mesh->SetSkeletalMesh(ChestMesh.Object);
	}
	static ConstructorHelpers::FObjectFinder<UAnimSequence> OpenAnim(TEXT("/Game/Meshes/Chest/Chest_OpenAnim.Chest_OpenAnim"));
	if (OpenAnim.Succeeded())
	{
		OpenAnimation = OpenAnim.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> CloseAnim(TEXT("/Game/Meshes/Chest/Chest_CloseAnim.Chest_CloseAnim"));
	if (CloseAnim.Succeeded())
	{
		CloseAnimation = CloseAnim.Object;
	}


	box->OnComponentBeginOverlap.AddDynamic(this, &AChest::OnOverlapBegin);
	box->OnComponentEndOverlap.AddDynamic(this, &AChest::OnOverlapEnd);

	bIsChestOpen = false;
}

void AChest::InteractStart()
{
	PlayOpenAnimation();
	ApplyRandomBuff();
	bIsChestOpen = true;
}

void AChest::InteractEnd()
{
	if (bIsChestOpen)
	{
		PlayCloseAnimation();
		bIsChestOpen = false;
	}
	GetWorldTimerManager().SetTimer(AnimationHandle, this, &AChest::RemoveChest, 3.0f, false);
}

void AChest::PlayOpenAnimation()
{
	if (OpenAnimation)
	{
		mesh->PlayAnimation(OpenAnimation, false);
	}
}

void AChest::PlayCloseAnimation()
{
	if (CloseAnimation)
	{
		mesh->PlayAnimation(CloseAnimation, false);
	}
}

void AChest::RemoveChest()
{
	Destroy();
}

//void AChest::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
//	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
//	if (character && mesh) // mesh가 null이 아닌지 확인
//	{
//		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
//		if (PlayerController && PlayerController->IsInputKeyDown(EKeys::E) && !bIsChestOpen)
//		{
//			InteractStart();
//		}
//	}
//}
//
//void AChest::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
//	if (character) { 
//		if (bIsChestOpen)
//		{
//			InteractEnd();
//		}
//	}
//}

void AChest::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character && !bIsChestOpen) 
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PlayerController && PlayerController->IsInputKeyDown(EKeys::Z))
		{
			InteractStart();
		}
	}
}

void AChest::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character && bIsChestOpen) 
	{
		InteractEnd();
	}
}

void AChest::ApplyRandomBuff()
{
	EBuffType RandomBuff = static_cast<EBuffType>(FMath::RandRange(1, static_cast<int32>(EBuffType::Heal)));

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		ADiveCharacter* Character = Cast<ADiveCharacter>(PlayerController->GetPawn());
		if (Character)
		{
			switch (RandomBuff)
			{
			case EBuffType::SpeedBoost:
				Character->ApplySpeedBoost();
				break;

			case EBuffType::Damage:
				Character->ApplyDamage();
				break;

			case EBuffType::Heal:
				Character->ApplyHeal();
				break;

				// 추가적인 버프 타입들에 대한 적용 로직을 추가합니다.
			default:
				break;
			}
		}
	}
	GetWorld()->GetTimerManager().SetTimer(BuffTimerHandle, this, &AChest::RemoveBuff, 10.0f, false);
}

void AChest::RemoveBuff()
{
	// 모든 플레이어 캐릭터의 버프를 제거합니다.
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		ADiveCharacter* Character = Cast<ADiveCharacter>(PlayerController->GetPawn());
		if (Character)
		{
			Character->RemoveBuff();
		}
	}
}

// Called when the game starts or when spawned
void AChest::BeginPlay()
{
	Super::BeginPlay();

	if (mesh)
	{
		USkeletalMesh* SkeletalMesh = mesh->SkeletalMesh;
		if (SkeletalMesh)
		{
			mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("SkeletalMesh is not assigned to the Chest"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Mesh component not found in the Chest actor"));
	}
	
}

// Called every frame
void AChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

