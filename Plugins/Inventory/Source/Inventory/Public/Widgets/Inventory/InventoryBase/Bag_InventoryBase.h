// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/Bag_GridTypes.h"
#include "Bag_InventoryBase.generated.h"

class UBag_ItemComponent;
/**
 * 
 */
UCLASS()
class INVENTORY_API UBag_InventoryBase : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual FBag_SlotAvailabilityResult HasRoomForItem(UBag_ItemComponent* ItemComponent) const {return FBag_SlotAvailabilityResult();}
};
