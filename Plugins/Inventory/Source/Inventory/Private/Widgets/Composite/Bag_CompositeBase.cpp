// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Composite/Bag_CompositeBase.h"

void UBag_CompositeBase::Collapse()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UBag_CompositeBase::Expand()
{
	SetVisibility(ESlateVisibility::Visible);
}
