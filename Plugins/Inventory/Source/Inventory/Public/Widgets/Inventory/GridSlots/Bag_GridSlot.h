// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Bag_GridSlot.generated.h"

class UBag_InventoryItem;
class UImage;

UENUM(BlueprintType)
enum class Ebag_GridSlotState : uint8
{
	Unoccupied,
	Occupied,
	Selected,
	GrayedOut
};

UCLASS()
class INVENTORY_API UBag_GridSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	int32 GetTileIndex() const { return TileIndex; }
	void SetTileIndex(int32 Index) { TileIndex = Index; }
	Ebag_GridSlotState GetGridSlotState() const { return GridSlotState; }
	TWeakObjectPtr<UBag_InventoryItem> GetInventoryItem() const { return InventoryItem; }
	void SetInventoryItem(UBag_InventoryItem* Item);
	int32 GetStackCount() const { return StackCount; }
	void SetStackCount(int32 Count) { StackCount = Count; }
	int32 GetIndex() const { return TileIndex; }
	void SetIndex(int32 Index) { TileIndex = Index; }
	int32 GetUpperLeftIndex() const { return UpperLeftIndex; }
	void SetUpperLeftIndex(int32 Index) { UpperLeftIndex = Index; }
	bool IsAvailable() { return bAvailable; }
	void SetAvailable(bool bIsAvailable) { bAvailable = bIsAvailable; }

	void SetUnOccupiedTexture();
	void SetSelectedTexture();
	void SetOccupiedTexture();
	void SetGrayedOutTexture();

private:
	int32 TileIndex;
	int32 StackCount;
	int32 UpperLeftIndex{ INDEX_NONE };
	TWeakObjectPtr<UBag_InventoryItem> InventoryItem;
	bool bAvailable;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Unoccupied;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Occupied;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Selected;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_GrayedQut;

	Ebag_GridSlotState GridSlotState;

};
