// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/Bag_GridTypes.h"

#include "Bag_InventoryGrid.generated.h"

struct FBag_ImageFragment;
struct FBag_GridFragment;
class UBag_SlottedItem;
struct FBag_ItemManifest;
class UBag_ItemComponent;
class UBag_InventoryComponent;
class UCanvasPanel;
class UBag_GridSlot;
/**
 * 
 */
UCLASS()
class INVENTORY_API UBag_InventoryGrid : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;

	EBag_ItemCategory GetItemCategory() const { return ItemCategory; }
	FBag_SlotAvailabilityResult HasRoomForItem(const UBag_ItemComponent* ItemComponent);

	UFUNCTION()
	void AddItem(UBag_InventoryItem* Item);

private:

	TWeakObjectPtr<UBag_InventoryComponent> InventoryComponent;

	void ConstructGrid();
	FBag_SlotAvailabilityResult HasRoomForItem(const UBag_InventoryItem* Item);
	FBag_SlotAvailabilityResult HasRoomForItem(const FBag_ItemManifest& Manifest);
	void AddItemToIndices(const FBag_SlotAvailabilityResult& Result, UBag_InventoryItem* NewItem);
	bool MatchesCategory(const UBag_InventoryItem* Item) const;
	FVector2D GetDrawSize(const FBag_GridFragment* GridFragment) const;
	void SetSlottedItemImage(const UBag_SlottedItem* SlottedItem, const FBag_GridFragment* GridFragment, const FBag_ImageFragment* ImageFragment) const;
	void AddItemAtIndex(UBag_InventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount);
	UBag_SlottedItem* CreateSlottedItem(UBag_InventoryItem* Item, const bool bStackable, const int32 StackAmount, const FBag_GridFragment* GridFragment, const FBag_ImageFragment* ImageFragment, const int32 Index);
	void AddSlottedItemToCanvas(const int32 Index, const FBag_GridFragment* GridFragment, UBag_SlottedItem* SlottedItem) const;
	void UpdateGridSlots(UBag_InventoryItem* NewItem, int32 Index, bool bStackableItem, const int32 StackAmount);

	UPROPERTY(EditAnywhere,BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Inventory")
	EBag_ItemCategory ItemCategory;

	UPROPERTY()
	TArray<TObjectPtr<UBag_GridSlot>> GridSlots;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UBag_GridSlot> GridSlotClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UBag_SlottedItem> SlottedItemClass;

	UPROPERTY()
	TMap<int32, TObjectPtr<UBag_SlottedItem>> SlottedItems;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Rows;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Columns;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float TileSize;

};



