// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveGameMode.h"

#include "DiveGameInstance.h"
#include "DiveGameState.h"
#include "HeadMountedDisplayTypes.h"
#include "DiveOrDie/Core/SerialPort.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"
#include <future>

SerialPort* _serialPort = nullptr;

int buttonA;
int buttonB;
int buttonC;
int buttonD;
int ax;
int ay;

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

   // ���ο� �����带 �����ؼ� �ø��� ��Ʈ �����͸� �񵿱������� �о�ɴϴ�.
    std::thread readThread(&ADiveGameMode::ReadSerialData, this);
    readThread.detach();

    LOG_SCREEN("Start");
}

void ADiveGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
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

void ADiveGameMode::ReadSerialData()
{
    while (true)
    {
        try
        {
            // ��ǥ�� �и� ����� ������ �迭
            TArray<FString> words;

            // �ø��� ��Ʈ���� �����͸� �о�ͼ� ���ڿ��� ��ȯ
            FString data = _serialPort->ReadLine();

            // ������ �ܾ� �ڿ� ��ǥ�� ���� ��� false�� �����Ͽ� �ʰ� ���� ����
            data.ParseIntoArray(words, TEXT(","), false);

            if (words.Num() == 10)
            {
                // ��ư ���� �������� �۾� ����
                auto getButtonValues = std::async(std::launch::async, [&words]()
                    {
                        int button1 = FCString::Atoi(*words[0]);
                        int button2 = FCString::Atoi(*words[1]);
                        int button3 = FCString::Atoi(*words[2]);
                        int button4 = FCString::Atoi(*words[3]);

                        return std::make_tuple(button1, button2, button3, button4);
                    });

                // ��ǥ ���� �������� �۾� ����
                auto getCoordinates = std::async(std::launch::async, [&words]()
                    {
                        int value1 = std::stoi(TCHAR_TO_ANSI(*words[4]));
                        int value2 = std::stoi(TCHAR_TO_ANSI(*words[5]));

                        if (words[4].StartsWith("-"))
                        {
                            value1 = -abs(value1);
                        }
                        if (words[5].StartsWith("-"))
                        {
                            value2 = -abs(value2);
                        }

                        return std::make_tuple(value1, value2);
                    });

                // ��ư ���� �������� �۾��� ����� ��ٸ��ϴ�.
                std::tuple<int, int, int, int> buttonValues = getButtonValues.get();

                buttonA = std::get<0>(buttonValues);
                buttonB = std::get<1>(buttonValues);
                buttonC = std::get<2>(buttonValues);
                buttonD = std::get<3>(buttonValues);

                // ��ǥ ���� �������� �۾��� ����� ��ٸ��ϴ�.
                std::tuple<int, int> coordinates = getCoordinates.get();
                int valueX = std::get<0>(coordinates);
                int valueY = std::get<1>(coordinates);

                ax = valueX;
                ay = valueY;

                //LOG_SCREEN("%d, %d, %d, %d, %d, %d", buttonA, buttonB, buttonC, buttonD, ax, ay);
            }
        }
        catch (const std::exception& e)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed, %s"), e.what());
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
