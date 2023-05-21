// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveGameMode.h"

#include "DiveGameInstance.h"
#include "DiveGameState.h"
#include "HeadMountedDisplayTypes.h"
#include "DiveOrDie/Core/SerialPort.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"
#include <future>

int buttonA = 2;
int buttonB = 2;
int buttonC = 2;
int buttonD = 2;
int ax = 0;
int ay = 0;
int az = 0;
int gx = 0;
int gy = 0;
int gz = 0;
int SumRX = 0;
int SumRY = 0;
float AvRx = 0.f;
float AvRy = 0.f;
int count = 0;
int Ba = 0;
int Bb = 0;
int Bc = 0;
int Bd = 0;
int Bx = 0;
int By = 0;

SerialPort* _serialPort = nullptr;

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

    LOG_SCREEN("Start");

    /*if (_serialPort == nullptr)
    {
        _serialPort = new SerialPort("COM4", 115200, 8, NOPARITY, ONESTOPBIT);
        LOG_SCREEN("Serial");

        // ���ο� �����带 �����ؼ� �ø��� ��Ʈ �����͸� �񵿱������� �о�ɴϴ�.
        std::thread readThread(&ADiveGameMode::ReadSerialData, this);
        readThread.detach();
    }*/
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
                        int buttonQ = FCString::Atoi(*words[0]);
                        int buttonW = FCString::Atoi(*words[1]);
                        int buttonE = FCString::Atoi(*words[2]);
                        int buttonR = FCString::Atoi(*words[3]);

                        return std::make_tuple(buttonQ, buttonW, buttonE, buttonR);
                    });

                // ���ӵ� ���� �������� �۾� ����
                auto getAccelerometerValues = std::async(std::launch::async, [&words]()
                    {
                        int accelerometerX = FCString::Atoi(*words[4]);
                        int accelerometerY = FCString::Atoi(*words[5]);
                        int accelerometerZ = FCString::Atoi(*words[6]);

                        return std::make_tuple(accelerometerX, accelerometerY, accelerometerZ);
                    });

                /*auto getGyroValues = std::async(std::launch::async, [&words]()
                    {
                        int gyroX = FCString::Atoi(*words[7]);
                        int gyroY = FCString::Atoi(*words[8]);
                        int gyroZ = FCString::Atoi(*words[9]);

                        return std::make_tuple(gyroX, gyroY, gyroZ);
                    });*/

                // �۾��� �Ϸ�� ������ ���
                std::tuple<int, int, int, int> buttonValues = getButtonValues.get();
                std::tuple<int, int, int> accelerometerValues = getAccelerometerValues.get();
                /*std::tuple<int, int, int> gyroValues = getGyroValues.get();*/

                // ����� ������ ����
                buttonA = std::get<0>(buttonValues);
                buttonB = std::get<1>(buttonValues);
                buttonC = std::get<2>(buttonValues);
                buttonD = std::get<3>(buttonValues);

                ax = std::get<0>(accelerometerValues);
                ay = std::get<1>(accelerometerValues);
                az = std::get<2>(accelerometerValues);

                /*gx = std::get<0>(gyroValues);
                gy = std::get<1>(gyroValues);
                gz = std::get<2>(gyroValues);*/
                Ba = buttonA, Bb = buttonB, Bc = buttonC, Bd = buttonD;
                Bx = ax, By = ay;

                if (count == 31)
                {

                }
                else if (count == 30)
                {
                    AvRx = SumRX / 20;
                    AvRy = SumRY / 20;
                    count++;
                }
                else if (count < 30 && count >= 10)
                {
                    SumRX += ax;
                    SumRY += ay;
                    count++;
                }
                else if (count < 10)
                {
                    count++;
                }
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
