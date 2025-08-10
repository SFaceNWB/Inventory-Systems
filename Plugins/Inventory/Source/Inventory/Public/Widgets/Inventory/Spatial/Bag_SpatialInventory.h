// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Inventory/InventoryBase/Bag_InventoryBase.h"
#include "Bag_SpatialInventory.generated.h"

class UCanvasPanel;
class UButton;
class UWidgetSwitcher;
class UBag_InventoryGrid;
/**
 * 
 */
UCLASS()
class INVENTORY_API UBag_SpatialInventory : public UBag_InventoryBase
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FBag_SlotAvailabilityResult HasRoomForItem(UBag_ItemComponent* ItemComponent) const override;
private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> Switcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBag_InventoryGrid> Grid_Equipped;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBag_InventoryGrid> Grid_Consumables;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBag_InventoryGrid> Grid_Craftables;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Equipped;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Consumables;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Craftables;

	UFUNCTION()
	void ShowEquipped();

	UFUNCTION()
	void ShowConsumables();

	UFUNCTION()
	void ShowCraftables();

	void DisableButton(UButton* Button);
	void SetActiveGrid(UBag_InventoryGrid* Grid, UButton* Button);
	TWeakObjectPtr<UBag_InventoryGrid> ActiveGrid;
};
