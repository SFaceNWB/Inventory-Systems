// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/Bag_SpatialInventory.h"
#include "Widgets/Inventory/Spatial/Bag_InventoryGrid.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

void UBag_SpatialInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Equipped->OnClicked.AddDynamic(this, &ThisClass::ShowEquipped);
	Button_Consumables->OnClicked.AddDynamic(this, &ThisClass::ShowConsumables);
	Button_Craftables->OnClicked.AddDynamic(this, &ThisClass::ShowCraftables);

	ShowEquipped();
}

FBag_SlotAvailabilityResult UBag_SpatialInventory::HasRoomForItem(UBag_ItemComponent* ItemComponent) const
{
	FBag_SlotAvailabilityResult Result;
	Result.TotalRoomToFill = 1;
	return Result;
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
	DisableButton(Button);
	Switcher->SetActiveWidget(Grid);
}
