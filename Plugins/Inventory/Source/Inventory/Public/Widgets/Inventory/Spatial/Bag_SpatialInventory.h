// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Widgets/Inventory/InventoryBase/Bag_InventoryBase.h"
#include "Bag_SpatialInventory.generated.h"

class UBag_EquippedSlottedItem;
class UBag_EquippedGridSlot;
class UBag_ItemDescription;
class UCanvasPanel;
class UButton;
class UWidgetSwitcher;
class UBag_InventoryGrid;
class UBag_HoverItem;
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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FBag_SlotAvailabilityResult HasRoomForItem(UBag_ItemComponent* ItemComponent) const override;
	virtual void OnItemHovered(UBag_InventoryItem* Item) override;
	virtual void OnItemUnhovered() override;
	virtual bool HasHoverItem() const override;
	virtual UBag_HoverItem* GetHoverItem() const override;
	virtual float GetTileSize() const override;
private:

	UPROPERTY()
	TArray<TObjectPtr<UBag_EquippedGridSlot>> EquippedGridSlots;

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

	FTimerHandle DescriptionTimer;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DescriptionTimerDelay = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UBag_ItemDescription> ItemDescriptionClass;

	UPROPERTY()
	TObjectPtr<UBag_ItemDescription> ItemDescription;

	UBag_ItemDescription* GetItemDescription();

	UFUNCTION()
	void ShowEquipped();

	UFUNCTION()
	void ShowConsumables();

	UFUNCTION()
	void ShowCraftables();

	UFUNCTION()
	void EquippedGridSlotClicked(UBag_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag);

	UFUNCTION()
	void EquippedSlottedItemClicked(UBag_EquippedSlottedItem* SlottedItem);

	void DisableButton(UButton* Button);
	void SetActiveGrid(UBag_InventoryGrid* Grid, UButton* Button);
	void SetItemDescriptionSizeAndPosition(UBag_ItemDescription* Description, UCanvasPanel* Canvas) const;
	bool CanEquipHoverItem(UBag_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag) const;

	TWeakObjectPtr<UBag_InventoryGrid> ActiveGrid;
};
