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
}

// Called when the game starts or when spawned
void ATurtle::BeginPlay()
{
	Super::BeginPlay();	

	TurtleInitialLocation = GetActorLocation();
}

void ATurtle::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character)
	{
		character->ReceiveOxygenDamage(10.f);
	}
}

// Called every frame
void ATurtle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = CurrentLocation;

	NewLocation.Y -= 15.f;

	SetActorLocation(NewLocation);
}