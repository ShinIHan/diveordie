// Fill out your copyright notice in the Description page of Project Settings.


#include "FishingNet.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
AFishingNet::AFishingNet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = box;
	box->SetBoxExtent(FVector(1020.0f, 40.0f, 600.0f));

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	mesh->SetRelativeLocation(FVector(-30.0f, 20.0f, -20.0f));
	mesh->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
	mesh->SetRelativeScale3D(FVector(4.0f, 4.0f, 4.0f));

	mesh->SetCollisionProfileName("Object");
	box->SetCollisionProfileName("OverlapAllDynamic");

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FISHINGNET_MESH(TEXT("/Game/Meshes/FishingNet.FishingNet"));
    if (FISHINGNET_MESH.Succeeded())
    {
	    mesh->SetStaticMesh(FISHINGNET_MESH.Object);
    }

	box->OnComponentBeginOverlap.AddDynamic(this, &AFishingNet::OnOverlapBegin);
}


void AFishingNet::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character)
	{
		character->Restraint(5.0f);
		Destroy();
	}
}

// Called when the game starts or when spawned
void AFishingNet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFishingNet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();

	float ArcHeight = 7.5f;
	float ArcDuration = 1.5f;

	float TimeElapsed = GetWorld()->GetTimeSeconds();
	float ZOffset = (FMath::Sin(TimeElapsed / ArcDuration * PI) * ArcHeight);

	FVector NewLocation = CurrentLocation;

	FRotator CurrentRotation = GetActorRotation();
	FRotator RotationToAdd = FRotator(0.f, 90.f, 0.f);
	FRotator NewRotation = CurrentRotation + RotationToAdd;
	FVector ForwardDirection = NewRotation.Vector();

	NewLocation -= ForwardDirection * 7.5f;
	NewLocation.Z += ZOffset;

	SetActorLocation(NewLocation);
}

