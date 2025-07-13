// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Bag_InventoryItem.h"

#include "Net/UnrealNetwork.h"

void UBag_InventoryItem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemManifest);
}

void UBag_InventoryItem::SetItemManifest(const FBag_ItemManifest& Manifest)
{
	ItemManifest = FInstancedStruct::Make<FBag_ItemManifest>(Manifest);
}
