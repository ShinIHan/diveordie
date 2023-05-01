// Fill out your copyright notice in the Description page of Project Settings.


#include "SchoolOfFish.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASchoolOfFish::ASchoolOfFish()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	for (int i = 0; i < 5; i++)
	{
		fishArr.Emplace(CreateDefaultSubobject<UStaticMeshComponent>(FName(FString::FromInt(i))));
	}

	RootComponent = fishArr[0];

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = false;

	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementCompoennt"));
	RotatingMovementComponent->SetUpdatedComponent(RootComponent);
	RotatingMovementComponent->RotationRate = FRotator(180.0f, 0.0f, 0.0f);

	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FISH_MESH(TEXT("/Game/Meshes/tuna.tuna"));
	if (FISH_MESH.Succeeded())
	{
		for (auto& fish : fishArr)
		{
			fish->SetCollisionProfileName("OverlapAllDynamic");
			fish->SetStaticMesh(FISH_MESH.Object);
			fish->OnComponentBeginOverlap.AddDynamic(this, &ASchoolOfFish::OnOverlapBegin);
			
			if (fish == RootComponent)
			{
				fish->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.3f));
				continue;
			}
			
			fish->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			
			fish->SetRelativeLocation(FVector(
					FMath::FRandRange(-500.0f, 500.0f),
					FMath::FRandRange(-500.0f, 500.0f),
					FMath::FRandRange(-500.0f, 500.0f)
				));
			
		}
	}
	
}

// Called when the game starts or when spawned
void ASchoolOfFish::BeginPlay()
{
	Super::BeginPlay();
	
	ProjectileMovementComponent->Velocity = GetActorForwardVector() * ProjectileMovementComponent->InitialSpeed;
	_StartLocation = GetActorLocation();
}

void ASchoolOfFish::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	LOG_SCREEN("Overlap SchoolOfFish");
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character)
	{
		character->ReceiveAnyDamage(25.0f);
	}
}

// Called every frame
void ASchoolOfFish::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_iRot == 10)
	{
		RotatingMovementComponent->RotationRate = FRotator(RotatingMovementComponent->RotationRate.Pitch * -1.0f, 0.0f, 0.0f);
		LOG_SCREEN("pitch : %f", RotatingMovementComponent->RotationRate.Pitch);
		_iRot = 0;
	}
	++_iRot;

	FRotator TargetRotator = GetActorRotation();
	FVector TargetLocation = FVector(_StartLocation.X + FMath::FRandRange(-300.0f, 300.0f),
		_StartLocation.Y + FMath::FRandRange(-300.0f, 300.0f),
		_StartLocation.Z);
	if (FVector::Dist(_StartLocation, GetActorLocation()) > FMath::FRandRange(500.0f, 1500.0f))
	{
		TargetRotator = UKismetMathLibrary::RLerp(GetActorRotation(),
			UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation),
			FMath::FRandRange(0.2f, 0.05f),
			true);
	}
	SetActorRotation(TargetRotator);

	ProjectileMovementComponent->Velocity = GetActorForwardVector() * ProjectileMovementComponent->InitialSpeed;
}

