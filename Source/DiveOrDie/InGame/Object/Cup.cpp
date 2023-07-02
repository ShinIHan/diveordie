// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/Cup.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
ACup::ACup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CupBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = CupBox;

	CupBox->SetBoxExtent(FVector(30.0f, 30.0f, 40.0f));

	CupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	CupMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	CupMesh->SetCollisionProfileName("Object");
	CupBox->SetCollisionProfileName("OverlapAllDynamic");

	CupMesh->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	CupMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CUP_MESH(TEXT("/Game/Meshes/Cup.Cup"));
	if (CUP_MESH.Succeeded())
	{
		CupMesh->SetStaticMesh(CUP_MESH.Object);
	}

	CupBox->OnComponentBeginOverlap.AddDynamic(this, &ACup::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ACup::BeginPlay()
{
	Super::BeginPlay();	
}

void ACup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void ACup::CupDestroy()
{
	Destroy();
}

// Called every frame
void ACup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

