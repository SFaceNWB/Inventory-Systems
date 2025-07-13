// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Manifest/Bag_ItemManifest.h"

#include "Items/Bag_InventoryItem.h"

UBag_InventoryItem* FBag_ItemManifest::Manifest(UObject* NewOuter)
{
	UBag_InventoryItem* Item = NewObject<UBag_InventoryItem>(NewOuter, UBag_InventoryItem::StaticClass());
	Item->SetItemManifest(*this);

	return Item;
}
