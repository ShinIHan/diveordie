// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include "DiveOrDie/Core/DiveGameInstance.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULoginWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

bool ULoginWidget::Login(const FText& Id, const FText& Pw)
{
	if (Id.IsEmpty() || Pw.IsEmpty()) return false;

	if (!GetIsConnect()) return false; 

	UDiveGameInstance* GameInstance = Cast<UDiveGameInstance>(GetWorld()->GetGameInstance());
	
	bool IsSuccess = GetSocket()->Login(Id, Pw, GameInstance);

	if (!IsSuccess) return false;

	GetSocket()->CloseSocket();
	
	return true;
}

bool ULoginWidget::SignUp(const FText& Id, const FText& Pw)
{
	if (Id.IsEmpty() || Pw.IsEmpty()) return false;

	if (!GetIsConnect()) return false;

	bool IsSuccess = GetSocket()->SignUp(Id, Pw);

	if (!IsSuccess) return false;
	
	return true;
}
