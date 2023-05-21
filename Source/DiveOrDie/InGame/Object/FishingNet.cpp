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

	KeyText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Help"));
	KeyText->SetHorizontalAlignment(EHTA_Center);
	KeyText->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	KeyText->SetText(FText::FromString(TEXT("Press E button to save your team!")));
	KeyText->SetHiddenInGame(true);

	FVector TextOffset(0.f, 0.f, 150.f);
	KeyText->SetRelativeLocation(TextOffset);


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
		FVector Direction = character->GetActorLocation() - GetActorLocation();
		FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
		KeyText->SetRelativeRotation(NewRotation);
		KeyText->SetHiddenInGame(false);
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

	/*FVector TextForward = -GetActorUpVector();
	FVector TextRight = GetActorRightVector();

	FRotator TargetRotation = FRotationMatrix::MakeFromXY(TextForward, TextRight).Rotator();
	KeyText->SetWorldRotation(TargetRotation);*/
}

