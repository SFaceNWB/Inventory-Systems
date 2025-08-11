// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/SlottedItems/Bag_SlottedItem.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InventoryManagement/Utils/Bag_InventoryStatics.h"
#include "Items/Bag_InventoryItem.h"

FReply UBag_SlottedItem::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	OnSlottedItemClicked.Broadcast(GridIndex, MouseEvent);
	return FReply::Handled();
}

void UBag_SlottedItem::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UBag_InventoryStatics::ItemHovered(GetOwningPlayer(), InventoryItem.Get());
}

void UBag_SlottedItem::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	UBag_InventoryStatics::ItemUnhovered(GetOwningPlayer());
}

void UBag_SlottedItem::SetInventoryItem(UBag_InventoryItem* Item)
{
	InventoryItem = Item;
}

void UBag_SlottedItem::SetImageBrush(const FSlateBrush& Brush) const
{
	Image_Icon->SetBrush(Brush);
}

void UBag_SlottedItem::UpdateStackCount(int32 StackCount)
{
	if (StackCount > 0)
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Visible);
		Text_StackCount->SetText(FText::AsNumber(StackCount));
	}
	else
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);	
	}
}
