// Fill out your copyright notice in the Description page of Project Settings.


#include "FishingNet.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
AFishingNet::AFishingNet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = box;
	box->SetBoxExtent(FVector(300.0f, 160.0f, 32.0f));
	//box->SetCapsuleRadius(80);

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	mesh->SetCollisionProfileName("Object");
	box->SetCollisionProfileName("OverlapAllDynamic");

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FISHINGNET_MESH(TEXT("/Game/Meshes/FishingNet.FishingNet"));
    if (FISHINGNET_MESH.Succeeded())
    {
	    mesh->SetStaticMesh(FISHINGNET_MESH.Object);
    }

	box->OnComponentBeginOverlap.AddDynamic(this, &AFishingNet::OnOverlapBegin);
}


void AFishingNet::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//LOG_SCREEN("Overlap FishingNet");
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character)
	{
		character->Restraint(10.0f);
	}
}

// Called when the game starts or when spawned
void AFishingNet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFishingNet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

