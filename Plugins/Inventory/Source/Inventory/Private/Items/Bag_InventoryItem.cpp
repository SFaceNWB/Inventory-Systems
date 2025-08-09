// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Bag_InventoryItem.h"

#include "Items/Fragments/Bag_ItemFragment.h"
#include "Net/UnrealNetwork.h"

void UBag_InventoryItem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemManifest);
	DOREPLIFETIME(ThisClass, TotalStackCount);
}

void UBag_InventoryItem::SetItemManifest(const FBag_ItemManifest& Manifest)
{
	ItemManifest = FInstancedStruct::Make<FBag_ItemManifest>(Manifest);
}

bool UBag_InventoryItem::IsStackable() const
{
	const FBag_StackableFragment* Stackable = GetItemManifest().GetFragmentOfType<FBag_StackableFragment>();
	return Stackable != nullptr;
}

bool UBag_InventoryItem::IsConsumable() const
{
	return GetItemManifest().GetItemCategory() == EBag_ItemCategory::Consumable;
}
