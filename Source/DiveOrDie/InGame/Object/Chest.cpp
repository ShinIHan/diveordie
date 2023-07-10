// Fill out your copyright notice in the Description page of Project Settings.


#include "Chest.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
AChest::AChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	box = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = box;

	box->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));

	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SKeletalMesh"));
	mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	mesh->SetCollisionProfileName("Object");
	box->SetCollisionProfileName("OverlapAllDynamic");

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ChestMesh(TEXT("/Game/Meshes/Chest/Chest.Chest"));
	if (ChestMesh.Succeeded())
	{
		mesh->SetSkeletalMesh(ChestMesh.Object);
	}
	static ConstructorHelpers::FObjectFinder<UAnimSequence> OpenAnim(TEXT("/Game/Meshes/Chest/Chest_Anim_Armature_A_Open.Chest_Anim_Armature_A_Open"));
	if (OpenAnim.Succeeded())
	{
		OpenAnimation = OpenAnim.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimSequence> CloseAnim(TEXT("/Game/Meshes/Chest/Chest_Anim_Armature_A_Close.Chest_Anim_Armature_A_Close"));
	if (CloseAnim.Succeeded())
	{
		CloseAnimation = CloseAnim.Object;
	}
	box->OnComponentBeginOverlap.AddDynamic(this, &AChest::OnOverlapBegin);
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
	Destroy();
}

void AChest::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character)
	{
		PlayOpenAnimation();

		GetWorld()->GetTimerManager().SetTimer(CloseBoxTimerHandle, this, &AChest::PlayCloseAnimation, 5.0f, false);
		
	}
}

// Called when the game starts or when spawned
void AChest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

