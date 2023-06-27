// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/TrashBagA.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
ATrashBagA::ATrashBagA()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TrashBagABox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = TrashBagABox;

	TrashBagABox->SetBoxExtent(FVector(70.0f, 70.0f, 80.0f));

	TrashBagAMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	TrashBagAMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	TrashBagAMesh->SetCollisionProfileName("Object");
	TrashBagABox->SetCollisionProfileName("OverlapAllDynamic");

	TrashBagAMesh->SetRelativeScale3D(FVector(5.0f, 5.0f, 5.0f));
	TrashBagAMesh->SetRelativeLocation(FVector(0.0f, 32.0f, -95.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> TRASHBAGA_MESH(TEXT("/Game/Meshes/TrashBagA.TrashBagA"));
	if (TRASHBAGA_MESH.Succeeded())
	{
		TrashBagAMesh->SetStaticMesh(TRASHBAGA_MESH.Object);
	}

	TrashBagABox->OnComponentBeginOverlap.AddDynamic(this, &ATrashBagA::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ATrashBagA::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATrashBagA::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* Character = Cast<ADiveCharacter>(OtherActor);
	if (Character)
	{
		Character->UpdateTrashCount();
		Destroy();
	}
}

// Called every frame
void ATrashBagA::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

