// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/TrashBagC.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
ATrashBagC::ATrashBagC()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TrashBagCBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = TrashBagCBox;

	TrashBagCBox->SetBoxExtent(FVector(90.0f, 100.0f, 50.0f));

	TrashBagCMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	TrashBagCMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	TrashBagCMesh->SetCollisionProfileName("Object");
	TrashBagCBox->SetCollisionProfileName("OverlapAllDynamic");

	TrashBagCMesh->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.0f));
	TrashBagCMesh->SetRelativeLocation(FVector(0.0f, 12.0f, -47.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> TRASHBAGC_MESH(TEXT("/Game/Meshes/TrashBagC.TrashBagC"));
	if (TRASHBAGC_MESH.Succeeded())
	{
		TrashBagCMesh->SetStaticMesh(TRASHBAGC_MESH.Object);
	}

	TrashBagCBox->OnComponentBeginOverlap.AddDynamic(this, &ATrashBagC::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ATrashBagC::BeginPlay()
{
	Super::BeginPlay();	
}

void ATrashBagC::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* Character = Cast<ADiveCharacter>(OtherActor);
	if (Character)
	{
		Character->UpdateTrashCount();
		Destroy();
	}
}


// Called every frame
void ATrashBagC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}