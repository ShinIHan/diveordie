// Fill out your copyright notice in the Description page of Project Settings.


#include "GoldRing.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
AGoldRing::AGoldRing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = capsule;
	capsule->SetCapsuleHalfHeight(110);
	capsule->SetCapsuleRadius(80);
	
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	movement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Rotating movement"));
	movement->RotationRate = FRotator(0, 0, 180);
	
	FScriptDelegate dele;
	dele.BindUFunction(this, "OnHit");

	mesh->OnComponentHit.Add(dele);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> GOLDENRING_MESH(TEXT("/Game/Meshes/goldring.goldring"));
    if (GOLDENRING_MESH.Succeeded())
    {
	    mesh->SetStaticMesh(GOLDENRING_MESH.Object);
    }

	capsule->OnComponentBeginOverlap.AddDynamic(this, &AGoldRing::OnOverlapBegin);	
}

void AGoldRing::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//LOG_SCREEN("Overlap GoldenRing");
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character)
	{
		character->Heal(PointValue);
		character->UpdateScore(PointValue);
		Destroy();
	}
}

// Called when the game starts or when spawned
void AGoldRing::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGoldRing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

