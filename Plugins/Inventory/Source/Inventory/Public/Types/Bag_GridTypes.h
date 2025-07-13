// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bag_GridTypes.generated.h"

class UBag_InventoryItem;

UENUM(BlueprintType)
enum class EBag_ItemCategory : uint8
{
	Equipped,
	Consumable,
	Craftable,
	None
};

USTRUCT()
struct FBag_SlotAvailability
{
	GENERATED_BODY()

	FBag_SlotAvailability() {}
	FBag_SlotAvailability(int32 ItemIndex, int32 Room, bool bHasItem)
	: Index(ItemIndex), AmountToFill(Room), bItemAtIndex(bHasItem) {}

	int32 Index{ INDEX_NONE };
	int32 AmountToFill{ 0 };
	bool bItemAtIndex{ false };
};

USTRUCT()
struct FBag_SlotAvailabilityResult
{
	GENERATED_BODY()

	FBag_SlotAvailabilityResult() {}

	TWeakObjectPtr<UBag_InventoryItem> Item;
	int32 TotalRoomToFill{ 0 };
	int32 Remainder{ 0 };
	bool bStackable{ false };
	TArray<FBag_SlotAvailability> SlotAvailabilities;
};


