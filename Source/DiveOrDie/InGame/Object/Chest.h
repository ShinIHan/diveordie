// Fill out your copyright notice in the Description page of Project Settings.w

#pragma once

#include "../game.h"
#include "GameFramework/Actor.h"
#include "Chest.generated.h"

UENUM(BlueprintType)
enum class EBuffType : uint8
{
	None,
	SpeedBoost,
	Damage,
	Heal,
};

UCLASS()
class GAME_API AChest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChest();

	UPROPERTY(EditAnywhere)
	UBoxComponent* box;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimSequence* OpenAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	UAnimSequence* CloseAnimation;

	UFUNCTION()
	void PlayOpenAnimation();

	UFUNCTION()
	void PlayCloseAnimation();

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	bool bIsChestOpen;
	
	UFUNCTION(BlueprintCallable, Category = "Buff")
	void ApplyRandomBuff();

	UFUNCTION(BlueprintCallable, Category = "Buff")
	void RemoveBuff();

	FTimerHandle AnimationHandle;

	FTimerHandle BuffTimerHandle;

	void RemoveChest();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void InteractStart();
	virtual void InteractEnd();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
