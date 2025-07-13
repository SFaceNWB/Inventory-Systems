// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/Bag_GridTypes.h"

#include "Bag_InventoryGrid.generated.h"

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

	UPROPERTY(EditAnywhere,BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Inventory")
	EBag_ItemCategory ItemCategory;

	UPROPERTY()
	TArray<TObjectPtr<UBag_GridSlot>> GridSlots;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UBag_GridSlot> GridSlotClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Rows;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Columns;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float TileSize;

	bool MatchesCategory(const UBag_InventoryItem* Item) const;
};



