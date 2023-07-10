// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/FishA.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

enum class MovementState
{
	Ascending,
	Descending
};

MovementState State = MovementState::Ascending;

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
	FishMaxHeight = 20.f;
	FishMinHeight = -20.f;
	FishSpeed = 5.f;
}

// Called when the game starts or when spawned
void AFishA::BeginPlay()
{
	Super::BeginPlay();	

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
			FVector CurrentLocation = FishA->GetActorLocation();

			FRotator CurrentRotation = FishA->GetActorRotation();
			FRotator RotationToAdd = FRotator(0.f, 90.f, 0.f);
			FRotator NewRotation = CurrentRotation + RotationToAdd;
			FVector ForwardDirection = NewRotation.Vector();

			FVector NewLocation = CurrentLocation;

			NewLocation += ForwardDirection * 20.f;

			switch (State)
			{
			case MovementState::Ascending:
				NewLocation.Z += FishA->FishSpeed;
				if (NewLocation.Z >= FishA->FishMaxHeight)
				{
					State = MovementState::Descending;
				}
				break;
			case MovementState::Descending:
				NewLocation.Z -= FishA->FishSpeed;
				if (NewLocation.Z <= FishA->FishMinHeight)
				{
					State = MovementState::Ascending;
				}
				break;
			}

			AsyncTask(ENamedThreads::GameThread, [this, NewLocation]()
				{
					if (FishA && FishA->IsValidLowLevel())
					{
						FishA->SetActorLocation(NewLocation);
					}
				});

			FPlatformProcess::Sleep(0.05f);
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