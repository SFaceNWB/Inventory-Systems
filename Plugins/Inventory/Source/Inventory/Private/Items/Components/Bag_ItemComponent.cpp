// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Components/Bag_ItemComponent.h"

#include "Net/UnrealNetwork.h"

UBag_ItemComponent::UBag_ItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	PickupMessage = FString("E - Pickup Item");
	SetIsReplicatedByDefault(true);
}

void UBag_ItemComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemManifest);
}

void UBag_ItemComponent::InitItemManifest(FBag_ItemManifest CopyOfManifest)
{
	ItemManifest = CopyOfManifest;
}

void UBag_ItemComponent::PickedUp()
{
	OnPickedUp();
	GetOwner()->Destroy();
}
