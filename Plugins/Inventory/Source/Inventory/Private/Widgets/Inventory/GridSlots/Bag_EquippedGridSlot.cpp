// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/GridSlots/Bag_EquippedGridSlot.h"

#include "Components/Image.h"
#include "InventoryManagement/Utils/Bag_InventoryStatics.h"
#include "Widgets/Inventory/HoverItem/Bag_HoverItem.h"

void UBag_EquippedGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!IsAvailable())
	{
		return;
	}
	UBag_HoverItem* HoverItem = UBag_InventoryStatics::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem))
	{
		return;
	}
	if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
	{
		SetOccupiedTexture();
		Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UBag_EquippedGridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (!IsAvailable())
	{
		return;
	}
	UBag_HoverItem* HoverItem = UBag_InventoryStatics::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem))
	{
		return;
	}
	if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
	{
		SetUnOccupiedTexture();
		Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}

FReply UBag_EquippedGridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	EquippedGridSlotClicked.Broadcast(this,	EquipmentTypeTag);
	return FReply::Handled();
}
