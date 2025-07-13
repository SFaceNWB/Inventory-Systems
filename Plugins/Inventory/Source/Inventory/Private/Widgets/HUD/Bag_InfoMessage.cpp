// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/HUD/Bag_InfoMessage.h"

#include "Components/TextBlock.h"

void UBag_InfoMessage::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MessageHide();
}

void UBag_InfoMessage::SetMessage()
{
	if (!bIsMessageActive)
	{
		MessageShow();
	}
	bIsMessageActive = true;

	GetWorld()->GetTimerManager().SetTimer(MessageTimer,
		[this]()
		{
			MessageHide();
			bIsMessageActive = false;
		},
		MessageLifeTime, false);
}
