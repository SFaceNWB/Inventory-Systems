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
	const FBag_GridFragment* GridFragment = GetFragment<FBag_GridFragment>(NewItem, FragmentTags::GridFragment);
	const FBag_ImageFragment* ImageFragment = GetFragment<FBag_ImageFragment>(NewItem, FragmentTags::IconFragment);
	if (!GridFragment || !ImageFragment)
	{
		return;
	}
	// 创建一个小部件并添加到网格中
	// 存放新的部件在一个容器中
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
