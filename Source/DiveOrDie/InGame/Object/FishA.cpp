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

	CalculateLocationTask = nullptr;
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
	
	//FishACalLocationAsync(DeltaTime);
}

void AFishA::FishACalLocationAsync(float DeltaTime)
{
	if (!CalculateLocationTask)
	{
		CalculateLocationTask = new FishACalLocationTask(this, DeltaTime);
		FRunnableThread::Create(CalculateLocationTask, TEXT("CalculateLocationTask"));
	}
}

FishACalLocationTask::FishACalLocationTask(AFishA* InFishA, float InDeltaTime)
	: FishA(InFishA)
	, DeltaTime(InDeltaTime)
	, bIsRunning(false)
{
}

bool FishACalLocationTask::Init()
{
	bIsRunning = true;
	return true;
}

uint32 FishACalLocationTask::Run()
{
	while (bIsRunning)
	{
		if (FishA && FishA->GetWorld() && FishA->GetWorld()->IsGameWorld())
		{
			float DeltaX = FMath::Sin(FishA->GetGameTimeSinceCreation()) * 400.f;
			float DeltaZ = FMath::Sin(FishA->GetGameTimeSinceCreation() * 2.f) * 100.f;

			FVector CurrentLocation = FishA->GetActorLocation();
			FVector NewLocation = FishA->FishAInitialLocation;
			NewLocation.X += DeltaX;

			FVector ForwardDirection = FishA->GetActorForwardVector();
			FRotator RotationToAdd = FRotator(0.f, 90.f, 0.f); 
			ForwardDirection = ForwardDirection.RotateAngleAxis(RotationToAdd.Yaw, FVector::UpVector);

			NewLocation += ForwardDirection * 20.f;
			NewLocation.Z += DeltaZ;

			FRotator DirectionRotation = (NewLocation - CurrentLocation).Rotation();
			FRotator TargetRotation = FRotator(DirectionRotation.Pitch, DirectionRotation.Yaw - 90.f, DirectionRotation.Roll);

			AsyncTask(ENamedThreads::GameThread, [this, NewLocation, TargetRotation]()
				{
					if (FishA && FishA->IsValidLowLevelFast())
					{
						FishA->SetActorLocation(NewLocation);
						FishA->SetActorRotation(TargetRotation);
					}
				});

			FPlatformProcess::Sleep(0.1f);
		}
		else
		{
			break;
		}
	}

	return 0;
}

void FishACalLocationTask::Stop()
{
	bIsRunning = false;
}

void FishACalLocationTask::Exit()
{
}