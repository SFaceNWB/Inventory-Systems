// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Inventory/GridSlots/Bag_GridSlot.h"
#include "GameplayTagContainer.h"
#include "Bag_EquippedGridSlot.generated.h"

class UOverlay;
class UBag_EquippedSlottedItem;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquippedGridSlotClicked, UBag_EquippedGridSlot*, GridSlot,
                                             const FGameplayTag&, EquipmentTypeTag);

UCLASS()
class INVENTORY_API UBag_EquippedGridSlot : public UBag_GridSlot
{
	GENERATED_BODY()
public:

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UBag_EquippedSlottedItem* OnItemEquipped(UBag_InventoryItem* Item, const FGameplayTag& EquipTypeTag, float TileSize);

	FEquippedGridSlotClicked EquippedGridSlotClicked;

private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "GameItems.Equipment"))
	FGameplayTag EquipmentTypeTag;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GrayedOutIcon;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UBag_EquippedSlottedItem> EquippedSlottedItemClass;

	UPROPERTY()
	TObjectPtr<UBag_EquippedSlottedItem> EquippedSlottedItem;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> Overlay_Root;
};
