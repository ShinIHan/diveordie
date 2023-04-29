// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/UI/TutorialWidget.h"

void UTutorialWidget::StartTutorialTimer()
{
	GetWorld()->GetTimerManager().SetTimer(TutorialTimer, this, &UTutorialWidget::TutorialEnd, 30.f, false);
}

void UTutorialWidget::TutorialEnd()
{
	OnTutorialEnd.Broadcast();
}
