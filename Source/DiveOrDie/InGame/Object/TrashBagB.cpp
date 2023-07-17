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

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(TrashBagBMesh);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> Interaction_UI(TEXT("/Game/VFX/WGBP_Interaction.WGBP_Interaction_C"));

	if (Interaction_UI.Succeeded())
	{
		WidgetComponent->SetWidgetClass(Interaction_UI.Class);
		WidgetComponent->SetDrawSize(FVector2D(150.f, 150.f));
	}
	TrashBagBBox->OnComponentBeginOverlap.AddDynamic(this, &ATrashBagB::OnOverlapBegin);
	TrashBagBBox->OnComponentEndOverlap.AddDynamic(this, &ATrashBagB::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ATrashBagB::BeginPlay()
{
	Super::BeginPlay();	
	WidgetComponent->SetVisibility(false);
}

void ATrashBagB::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void ATrashBagB::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character)
	{
		WidgetComponent->SetVisibility(false);
	}
}

void ATrashBagB::TrashBagBDestroy()
{
	Destroy();
}

// Called every frame
void ATrashBagB::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

