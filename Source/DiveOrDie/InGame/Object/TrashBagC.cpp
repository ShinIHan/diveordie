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

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(TrashBagCMesh);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> Interaction_UI(TEXT("/Game/VFX/WGBP_Interaction.WGBP_Interaction_C"));

	if (Interaction_UI.Succeeded())
	{
		WidgetComponent->SetWidgetClass(Interaction_UI.Class);
		WidgetComponent->SetDrawSize(FVector2D(150.f, 150.f));
	}

	TrashBagCBox->OnComponentBeginOverlap.AddDynamic(this, &ATrashBagC::OnOverlapBegin);
	TrashBagCBox->OnComponentEndOverlap.AddDynamic(this, &ATrashBagC::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ATrashBagC::BeginPlay()
{
	Super::BeginPlay();	
	WidgetComponent->SetVisibility(false);
}

void ATrashBagC::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* DiveCharacter = Cast<ADiveCharacter>(OtherActor);
	if (DiveCharacter)
	{
		FVector CharacterLocation = DiveCharacter->GetActorLocation();
		FVector ActorLocation = GetActorLocation();
		float InteractionDistanceSquared = 500.f * 500.f;

		if (FVector::DistSquared(CharacterLocation, ActorLocation) <= InteractionDistanceSquared)
		{
			WidgetComponent->SetVisibility(true);
		}
	}
}

void ATrashBagC::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character)
	{
		WidgetComponent->SetVisibility(false);
	}
}

void ATrashBagC::TrashBagCDestroy()
{
	Destroy();
}

// Called every frame
void ATrashBagC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}