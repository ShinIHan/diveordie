// Fill out your copyright notice in the Description page of Project Settings.


#include "LandMine.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
ALandMine::ALandMine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = box;

	box->SetBoxExtent(FVector(100.0f, 100.0f, 30.0f));
	
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	mesh->SetCollisionProfileName("Object");
	box->SetCollisionProfileName("OverlapAllDynamic");

	mesh->SetRelativeLocation(FVector(0.0f, 0.0f, -30.0f));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MINE_MESH(TEXT("/Game/Meshes/landmine.landmine"));
	if (MINE_MESH.Succeeded())
	{
		mesh->SetStaticMesh(MINE_MESH.Object);
	}

	box->OnComponentBeginOverlap.AddDynamic(this, &ALandMine::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ALandMine::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALandMine::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//LOG_SCREEN("Overlap LandMine");
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character)
	{
		character->ReceiveAnyDamage(150.0f);
		Destroy();
	}
}

// Called every frame
void ALandMine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

