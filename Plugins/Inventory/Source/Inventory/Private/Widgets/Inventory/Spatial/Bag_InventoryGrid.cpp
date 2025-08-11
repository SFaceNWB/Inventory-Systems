// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/Bag_InventoryGrid.h"

#include "Inventory.h"
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
#include "Widgets/Inventory/HoverItem/Bag_HoverItem.h"
#include "Widgets/ItemPopUp/Bag_ItemPopUp.h"

void UBag_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ConstructGrid();

	InventoryComponent = UBag_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this, &ThisClass::AddItem);
	InventoryComponent->OnStackChange.AddDynamic(this, &ThisClass::AddStacks);
}

void UBag_InventoryGrid::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const FVector2D CanvasPosition = UBag_WidgetUtils::GetWidgetPosition(CanvasPanel);
	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());

	if (CursorExitedCanvas(CanvasPosition, UBag_WidgetUtils::GetWidgetSize(CanvasPanel), MousePosition))
	{
		return;
	}

	UpdateTileParameters(CanvasPosition, MousePosition);
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
	SlottedItem->OnSlottedItemClicked.AddDynamic(this, &ThisClass::OnSlottedItemClicked);

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

bool UBag_InventoryGrid::IsRightClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
}

bool UBag_InventoryGrid::IsLeftClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;
}

void UBag_InventoryGrid::PickUp(UBag_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	//分配悬浮物品
	AssignHoverItem(ClickedInventoryItem, GridIndex, GridIndex);
	//移除从网格中的插槽物品
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);

}

void UBag_InventoryGrid::AssignHoverItem(UBag_InventoryItem* InventoryItem)
{
	if (!IsValid(HoverItem))
	{
		HoverItem = CreateWidget<UBag_HoverItem>(GetOwningPlayer(), HoverItemClass);
	}

	const FBag_GridFragment* GridFragment = GetFragment<FBag_GridFragment>(InventoryItem, FragmentTags::GridFragment);
	const FBag_ImageFragment* ImageFragment = GetFragment<FBag_ImageFragment>(InventoryItem, FragmentTags::IconFragment);
	if (!GridFragment || !ImageFragment)
	{
		return;
	}

	const FVector2D DrawSize = GetDrawSize(GridFragment);

	FSlateBrush IconBrush;
	IconBrush.SetResourceObject(ImageFragment->GetIcon());
	IconBrush.DrawAs = ESlateBrushDrawType::Image;
	IconBrush.ImageSize = DrawSize * UWidgetLayoutLibrary::GetViewportScale(this);

	HoverItem->SetImageBrush(IconBrush);
	HoverItem->SetGridDimensions(GridFragment->GetGridSize());
	HoverItem->SetInventoryItem(InventoryItem);
	HoverItem->SetIsStackable(InventoryItem->IsStackable());

	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, HoverItem);
}

void UBag_InventoryGrid::AssignHoverItem(UBag_InventoryItem* InventoryItem, const int32 GridIndex,
	const int32 PreviousGridIndex)
{
	AssignHoverItem(InventoryItem);
	HoverItem->SetPreviousGridIndex(PreviousGridIndex);
	HoverItem->UpdateStackCount(InventoryItem->IsStackable() ? GridSlots[GridIndex]->GetStackCount() : 0);
}

void UBag_InventoryGrid::RemoveItemFromGrid(UBag_InventoryItem* InventoryItem, const int32 GridIndex)
{
	const FBag_GridFragment* GridFragment = GetFragment<FBag_GridFragment>(InventoryItem, FragmentTags::GridFragment);
	if (!GridFragment)
	{
		return;
	}

	UBag_InventoryStatics::ForEach2D(GridSlots, GridIndex, GridFragment->GetGridSize(), Columns,
		[&](UBag_GridSlot* GridSlot)
		{
			GridSlot->SetInventoryItem(nullptr);
			GridSlot->SetUpperLeftIndex(INDEX_NONE);
			GridSlot->SetUnOccupiedTexture();
			GridSlot->SetAvailable(true);
			GridSlot->SetStackCount(0);
		});

	if (SlottedItems.Contains(GridIndex))
	{
		TObjectPtr<UBag_SlottedItem> FoundSlottedItem;
		SlottedItems.RemoveAndCopyValue(GridIndex, FoundSlottedItem);
		FoundSlottedItem->RemoveFromParent();
	}
}

