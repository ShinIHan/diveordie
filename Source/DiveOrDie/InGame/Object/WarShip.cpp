// Fill out your copyright notice in the Description page of Project Settings.


#include "WarShip.h"

#include "WarShip_Projectile.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AWarShip::AWarShip()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = mesh;
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	buoyancy = CreateDefaultSubobject<UBuoyancyComponent>(TEXT("Buoyancy"));

	mesh->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	
	mesh->SetEnableGravity(true);
	mesh->SetSimulatePhysics(true);

	buoyancy->AddCustomPontoon(200.0f, FVector(500.0f, 410.0f, 200.0f));
	buoyancy->AddCustomPontoon(200.0f, FVector(500.0f, -390.0f, 200.0f));
	buoyancy->AddCustomPontoon(200.0f, FVector(-620.0f, 410.0f, 200.0f));
	buoyancy->AddCustomPontoon(200.0f, FVector(-620.0f, -390.0f, 200.0f));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WARSHIP_MESH(TEXT("/Game/Meshes/Submarine.Submarine"));
    if (WARSHIP_MESH.Succeeded())
    {
    	mesh->SetStaticMesh(WARSHIP_MESH.Object);
    }
}

// Called when the game starts or when spawned
void AWarShip::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADiveCharacter::StaticClass(), actors);

	for (AActor* actor : actors)
	{
		if (actor)
		{
			ADiveCharacter* Character = Cast<ADiveCharacter>(actor);
			if (!Character) continue;
			Targets.Add(Character);
		}
	}
}

void AWarShip::MoveTarget(ADiveCharacter* Target)
{
	bOnMove = true;
	
	float force = UKismetMathLibrary::MapRangeClamped(
	GetVelocity().Size(),
	0.0f, 400.0f,
	10000.0f, 0.0f);

	mesh->AddForce(mesh->GetForwardVector() * force, NAME_None, true);
}

void AWarShip::Shoot(ADiveCharacter* Target)
{
	if (!bCanShoot) return;

	bCanShoot = false;
	AWarShip_Projectile* WarshipProjectile;

	USoundBase* Sound = LoadObject<USoundBase>(nullptr, TEXT("/Game/Sounds/Cannon_Cue.Cannon_Cue"));
	FVector SoundLocation = GetActorLocation();
	UGameplayStatics::PlaySoundAtLocation(this, Sound, SoundLocation, FRotator::ZeroRotator, 1.f, 1.f, 0.f, nullptr, nullptr, this);

	FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
	WarshipProjectile = GetWorld()->SpawnActor<AWarShip_Projectile>(AWarShip_Projectile::StaticClass(), GetActorLocation(), targetRotation);
	WarshipProjectile->SetBulletStat(200.0f, 10000.0f);

	GetWorldTimerManager().SetTimer(ShootTimer, this, &AWarShip::ShootReload, 2.0f, false);
}

void AWarShip::ShootReload()
{
	bCanShoot = true;
}

// Called every frame
void AWarShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (ADiveCharacter* Target : Targets)
	{
		if (Target != nullptr)
		{
			if (Target->GetCharacterMovement()->IsSwimming() && Target->GetDepth() <= 1)
			{
				SetActorRotation(FRotator(GetActorRotation().Pitch,
					UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation()).Yaw,
					GetActorRotation().Roll));
			
				if (FVector::Dist(GetActorLocation(), Target->GetActorLocation()) < 4000.0f)
				{
					Shoot(Target);	
				}
				else
				{
					MoveTarget(Target);
				}
			}
			else
			{
				if (bOnMove)
				{
					bOnMove = false;
				}
			}
		}
	}
}

