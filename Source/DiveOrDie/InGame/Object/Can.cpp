// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/Can.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
ACan::ACan()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CanBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = CanBox;

	CanBox->SetBoxExtent(FVector(75.0f, 75.0f, 80.0f));

	CanMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	CanMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	CanMesh->SetCollisionProfileName("Object");
	CanBox->SetCollisionProfileName("OverlapAllDynamic");

	CanMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 2.0f));
	CanMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -70.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CAN_MESH(TEXT("/Game/Meshes/Can.Can"));
	if (CAN_MESH.Succeeded())
	{
		CanMesh->SetStaticMesh(CAN_MESH.Object);
	}

	CanBox->OnComponentBeginOverlap.AddDynamic(this, &ACan::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ACan::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACan::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void ACan::CanDestroy()
{
	Destroy();
}

// Called every frame
void ACan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