void UBag_InventoryGrid::UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition)
{
	// 如果鼠标不在画布上，返回
	if (!bMouseWithinCanvas)
	{
		return;
	}

	// 计算瓷砖象限
	const FIntPoint HoveredCoordinates = CalculateHoveredCoordinates(CanvasPosition,MousePosition);

	LastTileParameters = TileParameters;
	TileParameters.TileCoordinates = HoveredCoordinates;
	TileParameters.TileIndex = UBag_WidgetUtils::GetIndexFromPosition(HoveredCoordinates, Columns);
	TileParameters.TileQuadrant = CalculateTileQuadrant(CanvasPosition, MousePosition);

	// 控制高亮/不高亮网格槽
	OnTileParametersUpdate(TileParameters);
}

FIntPoint UBag_InventoryGrid::CalculateHoveredCoordinates(const FVector2D& CanvasPosition,
	const FVector2D& MousePosition) const
{
	return  FIntPoint{
		static_cast<int32>(FMath::FloorToInt((MousePosition.X - CanvasPosition.X) / TileSize)),
		static_cast<int32>(FMath::FloorToInt((MousePosition.Y - CanvasPosition.Y) / TileSize))
	};
}

EBag_TileQuadrant UBag_InventoryGrid::CalculateTileQuadrant(const FVector2D& CanvasPosition,
	const FVector2D& MousePosition) const
{
	// 计算瓦片中的相对位置
	const float TileLocalX = FMath::Fmod(MousePosition.X - CanvasPosition.X, TileSize);
	const float TileLocalY = FMath::Fmod(MousePosition.Y - CanvasPosition.Y, TileSize);

	// 确定鼠标处于哪个象限
	const bool bIsTop = TileLocalY < TileSize / 2;
	const bool bIsLeft = TileLocalX < TileSize / 2;

	EBag_TileQuadrant HoveredTileQuadrant{EBag_TileQuadrant::None};
	if (bIsTop && bIsLeft)
	{
		HoveredTileQuadrant = EBag_TileQuadrant::TopLeft;
	}
	else if (bIsTop && !bIsLeft)
	{
		HoveredTileQuadrant = EBag_TileQuadrant::TopRight;
	}
	else if (!bIsTop && bIsLeft)
	{
		HoveredTileQuadrant = EBag_TileQuadrant::BottomLeft;
	}
	else if (!bIsTop && !bIsLeft)
	{
		HoveredTileQuadrant = EBag_TileQuadrant::BottomRight;
	}

	return HoveredTileQuadrant;
}

void UBag_InventoryGrid::OnTileParametersUpdate(const FBag_TileParameters& Parameters)
{
	if (!IsValid(HoverItem))
	{
		return;
	}

	// 获取鼠标悬停物品的尺寸
	const FIntPoint Dimensions = HoverItem->GetGridDimensions();
	// 计算高亮的起始坐标
	const FIntPoint StartingCoordinate = CalculateStartingCoordinate(Parameters.TileCoordinates, Dimensions, Parameters.TileQuadrant);
	ItemDropIndex = UBag_WidgetUtils::GetIndexFromPosition(StartingCoordinate, Columns);

	// 检查悬停坐标
	CurrentQueryResult = CheckHoverPosition(StartingCoordinate, Dimensions);
	if (CurrentQueryResult.bHasSpace)
	{
		HighlightSlots(ItemDropIndex, Dimensions);
		return;
	}
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);

	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		// 只有一个物品在这个空间内，可以交换或添加堆叠。
		const FBag_GridFragment* GridFragment = GetFragment<FBag_GridFragment>(CurrentQueryResult.ValidItem.Get(), FragmentTags::GridFragment);
		if (!GridFragment)
		{
			return;
		}
		ChangeHoverType(CurrentQueryResult.UpperLeftIndex, GridFragment->GetGridSize(), Ebag_GridSlotState::GrayedOut);
	}
}

