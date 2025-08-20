// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Widgets/Inventory/SlottedItems/Bag_SlottedItem.h"
#include "Bag_EquippedSlottedItem.generated.h"

class UBag_EquippedGridSlot;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquippedSlottedItemClicked, UBag_EquippedSlottedItem*, SlottedItem);

/**
 * 
 */
UCLASS()
class INVENTORY_API UBag_EquippedSlottedItem : public UBag_SlottedItem
{
	GENERATED_BODY()

public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	void SetEquipmentTypeTag(const FGameplayTag& NewEquipmentTypeTag) { EquipmentTypeTag = NewEquipmentTypeTag; }
	FGameplayTag GetEquipmentTypeTag() const { return EquipmentTypeTag; }

	FEquippedSlottedItemClicked OnEquippedSlottedItemClicked;

private:

	UPROPERTY()
	FGameplayTag EquipmentTypeTag;
};
