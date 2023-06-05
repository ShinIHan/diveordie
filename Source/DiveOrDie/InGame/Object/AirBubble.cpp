// Fill out your copyright notice in the Description page of Project Settings.


#include "AirBubble.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
AAirBubble::AAirBubble()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = box;
	box->SetBoxExtent(FVector(150.0f, 150.0f,  1000.0f));
	box->SetCollisionProfileName("OverlapAllDynamic");

	NiagaraSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraSystem"));
	NiagaraSystem->SetupAttachment(GetRootComponent());
	if (NiagaraSystem)
	{
		UNiagaraSystem* NiagaraSystemAsset = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/VFX/FX_Bubble.FX_Bubble")); 
		if (NiagaraSystemAsset)
		{
			NiagaraSystem->SetAsset(NiagaraSystemAsset);
			NiagaraSystem->SetCollisionProfileName(FName("Bubble"));
		}
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> BubbleAsset(TEXT("/Game/Sounds/Bubble_Cue.Bubble_Cue"));
	if (BubbleAsset.Succeeded())
	{
		BubbleCue = BubbleAsset.Object;
	}

}

//void AAirBubble::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
//	if (character)
//	{
//		character->Breath(OxygenAmount);
//		LOG_SCREEN("Collision");
//	} 
//}
void AAirBubble::OnParticleBeginOverlap(UNiagaraComponent* NiagaraComponent, int32 ParticleIndex, FHitResult HitResult)
{
	AActor* OtherActor = HitResult.GetActor();
	// 충돌한 오브젝트가 캐릭터인지 확인합니다.
	if (OtherActor->IsA(ADiveCharacter::StaticClass()))
	{
		// 캐릭터의 breath 함수를 호출합니다.
		ADiveCharacter* Character = Cast<ADiveCharacter>(OtherActor);
		//Character->Breath(OxygenAmount);
		// "Collision"을 화면에 출력합니다.
		LOG_SCREEN("Collision");
	}
}
//void AAirBubble::OnParticleCollide(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//
//	ADiveCharacter* Character = Cast<ADiveCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
//	if (Character)
//	{
//		Character->Breath(OxygenAmount);
//		LOG_SCREEN("Collision");
//	}
//}

// Called when the game starts or when spawned
void AAirBubble::BeginPlay()
{
	Super::BeginPlay();
	if (BubbleCue)
	{
		FVector SoundLocation = GetActorLocation();
		UGameplayStatics::PlaySoundAtLocation(this, BubbleCue, SoundLocation);
	}

}

// Called every frame
void AAirBubble::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

