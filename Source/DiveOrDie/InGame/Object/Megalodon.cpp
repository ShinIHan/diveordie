// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/Megalodon.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

AMegalodon::AMegalodon()
{
	PrimaryActorTick.bCanEverTick = true;

	box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = box;

	box->SetBoxExtent(FVector(450.0f, 1100.0f, 300.0f));

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	mesh->SetCollisionProfileName("Object");
	box->SetCollisionProfileName("OverlapAllDynamic");

	mesh->SetRelativeScale3D(FVector(10.0f, 10.0f, 10.0f));
	mesh->SetRelativeLocation(FVector(0.0f, 300.0f, -300.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MEGALODON_MESH(TEXT("/Game/Meshes/Megalodon.Megalodon"));
	if (MEGALODON_MESH.Succeeded())
	{
		mesh->SetStaticMesh(MEGALODON_MESH.Object);
	}

	box->OnComponentBeginOverlap.AddDynamic(this, &AMegalodon::OnOverlapBegin);
	CalculateLocationTask = nullptr;
}

// Called when the game starts or when spawned
void AMegalodon::BeginPlay()
{
	Super::BeginPlay();
}

void AMegalodon::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* Character = Cast<ADiveCharacter>(OtherActor);
	if (Character)
	{
		Character->ReceiveAnyDamage(50.0f);

		FVector CharacterLocation = Character->GetActorLocation();
		FVector WhaleLocation = GetActorLocation();
		FVector Direction = CharacterLocation - WhaleLocation;
		Direction.Normalize();

		float DistanceFromWhale = FVector::Distance(CharacterLocation, WhaleLocation);
		float BlockingDistance = 5.0f;

		if (DistanceFromWhale < BlockingDistance)
		{
			FVector BlockedLocation = WhaleLocation + Direction * BlockingDistance;
			Character->SetActorLocation(BlockedLocation);
		}
	}
}

// Called every frame
void AMegalodon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalculateLocationAsync(DeltaTime);
}

void AMegalodon::CalculateLocationAsync(float DeltaTime)
{
	if (!CalculateLocationTask)
	{
		CalculateLocationTask = new FAsyncCalculateLocationTask(this, DeltaTime);
		FRunnableThread::Create(CalculateLocationTask, TEXT("CalculateLocationTask"));
	}
}

FAsyncCalculateLocationTask::FAsyncCalculateLocationTask(AMegalodon* InMegalodon, float InDeltaTime)
	: Megalodon(InMegalodon)
	, DeltaTime(InDeltaTime)
	, bIsRunning(false)
{
}

bool FAsyncCalculateLocationTask::Init()
{
	bIsRunning = true;
	return true;
}

uint32 FAsyncCalculateLocationTask::Run()
{
	while (bIsRunning)
	{
		if (Megalodon && Megalodon->GetWorld() && Megalodon->GetWorld()->IsGameWorld())
		{
			FVector CurrentLocation = Megalodon->GetActorLocation();

			float ArcHeight = 20.f;
			float ArcDuration = 2.f;

			float TimeElapsed = Megalodon->GetWorld()->GetTimeSeconds();
			float ZOffset = (FMath::Sin(TimeElapsed / ArcDuration * PI) * ArcHeight);

			FVector NewLocation = CurrentLocation;

			FRotator CurrentRotation = Megalodon->GetActorRotation();
			FRotator RotationToAdd = FRotator(0.f, 90.f, 0.f);
			FRotator NewRotation = CurrentRotation + RotationToAdd;
			FVector ForwardDirection = NewRotation.Vector();

			NewLocation += ForwardDirection * 25.f;
			NewLocation.Z += ZOffset;

			AsyncTask(ENamedThreads::GameThread, [this, NewLocation]()
				{
					if (Megalodon && Megalodon->IsValidLowLevel())
					{
						Megalodon->SetActorLocation(NewLocation);
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

void FAsyncCalculateLocationTask::Stop()
{
	bIsRunning = false;
}

void FAsyncCalculateLocationTask::Exit()
{

}