// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/Bag_GridTypes.h"
#include "Bag_InventoryBase.generated.h"

class UBag_HoverItem;
class UBag_ItemComponent;
class UBag_InventoryItem;
/**
 * 
 */
UCLASS()
class INVENTORY_API UBag_InventoryBase : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual FBag_SlotAvailabilityResult HasRoomForItem(UBag_ItemComponent* ItemComponent) const {return FBag_SlotAvailabilityResult();}
	virtual void OnItemHovered(UBag_InventoryItem* Item){ }
	virtual void OnItemUnhovered() { }
	virtual bool HasHoverItem() const { return false; }
	virtual UBag_HoverItem* GetHoverItem() const { return nullptr; }
	virtual float GetTileSize() const { return 0.0f; }
};
