// Fill out your copyright notice in the Description page of Project Settings.


#include "Chest.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AChest::AChest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	box = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = box;

	box->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	box->SetBoxExtent(FVector(150.0f, 150.0f, 100.0f));

	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SKeletalMesh"));
	mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

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
	
	IsChestOpen = false;

}

void AChest::InteractStart()
{
	PlayOpenAnimation();
	IsChestOpen = true;
}

void AChest::InteractEnd()
{
	if (IsChestOpen)
	{
		PlayCloseAnimation();
		IsChestOpen = false;
	}
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

void AChest::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character) {
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsChestOpen)
		{
			InteractEnd();
		}
	}
}

void AChest::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PlayerController && PlayerController->IsInputKeyDown(EKeys::E) && !IsChestOpen)
		{
			InteractStart();
		}
		
	}
}

// Called when the game starts or when spawned
void AChest::BeginPlay()
{
	Super::BeginPlay();

	box->OnComponentBeginOverlap.AddDynamic(this, &AChest::OnOverlapBegin);
	box->OnComponentEndOverlap.AddDynamic(this, &AChest::OnOverlapEnd);

	USkeletalMesh* SkeletalMesh = mesh->SkeletalMesh;
	if (SkeletalMesh)
	{
		mesh->SetAnimationMode(EAnimationMode::AnimationCustomMode);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SkeletalMesh is not assigned to the Chest"));
	}
}

// Called every frame
void AChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