FIntPoint UBag_InventoryGrid::CalculateStartingCoordinate(const FIntPoint& Coordinate, const FIntPoint& Dimensions,
	const EBag_TileQuadrant Quadrant) const
{
	const int32 HasEventWidth = Dimensions.X % 2 == 0 ? 1 : 0;
	const int32 HasEventHeight = Dimensions.Y % 2 == 0 ? 1 : 0;

	FIntPoint StartingCoordinate;
	switch (Quadrant)
	{
	case EBag_TileQuadrant::TopLeft:
		StartingCoordinate.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X);
		StartingCoordinate.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
		break;
	case EBag_TileQuadrant::TopRight:
		StartingCoordinate.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEventWidth;
		StartingCoordinate.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
		break;
	case EBag_TileQuadrant::BottomLeft:
		StartingCoordinate.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X);
		StartingCoordinate.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEventHeight;
		break;
	case EBag_TileQuadrant::BottomRight:
		StartingCoordinate.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEventWidth;
		StartingCoordinate.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEventHeight;
		break;
	default:
		UE_LOG(LogInventory, Error, TEXT("Invalid Quadrant."));
		return  FIntPoint(-1, -1);
	}
	return StartingCoordinate;
}

Fbag_SpaceQueryResult UBag_InventoryGrid::CheckHoverPosition(const FIntPoint& Position,
	const FIntPoint& Dimensions)
{
	Fbag_SpaceQueryResult Result;

	// 是否在网格范围内？
	if (!IsInGridBounds(UBag_WidgetUtils::GetIndexFromPosition(Position, Columns), Dimensions))
	{
		return Result;
	}
	Result.bHasSpace = true;

	// 如果多个索引被同一项占用, 检查是否有相同的左上角索引
	TSet<int32> OccupiedUpperLeftIndices;
	UBag_InventoryStatics::ForEach2D(GridSlots, UBag_WidgetUtils::GetIndexFromPosition(Position, Columns), Dimensions, Columns,
		[&](const UBag_GridSlot* GridSlot)
		{
			if (GridSlot->GetInventoryItem().IsValid())
			{
				OccupiedUpperLeftIndices.Add(GridSlot->GetUpperLeftIndex());
				Result.bHasSpace = false;
			}
		});

	// 如果有，是否只有一个障碍物？(可以交换吗?)
	if (OccupiedUpperLeftIndices.Num() == 1)
	{
		const int32 Index = *OccupiedUpperLeftIndices.CreateConstIterator();
		Result.ValidItem = GridSlots[Index]->GetInventoryItem();
		Result.UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	}
	return Result;
}

bool UBag_InventoryGrid::CursorExitedCanvas(const FVector2D& BoundaryPosition, const FVector2D& BoundarySize,
	const FVector2D& Location)
{
	bLastMouseWithinCanvas = bMouseWithinCanvas;
	bMouseWithinCanvas = UBag_WidgetUtils::IsWithBounds(BoundaryPosition, BoundarySize,Location);
	if (!bMouseWithinCanvas && bLastMouseWithinCanvas)
	{
		UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
		return true;
	}
	return false;
}

void UBag_InventoryGrid::HighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	if (!bMouseWithinCanvas)
	{
		return;
	}
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	UBag_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns,
		[&](UBag_GridSlot* GridSlot)
		{
			GridSlot->SetOccupiedTexture();
		});
	LastHighlightedDimensions = Dimensions;
	LastHighlightedIndex = Index;
}

