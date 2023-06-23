// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/Shield.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
AShield::AShield()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	shieldcapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("ShieldCapsule"));
	RootComponent = shieldcapsule;
	shieldcapsule->SetCapsuleHalfHeight(260);
	shieldcapsule->SetCapsuleRadius(200);

	shieldmesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldStaticMesh"));
	shieldmesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	FScriptDelegate shielddele;
	shielddele.BindUFunction(this, "OnHit");

	shieldmesh->OnComponentHit.Add(shielddele);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SHIELD_MESH(TEXT("/Game/Meshes/Shield.Shield"));
	if (SHIELD_MESH.Succeeded())
	{
		shieldmesh->SetStaticMesh(SHIELD_MESH.Object);
	}

	shieldcapsule->OnComponentBeginOverlap.AddDynamic(this, &AShield::OnOverlapBegin);
}

void AShield::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character)
	{
		character->Unbeatable();
		Destroy();	
	}
}

// Called when the game starts or when spawned
void AShield::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void AShield::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float RotationSpeed = 20.0f;

	FRotator CurrentRotation = GetActorRotation();
	FRotator NewRotation = FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw + RotationSpeed * DeltaTime, CurrentRotation.Roll);

	SetActorRotation(NewRotation);
}

