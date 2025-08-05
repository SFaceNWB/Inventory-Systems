// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/GridSlots/Bag_GridSlot.h"
#include "Items/Bag_InventoryItem.h"

#include "Components/Image.h"

void UBag_GridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	GridSlotHovered.Broadcast(TileIndex, InMouseEvent);
}

void UBag_GridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	GridSlotUnhovered.Broadcast(TileIndex, InMouseEvent);
}

FReply UBag_GridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	GridSlotClicked.Broadcast(TileIndex, InMouseEvent);
	return FReply::Handled();
}

void UBag_GridSlot::SetInventoryItem(UBag_InventoryItem* Item)
{
	InventoryItem = Item;
}

void UBag_GridSlot::SetUnOccupiedTexture()
{
	GridSlotState = Ebag_GridSlotState::Unoccupied;
	Image_GridSlot->SetBrush(Brush_Unoccupied);
}

void UBag_GridSlot::SetSelectedTexture()
{
	GridSlotState = Ebag_GridSlotState::Selected;
	Image_GridSlot->SetBrush(Brush_Selected);
}

void UBag_GridSlot::SetOccupiedTexture()
{
	GridSlotState = Ebag_GridSlotState::Occupied;
	Image_GridSlot->SetBrush(Brush_Occupied);
}

void UBag_GridSlot::SetGrayedOutTexture()
{
	GridSlotState = Ebag_GridSlotState::GrayedOut;
	Image_GridSlot->SetBrush(Brush_GrayedOut);
}
