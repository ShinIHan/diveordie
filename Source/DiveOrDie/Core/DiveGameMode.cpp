// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveGameMode.h"

#include "DiveGameInstance.h"
#include "DiveGameState.h"
#include "HeadMountedDisplayTypes.h"
#include "DiveOrDie/Core/SerialPort.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

SerialPort* _serialPort = nullptr;

int button1 = 2;
int button2;
int button3;
int button4;
int ax;
int ay;

int StandardCount = 0;
int secondCount = 0;

float buffer1_x = 0, buffer1_y = 0;
float buffer2_x = 0, buffer2_y = 0;

int firstdata[50][2] = { 2 };
int seconddata[10][2] = { 2 };

ADiveGameMode::ADiveGameMode()
{
    static ConstructorHelpers::FClassFinder<ADiveCharacter> CHARCTER_BP(TEXT("/Game/Blueprints/BP_DiveCharacter.BP_DiveCharacter_C"));
    if (CHARCTER_BP.Succeeded())
    {
        DefaultPawnClass = CHARCTER_BP.Class;
    }

    static ConstructorHelpers::FClassFinder<ADiveGameState> GAMESTATE_BP(TEXT("/Game/Blueprints/BP_DiveGameState.BP_DiveGameState_C"));
    if (GAMESTATE_BP.Succeeded())
    {
        GameStateClass = GAMESTATE_BP.Class;
    }

    PrimaryActorTick.bCanEverTick = true;
    bUseSeamlessTravel = true;
}

void ADiveGameMode::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> Actors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStageManagerActor::StaticClass(), Actors);
    for (auto Actor : Actors)
    {
        StageManagerActor = Cast<AStageManagerActor>(Actor);
    }
    if (StageManagerActor)
    {
        StageManagerActor->OnEndCheck.AddUObject(this, &ADiveGameMode::GameClear);
    }

    if (_serialPort == nullptr)
    {
        _serialPort = new SerialPort("COM4", 115200, 8, NOPARITY, ONESTOPBIT);
        LOG_SCREEN("Serial");
    }

    LOG_SCREEN("Start");
}

void ADiveGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    try
    {
        // 쉼표로 분리 결과를 저장할 배열
        TArray<FString> words;

        // 시리얼 포트에서 데이터를 읽어와서 문자열로 변환
        FString data = _serialPort->ReadLine();

        // 마지막 단어 뒤에 쉼표가 없는 경우 false로 설정하여 초과 오류 방지
        data.ParseIntoArray(words, TEXT(","), false);

        if (words.Num() == 10)
        {
            button1 = FCString::Atoi(*words[0]);
            button2 = FCString::Atoi(*words[1]);
            button3 = FCString::Atoi(*words[2]);
            button4 = FCString::Atoi(*words[3]);

            unsigned int ValueX, ValueY;

            sscanf(TCHAR_TO_ANSI(*words[4]), "%x", &ValueX);
            sscanf(TCHAR_TO_ANSI(*words[5]), "%x", &ValueY);

            if (words[4].StartsWith("-"))
            {
                ValueX = -ValueX;
            }
            if (words[5].StartsWith("-"))
            {
                ValueY = -ValueY;
            }

            ax = ValueX;
            ay = ValueY;

            LOG_SCREEN("%d, %d, %d, %d, %d, %d", button1, button2, button3, button4, ax, ay);
        }
    }
    catch (const std::exception& e)
    {
        // 에러 처리
        UE_LOG(LogTemp, Error, TEXT("Error occurred while sending serial data: %s"), *FString(e.what()));
    }
}

void ADiveGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (NewPlayer)
    {
        ADiveCharacter* DiveCharacter = Cast<ADiveCharacter>(NewPlayer->GetPawn());
        if (DiveCharacter)
        {
            DiveCharacter->OnPlayerDieCheck.AddUObject(this, &ADiveGameMode::GameOver);
        }
    }
}

void ADiveGameMode::GameClear()
{
    LOG_SCREEN("GameClear");
    GetWorld()->ServerTravel("/Game/Maps/GameClear?listen");
}

void ADiveGameMode::GameOver()
{
    LOG_SCREEN("GameOver");
    if (StageManagerActor->bIsOnline)
    {
        GetWorld()->ServerTravel("/Game/Maps/GameOver?listen");
        return;
    }
    UGameplayStatics::OpenLevel(GetWorld(), "GameOver");
}