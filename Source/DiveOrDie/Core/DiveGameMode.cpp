// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/Core/DiveGameMode.h"
#include "DiveOrDie/Core/DiveGameState.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

ADiveGameMode::ADiveGameMode()
{
	static ConstructorHelpers::FClassFinder<ADiveCharacter> CHARCTER_BP(TEXT("/Game/Blueprints/BP_DiveCharacter.BP_DiveCharacter"));
	if (CHARCTER_BP.Succeeded())
	{
		DefaultPawnClass = CHARCTER_BP.Class;
	}

	static ConstructorHelpers::FClassFinder<ADiveCharacter> GAMESTATE_BP(TEXT("/Game/Blueprints/BP_DiveGameState.BP_DiveGameState"));
	if (CHARCTER_BP.Succeeded())
	{
		GameStateClass = GAMESTATE_BP.Class;
	}
}