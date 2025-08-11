// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemDescription/Bag_ItemDescription.h"

#include "Components/SizeBox.h"

FVector2D UBag_ItemDescription::GetBoxSize() const
{
	return SizeBox->GetDesiredSize();
}
