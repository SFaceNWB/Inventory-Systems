// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Utils/Bag_InventoryStatics.h"

#include "InventoryManagement/Components/Bag_InventoryComponent.h"
#include "Items/Components/Bag_ItemComponent.h"
#include "Widgets/Inventory/InventoryBase/Bag_InventoryBase.h"

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

void UBag_InventoryStatics::ItemHovered(APlayerController* PC, UBag_InventoryItem* Item)
{
	UBag_InventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC))
	{
		return;
	}
	UBag_InventoryBase* InventoryBase = IC->GetInventoryMenu();
	if (!IsValid(InventoryBase))
	{
		return;
	}
	if (InventoryBase->HasHoverItem())
	{
		return;
	}
	InventoryBase->OnItemHovered(Item);
}

void UBag_InventoryStatics::ItemUnhovered(APlayerController* PC)
{
	UBag_InventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC))
	{
		return;
	}
	UBag_InventoryBase* InventoryBase = IC->GetInventoryMenu();
	if (!IsValid(InventoryBase))
	{
		return;
	}
	if (InventoryBase->HasHoverItem())
	{
		return;
	}
	InventoryBase->OnItemUnhovered();
}

UBag_HoverItem* UBag_InventoryStatics::GetHoverItem(APlayerController* PC)
{
	UBag_InventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC))
	{
		return nullptr;
	}
	UBag_InventoryBase* InventoryBase = IC->GetInventoryMenu();
	if (!IsValid(InventoryBase))
	{
		return nullptr;
	}
	return InventoryBase->GetHoverItem();
}
