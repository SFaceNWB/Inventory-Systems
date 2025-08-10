// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/Bag_SpatialInventory.h"
#include "Widgets/Inventory/Spatial/Bag_InventoryGrid.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "InventoryManagement/Utils/Bag_InventoryStatics.h"
#include "Inventory.h"

void UBag_SpatialInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Equipped->OnClicked.AddDynamic(this, &ThisClass::ShowEquipped);
	Button_Consumables->OnClicked.AddDynamic(this, &ThisClass::ShowConsumables);
	Button_Craftables->OnClicked.AddDynamic(this, &ThisClass::ShowCraftables);

	Grid_Equipped->SetOwningCanvas(CanvasPanel);
	Grid_Consumables->SetOwningCanvas(CanvasPanel);
	Grid_Craftables->SetOwningCanvas(CanvasPanel);

	ShowEquipped();
}

FReply UBag_SpatialInventory::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	ActiveGrid->DropItem();
	return FReply::Handled();
}

FBag_SlotAvailabilityResult UBag_SpatialInventory::HasRoomForItem(UBag_ItemComponent* ItemComponent) const
{
	switch (UBag_InventoryStatics::GetItemCategoryFromItemComponent(ItemComponent))
	{
	case EBag_ItemCategory::Equipped:
		return Grid_Equipped->HasRoomForItem(ItemComponent);
	case EBag_ItemCategory::Consumable:
		return Grid_Consumables->HasRoomForItem(ItemComponent);
	case EBag_ItemCategory::Craftable:
		return Grid_Craftables->HasRoomForItem(ItemComponent);
	default:
		UE_LOG(LogInventory, Error, TEXT("ItemComponent doesn't have a valid Item Category."));
		return FBag_SlotAvailabilityResult();
	}
}

void UBag_SpatialInventory::ShowEquipped()
{
	SetActiveGrid(Grid_Equipped,Button_Equipped);
}

void UBag_SpatialInventory::ShowConsumables()
{
	SetActiveGrid(Grid_Consumables,Button_Consumables);
}

void UBag_SpatialInventory::ShowCraftables()
{
	SetActiveGrid(Grid_Craftables,Button_Craftables);
}

void UBag_SpatialInventory::DisableButton(UButton* Button)
{
	Button_Equipped->SetIsEnabled(true);
	Button_Consumables->SetIsEnabled(true);
	Button_Craftables->SetIsEnabled(true);
	Button->SetIsEnabled(false);
}

void UBag_SpatialInventory::SetActiveGrid(UBag_InventoryGrid* Grid, UButton* Button)
{
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->HideCursor();
	}
	ActiveGrid = Grid;
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->ShowCursor();
	}
	DisableButton(Button);
	Switcher->SetActiveWidget(Grid);
}
