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

UENUM(BlueprintType)
enum class EBag_TileQuadrant : uint8
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
	None
};

USTRUCT(BlueprintType)
struct FBag_TileParameters
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	FIntPoint TileCoordinates{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	int32 TileIndex{ INDEX_NONE };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	EBag_TileQuadrant TileQuadrant{ EBag_TileQuadrant::None };

};

inline bool operator==(const FBag_TileParameters& A, const FBag_TileParameters& B)
{
	return A.TileCoordinates == B.TileCoordinates && A.TileIndex == B.TileIndex && A.TileQuadrant == B.TileQuadrant;
}


USTRUCT()
struct Fbag_SpaceQueryResult
{
	GENERATED_BODY()

	// 如果查询的空间没有物品，则为true
	bool bHasSpace{ false };

	// 如果只有一个物品可以互相交互
	TWeakObjectPtr<UBag_InventoryItem> ValidItem = nullptr;

	// 如果有有效物品，左上角的索引
	int32 UpperLeftIndex{ INDEX_NONE };
};
