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

	// 确认这个物品是否可以叠加
	const FBag_StackableFragment* StackableFragment = Manifest.GetFragmentOfType<FBag_StackableFragment>();
	Result.bStackable = StackableFragment != nullptr;

	// 确认要添加多少堆叠物品。
	const int32 MaxStackSize = StackableFragment ? StackableFragment->GetMaxStackSize() : 1;
	int32 AmountToFill = StackableFragment ? StackableFragment->GetStackCount() : 1;

	TSet<int32> CheckIndices;
	// 对每个网格槽：
	for (const auto& GridSlot : GridSlots)
	{
		// 如果没有更多需要填充的物品，跳出循环
		if (AmountToFill == 0)
		{
			break;
		}

		// 这个索引是否被占用？
		if (IsIndexClaimed(CheckIndices, GridSlot->GetIndex()))
		{
			continue;
		}
		// 是否超出网格边界
		if (!IsInGridBounds(GridSlot->GetIndex(), GetItemDimensions(Manifest)))
		{
			continue;
		}

		// 这个物品是否可以放在这里？
		TSet<int32> TentativelyClaimed;
		if (!HasRoomAtIndex(GridSlot, GetItemDimensions(Manifest), CheckIndices, TentativelyClaimed, Manifest.GetItemType(), MaxStackSize))
		{
			continue;
		}

		// 填充多少？
		const int32 AmountToFillInSlot = DetermineFillAmoutForSlot(Result.bStackable, MaxStackSize, AmountToFill, GridSlot);
		if (AmountToFillInSlot == 0)
		{
			continue;
		}
		CheckIndices.Append(TentativelyClaimed);

		// 更新填充数量
		Result.TotalRoomToFill += AmountToFillInSlot;
		Result.SlotAvailabilities.Emplace(
			FBag_SlotAvailability
			{
				HasValidItem(GridSlot) ? GridSlot->GetUpperLeftIndex() : GridSlot->GetIndex(),
				Result.bStackable ? AmountToFillInSlot : 0,
				HasValidItem(GridSlot)
			});

		AmountToFill -= AmountToFillInSlot;

		// 剩下数量是多少？
		Result.Remainder = AmountToFill;
		if (AmountToFill == 0)
		{
			return Result;
		}
	}
	return Result;
}

