// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/Bag_InventoryGrid.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "InventoryManagement/Components/Bag_InventoryComponent.h"
#include "InventoryManagement/Utils/Bag_InventoryStatics.h"
#include "Items/Bag_InventoryItem.h"
#include "Items/Components/Bag_ItemComponent.h"
#include "Items/Fragments/Bag_FragmentTags.h"
#include "Items/Fragments/Bag_ItemFragment.h"
#include "Items/Manifest/Bag_ItemManifest.h"
#include "Widgets/Inventory/GridSlots/Bag_GridSlot.h"
#include "Widgets/Inventory/SlottedItems/Bag_SlottedItem.h"
#include "Widgets/Utils/Bag_WidgetUtils.h"

void UBag_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ConstructGrid();

	InventoryComponent = UBag_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this, &ThisClass::AddItem);
}

FBag_SlotAvailabilityResult UBag_InventoryGrid::HasRoomForItem(const UBag_ItemComponent* ItemComponent)
{
	return HasRoomForItem(ItemComponent->GetItemManifest());
}

FBag_SlotAvailabilityResult UBag_InventoryGrid::HasRoomForItem(const UBag_InventoryItem* Item)
{
	return HasRoomForItem(Item->GetItemManifest());
}

FBag_SlotAvailabilityResult UBag_InventoryGrid::HasRoomForItem(const FBag_ItemManifest& Manifest)
{
	FBag_SlotAvailabilityResult Result;
	Result.TotalRoomToFill = 1;

	FBag_SlotAvailability SlotAvailability;
	SlotAvailability.AmountToFill = 1;
	SlotAvailability.Index = 0;

	Result.SlotAvailabilities.Add(MoveTemp(SlotAvailability));

	return Result;
}

void UBag_InventoryGrid::AddItemToIndices(const FBag_SlotAvailabilityResult& Result, UBag_InventoryItem* NewItem)
{
	for (const auto& Availability : Result.SlotAvailabilities)
	{
		AddItemAtIndex(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
	}
	
}

FVector2D UBag_InventoryGrid::GetDrawSize(const FBag_GridFragment* GridFragment) const
{
	const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2;
	return GridFragment->GetGridSize() * IconTileWidth;
}

void UBag_InventoryGrid::SetSlottedItemImage(const UBag_SlottedItem* SlottedItem, const FBag_GridFragment* GridFragment,
	const FBag_ImageFragment* ImageFragment) const
{
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = GetDrawSize(GridFragment);
	SlottedItem->SetImageBrush(Brush);
}

void UBag_InventoryGrid::AddItemAtIndex(UBag_InventoryItem* Item, const int32 Index, const bool bStackable,
	const int32 StackAmount)
{
	const FBag_GridFragment* GridFragment = GetFragment<FBag_GridFragment>(Item, FragmentTags::GridFragment);
	const FBag_ImageFragment* ImageFragment = GetFragment<FBag_ImageFragment>(Item, FragmentTags::IconFragment);
	if (!GridFragment || !ImageFragment)
	{
		return;
	}
	UBag_SlottedItem* SlottedItem = CreateSlottedItem(Item, bStackable, StackAmount, GridFragment, ImageFragment, Index);
	AddSlottedItemToCanvas(Index, GridFragment, SlottedItem);
	SlottedItems.Add(Index, SlottedItem);
}

UBag_SlottedItem* UBag_InventoryGrid::CreateSlottedItem(UBag_InventoryItem* Item, const bool bStackable,
	const int32 StackAmount, const FBag_GridFragment* GridFragment, const FBag_ImageFragment* ImageFragment,
	const int32 Index)
{
	UBag_SlottedItem* SlottedItem = CreateWidget<UBag_SlottedItem>(GetOwningPlayer(), SlottedItemClass);
	SlottedItem->SetInventoryItem(Item);
	SetSlottedItemImage(SlottedItem, GridFragment, ImageFragment);
	SlottedItem->SetGridIndex(Index);

	return SlottedItem;
}

void UBag_InventoryGrid::AddSlottedItemToCanvas(const int32 Index, const FBag_GridFragment* GridFragment,
	UBag_SlottedItem* SlottedItem) const
{
	CanvasPanel->AddChild(SlottedItem);
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(SlottedItem);
	CanvasSlot->SetSize(GetDrawSize(GridFragment));
	const FVector2D DrawPosition = UBag_WidgetUtils::GetPositionFromIndex(Index, Columns) * TileSize;
	const FVector2D DrawPositionWithPadding = DrawPosition + FVector2D(GridFragment->GetGridPadding());
	CanvasSlot->SetPosition(DrawPositionWithPadding);
}


void UBag_InventoryGrid::AddItem(UBag_InventoryItem* Item)
{
	if (!MatchesCategory(Item))
	{
		return;
	}

	FBag_SlotAvailabilityResult Result = HasRoomForItem(Item);

	AddItemToIndices(Result, Item);
}

void UBag_InventoryGrid::ConstructGrid()
{
	GridSlots.Reserve(Rows * Columns);

	for (int32 j = 0; j < Rows; ++j)
	{
		for (int32 i = 0; i < Columns; ++i)
		{
			UBag_GridSlot* GridSlot = CreateWidget<UBag_GridSlot>(this, GridSlotClass);
			CanvasPanel->AddChildToCanvas(GridSlot);

			const FIntPoint TilePosition(i, j);
			GridSlot->SetTileIndex(UBag_WidgetUtils::GetIndexFromPosition(TilePosition, Columns));

			UCanvasPanelSlot* GridCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(GridSlot);
			GridCPS->SetSize(FVector2D(TileSize));
			GridCPS->SetPosition(TilePosition * TileSize);

			GridSlots.Add(GridSlot);
		}
	}
}


bool UBag_InventoryGrid::MatchesCategory(const UBag_InventoryItem* Item) const
{
	return Item->GetItemManifest().GetItemCategory() == ItemCategory;
}

