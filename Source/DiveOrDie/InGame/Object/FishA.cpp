// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/FishA.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
AFishA::AFishA()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	for (int i = 0; i < 50; i++)
	{
		FishAMesh.Emplace(CreateDefaultSubobject<UStaticMeshComponent>(FName(FString::FromInt(i))));
	}

	RootComponent = FishAMesh[0];

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FISHA_MESH(TEXT("/Game/Meshes/Fish.Fish"));
	if (FISHA_MESH.Succeeded())
	{
		for (int i = 0; i < 50; i++)
		{
			UStaticMeshComponent* Fish_A = FishAMesh[i];
			
			Fish_A->SetStaticMesh(FISHA_MESH.Object);
			Fish_A->SetCollisionProfileName("OverlapAllDynamic");
			Fish_A->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.3f));

			if (Fish_A == RootComponent)
			{
				Fish_A->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.3f));
				continue;
			}

			Fish_A->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}

// Called when the game starts or when spawned
void AFishA::BeginPlay()
{
	Super::BeginPlay();	

	FishAInitialLocation = GetActorLocation();

	for (auto& MeshComponent : FishAMesh)
	{
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		MeshComponent->SetGenerateOverlapEvents(true);
		MeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AFishA::OnOverlapBegin);
	}
}

void AFishA::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* Character = Cast<ADiveCharacter>(OtherActor);
	if (Character)
	{
		Character->ReceiveAnyDamage(10.0f);
	}
}

// Called every frame
void AFishA::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = CurrentLocation;

	float DeltaX = FMath::Sin(GetGameTimeSinceCreation()) * 400.f;
	float DeltaZ = FMath::Sin(GetGameTimeSinceCreation() * 2.f) * 100.f;

	NewLocation.X = FMath::Lerp(CurrentLocation.X, FishAInitialLocation.X + DeltaX, DeltaTime * 2.f); 
	NewLocation.Y += 20.f;
	NewLocation.Z = FishAInitialLocation.Z + DeltaZ;

	SetActorLocation(NewLocation);

	FRotator CurrentRotation = GetActorRotation();
	FRotator NewRotation = CurrentRotation;

	FVector Direction = NewLocation - CurrentLocation;
	Direction.Normalize();

	FRotator TargetRotation = Direction.Rotation();
	NewRotation.Yaw = FMath::Lerp(CurrentRotation.Yaw, TargetRotation.Yaw - 90.f, DeltaTime * 2.f); 
	NewRotation.Roll = FMath::Lerp(CurrentRotation.Roll, TargetRotation.Roll, DeltaTime * 2.f);

	SetActorRotation(NewRotation);
}