void UBag_InventoryGrid::UnHighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	UBag_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns,
		[](UBag_GridSlot* GridSlot)
		{
			if (GridSlot->IsAvailable())
			{
				GridSlot->SetUnOccupiedTexture();
			}
			else
			{
				GridSlot->SetOccupiedTexture();
			}
		});
}

void UBag_InventoryGrid::ChangeHoverType(const int32 Index, const FIntPoint& Dimensions,
	Ebag_GridSlotState GridSlotState)
{
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedIndex);
	UBag_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns,
		[State = GridSlotState](UBag_GridSlot* GridSlot)
		{
			switch (State)
			{
			case Ebag_GridSlotState::Occupied:
				GridSlot->SetOccupiedTexture();
				break;
			case Ebag_GridSlotState::Unoccupied:
				GridSlot->SetUnOccupiedTexture();
				break;
			case Ebag_GridSlotState::GrayedOut:
				GridSlot->SetGrayedOutTexture();
				break;
			case Ebag_GridSlotState::Selected:
				GridSlot->SetSelectedTexture();
				break;
			}
		});

	LastHighlightedIndex = Index;
	LastHighlightedDimensions = Dimensions;
}

void UBag_InventoryGrid::PutDownOnIndex(const int32 Index)
{
	AddItemAtIndex(HoverItem->GetInventoryItem(), Index, HoverItem->IsStackable(), HoverItem->GetStackCount());
	UpdateGridSlots(HoverItem->GetInventoryItem(), Index, HoverItem->IsStackable(), HoverItem->GetStackCount());
	ClearHoverItem();
}

void UBag_InventoryGrid::ClearHoverItem()
{
	if (!IsValid(HoverItem))
	{
		return;
	}
	HoverItem->SetInventoryItem(nullptr);
	HoverItem->SetIsStackable(false);
	HoverItem->SetPreviousGridIndex(INDEX_NONE);
	HoverItem->UpdateStackCount(0);
	HoverItem->SetImageBrush(FSlateNoResource());

	HoverItem->RemoveFromParent();
	HoverItem = nullptr;

	ShowCursor();
}

UUserWidget* UBag_InventoryGrid::GetVisibleCursorWidget()
{
	if (!IsValid(GetOwningPlayer()))
	{
		return nullptr;
	}
	if (!IsValid(VisibleCursorWidget))
	{
		VisibleCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), VisibleCursorWidgetClass);
	}
	return VisibleCursorWidget;
}

UUserWidget* UBag_InventoryGrid::GetHiddenCursorWidget()
{
	if (!IsValid(GetOwningPlayer()))
	{
		return nullptr;
	}
	if (!IsValid(HiddenCursorWidget))
	{
		HiddenCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), VisibleCursorWidgetClass);
	}
	return HiddenCursorWidget;
}

bool UBag_InventoryGrid::IsSameStackable(const UBag_InventoryItem* ClickedInventoryItem) const
{
	const bool bIsSameItem = ClickedInventoryItem == HoverItem->GetInventoryItem();
	const bool bIsStackable = ClickedInventoryItem->IsStackable();
	return bIsSameItem && bIsStackable && HoverItem->GetItemType().MatchesTagExact(ClickedInventoryItem->GetItemManifest().GetItemType());
}

void UBag_InventoryGrid::SwapWithHoverItem(UBag_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	if (!IsValid(HoverItem))
	{
		return;
	}
	UBag_InventoryItem* TempInventoryItem = HoverItem->GetInventoryItem();
	const int32 TempStackCount = HoverItem->GetStackCount();
	const bool bTempIsStackable = HoverItem->IsStackable();
	// 保持相同的前一个网格索引
	AssignHoverItem(ClickedInventoryItem, GridIndex, HoverItem->GetPreviousGridIndex());
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
	AddItemAtIndex(TempInventoryItem, ItemDropIndex, bTempIsStackable, TempStackCount);
	UpdateGridSlots(TempInventoryItem, ItemDropIndex, bTempIsStackable, TempStackCount);
}

