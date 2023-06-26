// Fill out your copyright notice in the Description page of Project Settings.


#include "Trash.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
ATrash::ATrash()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = capsule;
	capsule->SetCapsuleHalfHeight(60);
	capsule->SetCapsuleRadius(40);

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	movement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Rotating movement"));
	movement->RotationRate = FRotator(0, 0, 180);

	FScriptDelegate dele;
	dele.BindUFunction(this, "OnHit");
	mesh->OnComponentHit.Add(dele);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Trash_Mesh(TEXT("/Game/Meshes/Cup.Cup"));
	if (Trash_Mesh.Succeeded())
	{
		mesh->SetStaticMesh(Trash_Mesh.Object);
	}

	capsule->OnComponentBeginOverlap.AddDynamic(this, &ATrash::OnOverlapBegin);
}

void ATrash::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character)
	{
		character->UpdateTrashCount();
		Destroy();
	}
}

// Called when the game starts or when spawned
void ATrash::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATrash::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
