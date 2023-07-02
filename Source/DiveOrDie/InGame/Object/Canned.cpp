// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/Canned.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
ACanned::ACanned()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CannedBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = CannedBox;

	CannedBox->SetBoxExtent(FVector(65.0f, 65.0f, 68.0f));

	CannedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	CannedMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	CannedMesh->SetCollisionProfileName("Object");
	CannedBox->SetCollisionProfileName("OverlapAllDynamic");

	CannedMesh->SetRelativeScale3D(FVector(12.0f, 12.0f, 10.0f));
	CannedMesh->SetRelativeLocation(FVector(5.0f, 22.0f, -70.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CANNED_MESH(TEXT("/Game/Meshes/Canned.Canned"));
	if (CANNED_MESH.Succeeded())
	{
		CannedMesh->SetStaticMesh(CANNED_MESH.Object);
	}

	CannedBox->OnComponentBeginOverlap.AddDynamic(this, &ACanned::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ACanned::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACanned::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void ACanned::CannedDestroy()
{
	Destroy();
}

// Called every frame
void ACanned::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

