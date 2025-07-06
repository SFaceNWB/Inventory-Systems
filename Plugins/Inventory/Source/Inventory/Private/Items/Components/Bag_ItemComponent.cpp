// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Components/Bag_ItemComponent.h"

UBag_ItemComponent::UBag_ItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	PickupMessage = FString("E - Pickup Item");
}
