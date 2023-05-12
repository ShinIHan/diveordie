// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialWidget.h"

void UTutorialWidget::StartTutorialTimer()
{
	GetWorld()->GetTimerManager().SetTimer(TutorialTimer, this, &UTutorialWidget::TutorialEnd, 30.0f, false);
}

void UTutorialWidget::TutorialEnd()
{
	OnTutorialEnd.Broadcast();
}
