// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/SmallFishes.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
ASmallFishes::ASmallFishes()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	for (int i = 0; i < 5; i++)
	{
		SmallFishesMesh.Emplace(CreateDefaultSubobject<UStaticMeshComponent>(FName(FString::FromInt(i))));
	}

	RootComponent = SmallFishesMesh[0];

	static ConstructorHelpers::FObjectFinder<UStaticMesh>SMALLFISHES_MESH(TEXT("/Game/Meshes/Fish.Fish"));
	if (SMALLFISHES_MESH.Succeeded())
	{
		for (auto& SmallFishes : SmallFishesMesh)
		{
			SmallFishes->SetCollisionProfileName("OverlapAllDynamic");
			SmallFishes->SetStaticMesh(SMALLFISHES_MESH.Object);
			SmallFishes->OnComponentBeginOverlap.AddDynamic(this, &ASmallFishes::OnOverlapBegin);

			if (SmallFishes == RootComponent)
			{
				SmallFishes->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
				continue;
			}

			SmallFishes->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}

	CalculateLocationTask = nullptr;
}

// Called when the game starts or when spawned
void ASmallFishes::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASmallFishes::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* Character = Cast<ADiveCharacter>(OtherActor);
	if (Character)
	{
		Character->ReceiveAnyDamage(10.0f);
	}
	else if (OtherComp->GetCollisionObjectType() == ECC_WorldStatic)
	{
		//Destroy();
	}
}

// Called every frame
void ASmallFishes::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SmallFishesCalLocationAsync(DeltaTime);
}

void ASmallFishes::SmallFishesCalLocationAsync(float DeltaTime)
{
	if (!CalculateLocationTask)
	{
		CalculateLocationTask = new SmallFishesCalLocationTask(this, DeltaTime);
		FRunnableThread::Create(CalculateLocationTask, TEXT("CalculateLocationTask"));
	}
}

SmallFishesCalLocationTask::SmallFishesCalLocationTask(ASmallFishes* InSmallFishes, float InDeltaTime)
	: SmallFishes(InSmallFishes)
	, DeltaTime(InDeltaTime)
	, bIsRunning(false)
{
}

bool SmallFishesCalLocationTask::Init()
{
	bIsRunning = true;
	return true;
}

uint32 SmallFishesCalLocationTask::Run()
{
	while (bIsRunning)
	{
		if (SmallFishes && SmallFishes->GetWorld() && SmallFishes->GetWorld()->IsGameWorld())
		{
			FVector CurrentLocation = SmallFishes->GetActorLocation();

			float ArcHeight = 10.f;
			float ArcDuration = 2.f;

			float TimeElapsed = SmallFishes->GetWorld()->GetTimeSeconds();
			float ZOffset = (FMath::Sin(TimeElapsed / ArcDuration * PI) * ArcHeight);

			FRotator CurrentRotation = SmallFishes->GetActorRotation();
			FRotator RotationToAdd = FRotator(0.f, 90.f, 0.f);
			FRotator NewRotation = CurrentRotation + RotationToAdd;
			FVector ForwardDirection = NewRotation.Vector();

			FVector NewLocation = CurrentLocation;

			NewLocation += ForwardDirection * 10.f;
			NewLocation.Z += ZOffset;

			AsyncTask(ENamedThreads::GameThread, [this, NewLocation, ZOffset]()
				{
					if (SmallFishes && SmallFishes->IsValidLowLevel())
					{
						SmallFishes->SetActorLocation(NewLocation);
					}
				});

			FPlatformProcess::Sleep(0.04f);
		}
		else
		{
			break;
		}
	}

	return 0;
}

void SmallFishesCalLocationTask::Stop()
{
	bIsRunning = false;
}

void SmallFishesCalLocationTask::Exit()
{
}