// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/Can.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
ACan::ACan()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CanBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = CanBox;

	CanBox->SetBoxExtent(FVector(75.0f, 75.0f, 80.0f));

	CanMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	CanMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	CanMesh->SetCollisionProfileName("Object");
	CanBox->SetCollisionProfileName("OverlapAllDynamic");

	CanMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 2.0f));
	CanMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -70.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CAN_MESH(TEXT("/Game/Meshes/Can.Can"));
	if (CAN_MESH.Succeeded())
	{
		CanMesh->SetStaticMesh(CAN_MESH.Object);
	}
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(CanMesh);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> Interaction_UI(TEXT("/Game/VFX/WGBP_Interaction.WGBP_Interaction_C"));

	if (Interaction_UI.Succeeded())
	{
		WidgetComponent->SetWidgetClass(Interaction_UI.Class);
		WidgetComponent->SetDrawSize(FVector2D(150.f, 150.f));
	}

	CanBox->OnComponentBeginOverlap.AddDynamic(this, &ACan::OnOverlapBegin);
	CanBox->OnComponentEndOverlap.AddDynamic(this, &ACan::OnOverlapEnd);

}

// Called when the game starts or when spawned
void ACan::BeginPlay()
{
	Super::BeginPlay();
	WidgetComponent->SetVisibility(false);
}

void ACan::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* DiveCharacter = Cast<ADiveCharacter>(OtherActor);
	if (DiveCharacter)
	{
		FVector CharacterLocation = DiveCharacter->GetActorLocation();
		FVector ActorLocation = GetActorLocation();
		float InteractionDistanceSquared = 275.f * 275.f;

		if (FVector::DistSquared(CharacterLocation, ActorLocation) <= InteractionDistanceSquared)
		{
			WidgetComponent->SetVisibility(true);
		}
	}
}

void ACan::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ADiveCharacter* DiveCharacter = Cast<ADiveCharacter>(OtherActor);
	if (DiveCharacter)
	{
		WidgetComponent->SetVisibility(false);
	}
}
void ACan::CanDestroy()
{
	Destroy();
}

// Called every frame
void ACan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

