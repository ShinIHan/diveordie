// Fill out your copyright notice in the Description page of Project Settings.


#include "SeaWeeds.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
ASeaWeeds::ASeaWeeds()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = capsule;
	capsule->SetCapsuleHalfHeight(70.0f);
	capsule->SetCapsuleRadius(40.0f);

	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	mesh->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
	mesh->SetRelativeLocation(FVector(0.0f, 15.0f, -80.0f));
	
	mesh->SetCollisionProfileName("Object");
	capsule->SetCollisionProfileName("OverlapAllDynamic");

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SEAWEEDS_SK(TEXT("/Game/Meshes/Seaweed/Seaweed_Plane001.Seaweed_Plane001"));
	if (SEAWEEDS_SK.Succeeded())
	{
		mesh->SetSkeletalMesh(SEAWEEDS_SK.Object);
	}
	
	capsule->OnComponentBeginOverlap.AddDynamic(this, &ASeaWeeds::OnOverlapBegin);
}

void ASeaWeeds::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//LOG_SCREEN("Overlap SeaWeeds");
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character)
	{
		character->SlowDown(10.0f);
	}
}

// Called when the game starts or when spawned
void ASeaWeeds::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASeaWeeds::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

