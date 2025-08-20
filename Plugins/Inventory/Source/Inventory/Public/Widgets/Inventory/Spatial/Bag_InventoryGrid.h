// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/Bag_GridTypes.h"

#include "Bag_InventoryGrid.generated.h"

class UBag_ItemPopUp;
enum class Ebag_GridSlotState : uint8;
class UBag_HoverItem;
struct FGameplayTag;
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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	EBag_ItemCategory GetItemCategory() const { return ItemCategory; }
	FBag_SlotAvailabilityResult HasRoomForItem(const UBag_ItemComponent* ItemComponent);

	UFUNCTION()
	void AddItem(UBag_InventoryItem* Item);


	void ShowCursor();
	void HideCursor();
	void SetOwningCanvas(UCanvasPanel* OwningCanvas);
	void DropItem();
	bool HasHoverItem() const;
	UBag_HoverItem* GetHoverItem() const;
	float GetTileSize() const;
	void ClearHoverItem();
	void AssignHoverItem(UBag_InventoryItem* InventoryItem);
	void OnHide();

private:

	TWeakObjectPtr<UBag_InventoryComponent> InventoryComponent;
	TWeakObjectPtr<UCanvasPanel> OwningCanvasPanel;

	void ConstructGrid();
	FBag_SlotAvailabilityResult HasRoomForItem(const UBag_InventoryItem* Item, const int32 StackAmountOverride = -1);
	FBag_SlotAvailabilityResult HasRoomForItem(const FBag_ItemManifest& Manifest, const int32 StackAmountOverride = -1);
	void AddItemToIndices(const FBag_SlotAvailabilityResult& Result, UBag_InventoryItem* NewItem);
	bool MatchesCategory(const UBag_InventoryItem* Item) const;
	FVector2D GetDrawSize(const FBag_GridFragment* GridFragment) const;
	void SetSlottedItemImage(const UBag_SlottedItem* SlottedItem, const FBag_GridFragment* GridFragment, const FBag_ImageFragment* ImageFragment) const;
	void AddItemAtIndex(UBag_InventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount);
	UBag_SlottedItem* CreateSlottedItem(UBag_InventoryItem* Item, const bool bStackable, const int32 StackAmount, const FBag_GridFragment* GridFragment, const FBag_ImageFragment* ImageFragment, const int32 Index);
	void AddSlottedItemToCanvas(const int32 Index, const FBag_GridFragment* GridFragment, UBag_SlottedItem* SlottedItem) const;
	void UpdateGridSlots(UBag_InventoryItem* NewItem, int32 Index, bool bStackableItem, const int32 StackAmount);
	bool IsIndexClaimed(const TSet<int32>& CheckIndices, const int32 Index) const;
	bool HasRoomAtIndex(const UBag_GridSlot* GridSlot, const FIntPoint& Dimension, const TSet<int32>& CheckIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, const int32 MaxStackSize);
	FIntPoint GetItemDimensions(const FBag_ItemManifest& Manifest) const;
	bool CheckSlotConstraints(const UBag_GridSlot* GridSlot, const UBag_GridSlot* SubGridSlot, const TSet<int32>& CheckIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, const int32 MaxStackSize) const;
	bool HasValidItem(const UBag_GridSlot* GridSlot) const;
	bool IsUpperLeftSlot(const UBag_GridSlot* GridSlot, const UBag_GridSlot* SubGridSlot) const;
	bool DoesItemTypeMatch(const UBag_InventoryItem* SubItem, const FGameplayTag& ItemType) const;
	bool IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimensions) const;
	int32 DetermineFillAmoutForSlot(const bool bStackable, const int32 MaxStackSize, const int32 AmountToFill, const UBag_GridSlot* GridSlot) const;
	int32 GetStackAmount(const UBag_GridSlot* GridSlot) const;
	bool IsRightClick(const FPointerEvent& MouseEvent) const;
	bool IsLeftClick(const FPointerEvent& MouseEvent) const;
	void PickUp(UBag_InventoryItem* ClickedInventoryItem, const int32 GridIndex);
	void AssignHoverItem(UBag_InventoryItem* InventoryItem, const int32 GridIndex, const int32 PreviousGridIndex);
	void RemoveItemFromGrid(UBag_InventoryItem* InventoryItem, const int32 GridIndex);
	void UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition);
	FIntPoint CalculateHoveredCoordinates(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const;
	EBag_TileQuadrant CalculateTileQuadrant(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const;
	void OnTileParametersUpdate(const FBag_TileParameters& Parameters);
	FIntPoint CalculateStartingCoordinate(const FIntPoint& Coordinate, const FIntPoint& Dimensions, const EBag_TileQuadrant Quadrant) const;
	Fbag_SpaceQueryResult CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions);
	bool CursorExitedCanvas(const FVector2D& BoundaryPosition, const FVector2D& BoundarySize, const FVector2D& Location);
	void HighlightSlots(const int32 Index, const FIntPoint& Dimensions);
	void UnHighlightSlots(const int32 Index, const FIntPoint& Dimensions);
	void ChangeHoverType(const int32 Index, const FIntPoint& Dimensions, Ebag_GridSlotState GridSlotState);
	void PutDownOnIndex(const int32 Index);
	UUserWidget* GetVisibleCursorWidget();
	UUserWidget* GetHiddenCursorWidget();
	bool IsSameStackable(const UBag_InventoryItem* ClickedInventoryItem) const;
	void SwapWithHoverItem(UBag_InventoryItem* ClickedInventoryItem, const int32 GridIndex);
	bool ShouldSwapStackCount(const int32 RoomInClickedSlot, const int32 HoveredStackCount, const int32 MaxStackSize) const;
	void SwapStackCounts(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index);
	bool ShouldConsumeHoverItemStacks(const int32 RoomInClickedSlot, const int32 HoveredStackCount) const;
	void ConsumeHoverItemStacks(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index);
	bool ShouldFillInStack(const int32 RoomInClickedSlot, const int32 HoveredStackCount) const;
	void FillInStack(const int32 FillAmount, const int32 Remainder, const int32 Index);
	void CreateItemPopUp(const int32 GridIndex);
	void PutHoverItemBack();

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UBag_ItemPopUp> ItemPopUpClass;

	UPROPERTY()
	TObjectPtr<UBag_ItemPopUp> ItemPopUp;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UUserWidget> VisibleCursorWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UUserWidget> HiddenCursorWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> VisibleCursorWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> HiddenCursorWidget;

	UFUNCTION()
	void AddStacks(const FBag_SlotAvailabilityResult& Result);



	UFUNCTION()
	void OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent);

	UFUNCTION()
	void OnGridSlotClicked(int32 GridIndex, const FPointerEvent& MouseEvent);

	UFUNCTION()
	void OnGridSlotHovered(int32 GridIndex, const FPointerEvent& MouseEvent);

	UFUNCTION()
	void OnGridSlotUnhovered(int32 GridIndex, const FPointerEvent& MouseEvent);

	UFUNCTION()
	void OnPopUpMenuSplit(int32 SplitAmount, int32 Index);

	UFUNCTION()
	void OnPopUpMenuDrop(int32 Index);

	UFUNCTION()
	void OnPopUpMenuConsume(int32 Index);

	UFUNCTION()
	void OnInventoryMenuToggle(bool bOpen);

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

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UBag_HoverItem> HoverItemClass;

	UPROPERTY()
	TObjectPtr<UBag_HoverItem> HoverItem;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D ItemPopUpOffset;

	FBag_TileParameters TileParameters;
	FBag_TileParameters LastTileParameters;

	// 如果在有效位置点击网格，物品应该放置的索引
	int32 ItemDropIndex{ INDEX_NONE };
	Fbag_SpaceQueryResult CurrentQueryResult;
	bool bMouseWithinCanvas;
	bool bLastMouseWithinCanvas;
	int32 LastHighlightedIndex;
	FIntPoint LastHighlightedDimensions;
};



