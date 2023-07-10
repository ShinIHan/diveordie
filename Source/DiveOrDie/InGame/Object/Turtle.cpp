// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/Turtle.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
ATurtle::ATurtle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TurtleBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TurtleBox"));
	RootComponent = TurtleBox;
	TurtleBox->SetBoxExtent(FVector(115.0f, 125.0f, 35.0f));

	Turtlemesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurtleStaticMesh"));
	Turtlemesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	FScriptDelegate Turtledele;
	Turtledele.BindUFunction(this, "OnHit");

	Turtlemesh->OnComponentHit.Add(Turtledele);
	Turtlemesh->SetRelativeScale3D(FVector(20.0f, 20.0f, 20.0f));
	Turtlemesh->SetRelativeLocation(FVector(0.0f, 25.0f, -5.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> TURTLE_MESH(TEXT("/Game/Meshes/Turtle.Turtle"));
	if (TURTLE_MESH.Succeeded())
	{
		Turtlemesh->SetStaticMesh(TURTLE_MESH.Object);
	}

	TurtleBox->OnComponentBeginOverlap.AddDynamic(this, &ATurtle::OnOverlapBegin);
	TurtleCalculateLocationTask = nullptr;
}

void ATurtle::BeginPlay()
{
	Super::BeginPlay();
}

void ATurtle::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character)
	{
		character->ReceiveOxygenDamage(10.f);

		FVector CharacterLocation = character->GetActorLocation();
		FVector WhaleLocation = GetActorLocation();
		FVector Direction = CharacterLocation - WhaleLocation;
		Direction.Normalize();

		float DistanceFromWhale = FVector::Distance(CharacterLocation, WhaleLocation);
		float BlockingDistance = 5.0f;

		if (DistanceFromWhale < BlockingDistance)
		{
			FVector BlockedLocation = WhaleLocation + Direction * BlockingDistance;
			character->SetActorLocation(BlockedLocation);
		}
	}
}

void ATurtle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TurtleCalLocationAsync(DeltaTime);
}

void ATurtle::TurtleCalLocationAsync(float DeltaTime)
{
	if (!TurtleCalculateLocationTask)
	{
		TurtleCalculateLocationTask = new TurtleCalLocationTask(this, DeltaTime);
		FRunnableThread::Create(TurtleCalculateLocationTask, TEXT("TurtleCalculateLocationTask"));
	}
}

TurtleCalLocationTask::TurtleCalLocationTask(ATurtle* InTurtle, float InTurtleDeltaTime)
	: Turtle(InTurtle)
	, DeltaTime(InTurtleDeltaTime)
	, bIsRunning(false)
{
}

bool TurtleCalLocationTask::Init()
{
	bIsRunning = true;
	return true;
}

uint32 TurtleCalLocationTask::Run()
{
	while (bIsRunning)
	{
		if (Turtle && Turtle->IsValidLowLevel() && Turtle->GetWorld() && Turtle->GetWorld()->IsGameWorld())
		{
			FVector CurrentLocation = Turtle->GetActorLocation();
			FVector NewLocation = CurrentLocation;

			FRotator CurrentRotation = Turtle->GetActorRotation();
			FRotator RotationToAdd = FRotator(0.f, 90.f, 0.f);
			FRotator NewRotation = CurrentRotation + RotationToAdd;
			FVector ForwardDirection = NewRotation.Vector();

			NewLocation -= ForwardDirection * 15.f;

			TWeakObjectPtr<ATurtle> TurtlePtr = Turtle;

			AsyncTask(ENamedThreads::GameThread, [TurtlePtr, NewLocation]()
				{
					if (TurtlePtr.IsValid())
					{
						TurtlePtr->SetActorLocation(NewLocation);
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

void TurtleCalLocationTask::Stop()
{
	bIsRunning = false;
}

void TurtleCalLocationTask::Exit()
{
}