bool UBag_InventoryGrid::ShouldSwapStackCount(const int32 RoomInClickedSlot, const int32 HoveredStackCount,
	const int32 MaxStackSize) const
{
	return RoomInClickedSlot == 0 && HoveredStackCount < MaxStackSize;
}

void UBag_InventoryGrid::SwapStackCounts(const int32 ClickedStackCount, const int32 HoveredStackCount,
	const int32 Index)
{
	UBag_GridSlot* GridSlot = GridSlots[Index];
	GridSlot->SetStackCount(HoveredStackCount);

	UBag_SlottedItem* ClickedSlottedItem = SlottedItems.FindChecked(Index);
	ClickedSlottedItem->UpdateStackCount(HoveredStackCount);

	HoverItem->UpdateStackCount(ClickedStackCount);
}

bool UBag_InventoryGrid::ShouldConsumeHoverItemStacks(const int32 RoomInClickedSlot,
	const int32 HoveredStackCount) const
{
	return RoomInClickedSlot >= HoveredStackCount;
}

void UBag_InventoryGrid::ConsumeHoverItemStacks(const int32 ClickedStackCount, const int32 HoveredStackCount,
	const int32 Index)
{
	const int32 AmountToTransfer = HoveredStackCount;
	const int32 NewClickedStackCount = ClickedStackCount + AmountToTransfer;

	UBag_GridSlot* Gridslot = GridSlots[Index];
	Gridslot->SetStackCount(NewClickedStackCount);
	SlottedItems.FindChecked(Index)->UpdateStackCount(NewClickedStackCount);
	ClearHoverItem();
	ShowCursor();

	const FBag_GridFragment* GridFragment = Gridslot->GetInventoryItem()->GetItemManifest().GetFragmentOfType<FBag_GridFragment>();
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);
	HighlightSlots(Index, Dimensions);
}

bool UBag_InventoryGrid::ShouldFillInStack(const int32 RoomInClickedSlot, const int32 HoveredStackCount) const
{
	return RoomInClickedSlot < HoveredStackCount;
}

void UBag_InventoryGrid::FillInStack(const int32 FillAmount, const int32 Remainder, const int32 Index)
{
	UBag_GridSlot* Gridslot = GridSlots[Index];
	const int32 NewStackCount = Gridslot->GetStackCount() + FillAmount;

	Gridslot->SetStackCount(NewStackCount);

	UBag_SlottedItem* ClickedSlottedItem = SlottedItems.FindChecked(Index);
	ClickedSlottedItem->UpdateStackCount(NewStackCount);

	HoverItem->UpdateStackCount(Remainder);
}

void UBag_InventoryGrid::CreateItemPopUp(const int32 GridIndex)
{
	UBag_InventoryItem* RightClickedItem = GridSlots[GridIndex]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem))
	{
		return;
	}

	if (IsValid(GridSlots[GridIndex]->GetItemPopUp()))
	{
		return;
	}
	ItemPopUp = CreateWidget<UBag_ItemPopUp>(GetOwningPlayer(), ItemPopUpClass);
	GridSlots[GridIndex]->SetItemPopUp(ItemPopUp);

	OwningCanvasPanel->AddChild(ItemPopUp);
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(ItemPopUp);

	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
	CanvasSlot->SetPosition(MousePosition - ItemPopUpOffset);
	CanvasSlot->SetSize(ItemPopUp->GetBoxSize());

	const int32 SliderMax = GridSlots[GridIndex]->GetStackCount() - 1;
	if (RightClickedItem->IsStackable() && SliderMax > 0)
	{
		ItemPopUp->OnSplit.BindDynamic(this, &ThisClass::OnPopUpMenuSplit);
		ItemPopUp->SetSliderParams(SliderMax, FMath::Max(1, GridSlots[GridIndex]->GetStackCount() / 2));
	}
	else
	{
		ItemPopUp->CollapseSplitButton();
	}
	ItemPopUp->OnDrop.BindDynamic(this, &ThisClass::OnPopUpMenuDrop);

	if (RightClickedItem->IsConsumable())
	{
		ItemPopUp->OnConsume.BindDynamic(this, &ThisClass::OnPopUpMenuConsume);
	}
	else
	{
		ItemPopUp->CollapseConsumeButton();
	}
}

