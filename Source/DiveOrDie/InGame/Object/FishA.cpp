// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/FishA.h"

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

			if (Fish_A == RootComponent)
			{
				Fish_A->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.3f));
				continue;
			}

			Fish_A->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}

// Called when the game starts or when spawned
void AFishA::BeginPlay()
{
	Super::BeginPlay();	
}

void AFishA::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

// Called every frame
void AFishA::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}