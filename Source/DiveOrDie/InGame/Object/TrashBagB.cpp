// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/TrashBagB.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
ATrashBagB::ATrashBagB()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TrashBagBBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = TrashBagBBox;

	TrashBagBBox->SetBoxExtent(FVector(50.0f, 40.0f, 70.0f));

	TrashBagBMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	TrashBagBMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	TrashBagBMesh->SetCollisionProfileName("Object");
	TrashBagBBox->SetCollisionProfileName("OverlapAllDynamic");

	TrashBagBMesh->SetRelativeScale3D(FVector(5.0f, 5.0f, 5.0f));
	TrashBagBMesh->SetRelativeLocation(FVector(-5.0f, 0.0f, -85.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> TRASHBAGB_MESH(TEXT("/Game/Meshes/TrashBagB.TrashBagB"));
	if (TRASHBAGB_MESH.Succeeded())
	{
		TrashBagBMesh->SetStaticMesh(TRASHBAGB_MESH.Object);
	}

	TrashBagBBox->OnComponentBeginOverlap.AddDynamic(this, &ATrashBagB::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ATrashBagB::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATrashBagB::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* Character = Cast<ADiveCharacter>(OtherActor);
	if (Character)
	{
		Character->UpdateTrashCount();
		Destroy();
	}
}

// Called every frame
void ATrashBagB::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

