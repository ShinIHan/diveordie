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
	virtual void NativeConstruct() override;			// UMG Widget ������, ������ �ʱ�ȭ ���.

	virtual void NativeDestruct() override;				// UMG Widget ��ü �ı��� ȣ��. �Ҵ��� ���ҽ� ����

	bool GetIsConnect();

	ClientSocket* GetSocket();

private:
	ClientSocket* Socket = nullptr;
	bool _bIsConnected = false;
};