void UBag_InventoryGrid::AddItemToIndices(const FBag_SlotAvailabilityResult& Result, UBag_InventoryItem* NewItem)
{
	for (const auto& Availability : Result.SlotAvailabilities)
	{
		AddItemAtIndex(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
		UpdateGridSlots(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
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
	SlottedItem->SetIsStackable(bStackable);
	const int32 StackUpdateAmount = bStackable ? StackAmount : 0;
	SlottedItem->UpdateStackCount(StackUpdateAmount);

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

void UBag_InventoryGrid::UpdateGridSlots(UBag_InventoryItem* NewItem, int32 Index, bool bStackableItem, const int32 StackAmount)
{
	check(GridSlots.IsValidIndex(Index));

	if (bStackableItem)
	{
		GridSlots[Index]->SetStackCount(StackAmount);
	}

	const FBag_GridFragment* GridFragment = GetFragment<FBag_GridFragment>(NewItem, FragmentTags::GridFragment);
	const FIntPoint Dimension = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);

	UBag_InventoryStatics::ForEach2D(GridSlots, Index, Dimension, Columns, 
		[&](UBag_GridSlot* GridSlot)
		{
			GridSlot->SetInventoryItem(NewItem);
			GridSlot->SetUpperLeftIndex(Index);
			GridSlot->SetOccupiedTexture();
			GridSlot->SetAvailable(false);
		});
}

bool UBag_InventoryGrid::IsIndexClaimed(const TSet<int32>& CheckIndices, const int32 Index) const
{
	return CheckIndices.Contains(Index);
}

bool UBag_InventoryGrid::HasRoomAtIndex(const UBag_GridSlot* GridSlot, const FIntPoint& Dimension, 
	const TSet<int32>& CheckIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, 
	const int32 MaxStackSize)
{
	// 这个索引是否可以容纳这个物品？
	bool bHasRoomAtIndex = true;

	UBag_InventoryStatics::ForEach2D(GridSlots, GridSlot->GetIndex(), Dimension, Columns,
		[&](const UBag_GridSlot* SubGridSlot)
		{
			// 检查其他重要条件 -- 循环一个二维数组
			if (CheckSlotConstraints(GridSlot ,SubGridSlot, CheckIndices, OutTentativelyClaimed, ItemType, MaxStackSize))
			{
				OutTentativelyClaimed.Add(SubGridSlot->GetIndex());
			}
			else
			{
				bHasRoomAtIndex = false;
			}
		});

	return bHasRoomAtIndex;
}

FIntPoint UBag_InventoryGrid::GetItemDimensions(const FBag_ItemManifest& Manifest) const
{
	const FBag_GridFragment* GridFragment = Manifest.GetFragmentOfType<FBag_GridFragment>();
	return GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);
}

bool UBag_InventoryGrid::CheckSlotConstraints(const UBag_GridSlot* GridSlot, const UBag_GridSlot* SubGridSlot, 
	const TSet<int32>& CheckIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, 
	const int32 MaxStackSize) const
{
	// 索引是否被声明？
	if (IsIndexClaimed(CheckIndices, SubGridSlot->GetIndex()))
	{
		return false;
	}

	// 物品是否有效？
	if (!HasValidItem(SubGridSlot))
	{
		OutTentativelyClaimed.Add(SubGridSlot->GetIndex());
		return true;
	}

	// 这个网格插槽是否是左上角的索引第一个插槽
	if (!IsUpperLeftSlot(GridSlot, SubGridSlot))
	{
		return false;
	}

	// 物品是否与添加的物品为同类型？
	const UBag_InventoryItem* SubItem = SubGridSlot->GetInventoryItem().Get();
	if (!SubItem->IsStackable())
	{
		return false;
	}

	// 如果是，它可以叠加吗？
	if (!DoesItemTypeMatch(SubItem, ItemType))
	{
		return false;
	}

	// 如果可以叠加，插槽是否到达最大堆叠数量？
	if (GridSlot->GetStackCount() >= MaxStackSize)
	{
		return false;
	}
	return true;
}

bool UBag_InventoryGrid::HasValidItem(const UBag_GridSlot* GridSlot) const
{
	return GridSlot->GetInventoryItem().IsValid();
}

bool UBag_InventoryGrid::IsUpperLeftSlot(const UBag_GridSlot* GridSlot, const UBag_GridSlot* SubGridSlot) const
{
	return SubGridSlot->GetUpperLeftIndex() == GridSlot->GetIndex();
}

bool UBag_InventoryGrid::DoesItemTypeMatch(const UBag_InventoryItem* SubItem, const FGameplayTag& ItemType) const
{
	return SubItem->GetItemManifest().GetItemType().MatchesTagExact(ItemType);
}

bool UBag_InventoryGrid::IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimensions) const
{
	if (StartIndex < 0 || StartIndex >= GridSlots.Num())
	{
		return false;
	}
	const int32 EndColumn = (StartIndex % Columns) + ItemDimensions.X;
	const int32 EndRow = (StartIndex / Columns) + ItemDimensions.Y;
	return EndColumn <= Columns && EndRow <= Rows;
}

int32 UBag_InventoryGrid::DetermineFillAmoutForSlot(const bool bStackable, const int32 MaxStackSize,
	const int32 AmountToFill, const UBag_GridSlot* GridSlot) const
{
	// 计算插槽的空间
	const int32 RoomInSlot = MaxStackSize - GetStackAmount(GridSlot);
	// 如果可以堆叠，计算填充数量和插槽空间的最小值
	return bStackable ? FMath::Min(RoomInSlot, AmountToFill) : 1;
}

int32 UBag_InventoryGrid::GetStackAmount(const UBag_GridSlot* GridSlot) const
{
	int32 CurrentSlotStackCount = GridSlot->GetStackCount();
	// 如果处于一个不可堆叠的插槽，获取实际的可堆叠计数的插槽
	if (const int32 UpperLeftIndex = GridSlot->GetUpperLeftIndex(); UpperLeftIndex != INDEX_NONE)
	{
		UBag_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
		CurrentSlotStackCount = UpperLeftGridSlot->GetStackCount();
	}
	return CurrentSlotStackCount;
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

