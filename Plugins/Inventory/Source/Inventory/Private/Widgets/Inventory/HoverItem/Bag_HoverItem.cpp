// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/HoverItem/Bag_HoverItem.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Items/Bag_InventoryItem.h"

void UBag_HoverItem::SetImageBrush(const FSlateBrush& Brush) const
{
	Image_Icon->SetBrush(Brush);
}

void UBag_HoverItem::UpdateStackCount(const int32 Count)
{
	StackCount = Count;
	if (Count > 0)
	{
		Text_StackCount->SetText(FText::AsNumber(Count));
		Text_StackCount->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}

FGameplayTag UBag_HoverItem::GetItemType() const
{
	if (InventoryItem.IsValid())
	{
		return InventoryItem->GetItemManifest().GetItemType();
	}
	return FGameplayTag();
}

void UBag_HoverItem::SetIsStackable(bool bStacks)
{
	bIsStackable = bStacks;
	if (!bStacks)
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}

UBag_InventoryItem* UBag_HoverItem::GetInventoryItem() const
{
	return InventoryItem.Get();
}

void UBag_HoverItem::SetInventoryItem(UBag_InventoryItem* Item)
{
	InventoryItem = Item;
} 