void UBag_InventoryGrid::DropItem()
{
	if (!IsValid(HoverItem))
	{
		return;
	}
	if (!IsValid(HoverItem->GetInventoryItem()))
	{
		return;
	}

	InventoryComponent->Server_DropItem(HoverItem->GetInventoryItem(), HoverItem->GetStackCount());

	ClearHoverItem();
	ShowCursor();
}

bool UBag_InventoryGrid::HasHoverItem() const
{
	return IsValid(HoverItem);
}

void UBag_InventoryGrid::ShowCursor()
{
	if (!IsValid(GetOwningPlayer()))
	{
		return;
	}
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetVisibleCursorWidget());
}

void UBag_InventoryGrid::HideCursor()
{
	if (!IsValid(GetOwningPlayer()))
	{
		return;
	}
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetHiddenCursorWidget());
}

void UBag_InventoryGrid::SetOwningCanvas(UCanvasPanel* OwningCanvas)
{
	OwningCanvasPanel = OwningCanvas;
}

void UBag_InventoryGrid::AddStacks(const FBag_SlotAvailabilityResult& Result)
{
	if (!MatchesCategory(Result.Item.Get()))
	{
		return;
	}
	for (const auto& Availability : Result.SlotAvailabilities)
	{
		if (Availability.bItemAtIndex)
		{
			const auto& GridSlot = GridSlots[Availability.Index];
			const auto& SlottedItem = SlottedItems.FindChecked(Availability.Index);
			SlottedItem->UpdateStackCount(GridSlot->GetStackCount() + Availability.AmountToFill);
			GridSlot->SetStackCount(GridSlot->GetStackCount() + Availability.AmountToFill);
		}
		else
		{
			AddItemAtIndex(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
			UpdateGridSlots(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
		}
	}
}

void UBag_InventoryGrid::OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	UBag_InventoryStatics::ItemUnhovered(GetOwningPlayer());

	check(GridSlots.IsValidIndex(GridIndex));
	UBag_InventoryItem* ClickedInventoryItem = GridSlots[GridIndex]->GetInventoryItem().Get();

	if (!IsValid(HoverItem) && IsLeftClick(MouseEvent))
	{
		PickUp(ClickedInventoryItem, GridIndex);
		return;
	}

	if (IsRightClick(MouseEvent))
	{
		CreateItemPopUp(GridIndex);
		return;
	}

	// 悬停的物品和点击的背包物品是同一类型吗，可以堆叠吗？
	if (IsSameStackable(ClickedInventoryItem))
	{
		const int32 ClickedStackCount = GridSlots[GridIndex]->GetStackCount();
		const FBag_StackableFragment* StackableFragment = ClickedInventoryItem->GetItemManifest().GetFragmentOfType<FBag_StackableFragment>();
		const int32 MaxStackSize = StackableFragment->GetMaxStackSize();
		const int32 RoomInClickedSlot = MaxStackSize - ClickedStackCount;
		const int32 HoveredStackCount = HoverItem->GetStackCount();
		// 是否应该交换堆叠数量？(点击物品剩余填充数 == 0 && 悬停物品堆叠量 < 最大堆叠)
		if (ShouldSwapStackCount(RoomInClickedSlot, HoveredStackCount, MaxStackSize))
		{
			SwapStackCounts(ClickedStackCount, HoveredStackCount, GridIndex);
			return;
		}

		// 是否应该消耗悬停物品的叠加？(点击物品剩余填充数 >= 悬停物品堆叠量)
		if (ShouldConsumeHoverItemStacks(RoomInClickedSlot, HoveredStackCount))
		{
			ConsumeHoverItemStacks(ClickedStackCount, HoveredStackCount, GridIndex);
			return;
		}

		// 是否应该填充点击物品的堆叠数？(不是消耗物品)
		if (ShouldFillInStack(RoomInClickedSlot, HoveredStackCount))
		{
			FillInStack(RoomInClickedSlot, HoveredStackCount - RoomInClickedSlot, GridIndex);
			return;
		}
		// 点击的槽位已经满了不作处理(也可以播放一个音乐)
		if (RoomInClickedSlot == 0)
		{
			return;
		}
	}
	// 交换悬停物品和点击的物品
	SwapWithHoverItem(ClickedInventoryItem, GridIndex);
}

void UBag_InventoryGrid::OnGridSlotClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (!IsValid(HoverItem))
	{
		return;
	}
	if (!GridSlots.IsValidIndex(ItemDropIndex))
	{
		return;
	}
	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		OnSlottedItemClicked(CurrentQueryResult.UpperLeftIndex, MouseEvent);
		return;
	}

	auto GridSlot = GridSlots[ItemDropIndex];
	if (!GridSlot->GetInventoryItem().IsValid())
	{
		PutDownOnIndex(ItemDropIndex);
	}
}

