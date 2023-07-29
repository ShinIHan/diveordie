// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/FishingTrap.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
AFishingTrap::AFishingTrap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FishingTrapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("FishingTrapBox"));
	RootComponent = FishingTrapBox;

	FishingTrapBox->SetBoxExtent(FVector(1000.0f, 1000.0f, 14600.0f));

	FishingTrapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	FishingTrapMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	FishingTrapMesh->SetCollisionProfileName("Object");
	FishingTrapBox->SetCollisionProfileName("OverlapAllDynamic");

	FishingTrapMesh->SetRelativeScale3D(FVector(5.0f, 5.0f, 50.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FISHINGTRAP_MESH(TEXT("/Game/Meshes/FishingTrap.FishingTrap"));
	if (FISHINGTRAP_MESH.Succeeded())
	{
		FishingTrapMesh->SetStaticMesh(FISHINGTRAP_MESH.Object);
	}

	FishingTrapBox->OnComponentBeginOverlap.AddDynamic(this, &AFishingTrap::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AFishingTrap::BeginPlay()
{
	Super::BeginPlay();	
}

void AFishingTrap::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character)
	{
		character->bIsHitTrap = true;
		character->Stern(1.5f);
	}
}

// Called every frame
void AFishingTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}