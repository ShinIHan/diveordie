// Fill out your copyright notice in the Description page of Project Settings.

#include "JellyFish.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
AJellyFish::AJellyFish()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = capsule;
	capsule->SetCapsuleHalfHeight(80.0f);
	capsule->SetCapsuleRadius(45.0f);
	
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	mesh->SetRelativeScale3D(FVector(500.0f, 500.0f, 500.0f));
	mesh->SetRelativeLocation(FVector(-20.0f, -130.0f, -46.0f));

	mesh->SetCollisionProfileName("Object");
	capsule->SetCollisionProfileName("OverlapAllDynamic");

	static ConstructorHelpers::FObjectFinder<UStaticMesh> JELLYFISH_MESH(TEXT("/Game/Meshes/JellyFish.JellyFish"));
    if (JELLYFISH_MESH.Succeeded())
    {
	    mesh->SetStaticMesh(JELLYFISH_MESH.Object);
    }

	capsule->OnComponentBeginOverlap.AddDynamic(this, &AJellyFish::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AJellyFish::BeginPlay()
{
	Super::BeginPlay();
	
}

void AJellyFish::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//LOG_SCREEN("Overlap JellyFish");
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character)
	{
		character->Stern(3.0f);
	}
}

// Called every frame
void AJellyFish::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