void UBag_InventoryGrid::OnGridSlotHovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem))
	{
		return;
	}

	UBag_GridSlot* GridSlot = GridSlots[GridIndex];
	if (GridSlot->IsAvailable())
	{
		GridSlot->SetOccupiedTexture();
	}
}

void UBag_InventoryGrid::OnGridSlotUnhovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem))
	{
		return;
	}

	UBag_GridSlot* GridSlot = GridSlots[GridIndex];
	if (GridSlot->IsAvailable())
	{
		GridSlot->SetUnOccupiedTexture();
	}
}

void UBag_InventoryGrid::OnPopUpMenuSplit(int32 SplitAmount, int32 Index)
{
	UBag_InventoryItem* RightClickedItem = GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem))
	{
		return;
	}
	if (!RightClickedItem->IsStackable())
	{
		return;
	}
	const int32 UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	UBag_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
	const int32 StackCount = UpperLeftGridSlot->GetStackCount();
	const int32 NewStackCount = StackCount - SplitAmount;

	UpperLeftGridSlot->SetStackCount(NewStackCount);
	SlottedItems.FindChecked(UpperLeftIndex)->UpdateStackCount(NewStackCount);

	AssignHoverItem(RightClickedItem, UpperLeftIndex, UpperLeftIndex);
	HoverItem->UpdateStackCount(SplitAmount);
}

void UBag_InventoryGrid::OnPopUpMenuDrop(int32 Index)
{
	UBag_InventoryItem* RightClickedItem = GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem))
	{
		return;
	}

	PickUp(RightClickedItem, Index);
	DropItem();
}

void UBag_InventoryGrid::OnPopUpMenuConsume(int32 Index)
{
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
			CanvasPanel->AddChild(GridSlot);

			const FIntPoint TilePosition(i, j);
			GridSlot->SetTileIndex(UBag_WidgetUtils::GetIndexFromPosition(TilePosition, Columns));

			UCanvasPanelSlot* GridCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(GridSlot);
			GridCPS->SetSize(FVector2D(TileSize));
			GridCPS->SetPosition(TilePosition * TileSize);

			GridSlots.Add(GridSlot);

			GridSlot->GridSlotClicked.AddDynamic(this, &ThisClass::OnGridSlotClicked);
			GridSlot->GridSlotHovered.AddDynamic(this, &ThisClass::OnGridSlotHovered);
			GridSlot->GridSlotUnhovered.AddDynamic(this, &ThisClass::OnGridSlotUnhovered);
		}
	}
}


bool UBag_InventoryGrid::MatchesCategory(const UBag_InventoryItem* Item) const
{
	return Item->GetItemManifest().GetItemCategory() == ItemCategory;
}

