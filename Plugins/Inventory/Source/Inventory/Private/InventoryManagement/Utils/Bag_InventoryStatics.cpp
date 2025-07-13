// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Utils/Bag_InventoryStatics.h"

#include "InventoryManagement/Components/Bag_InventoryComponent.h"
#include "Items/Components/Bag_ItemComponent.h"

UBag_InventoryComponent* UBag_InventoryStatics::GetInventoryComponent(const APlayerController* PlayerController)
{
	if (!IsValid(PlayerController))
	{
		return nullptr;
	}
	UBag_InventoryComponent* InventoryComponent = PlayerController->FindComponentByClass<UBag_InventoryComponent>();
	return InventoryComponent;
}

EBag_ItemCategory UBag_InventoryStatics::GetItemCategoryFromItemComponent(UBag_ItemComponent* ItemComponent)
{
	if (!IsValid(ItemComponent))
	{
		return EBag_ItemCategory::None;
	}
	return ItemComponent->GetItemManifest().GetItemCategory();
}
