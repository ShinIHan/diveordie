// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "DiveOrDie/Core/ClientSocket.h"
#include "Blueprint/UserWidget.h"
#include "DiveGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API UDiveGameWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;			// UMG Widget 생성시, 위젯의 초기화 담당.

	virtual void NativeDestruct() override;				// UMG Widget 객체 파괴시 호출. 할당한 리소스 해제

	bool GetIsConnect();

	ClientSocket* GetSocket();

private:
	ClientSocket* Socket = nullptr;
	bool _bIsConnected = false;
};
