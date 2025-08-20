// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/GridSlots/Bag_EquippedGridSlot.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "InventoryManagement/Utils/Bag_InventoryStatics.h"
#include "Items/Bag_InventoryItem.h"
#include "Items/Fragments/Bag_FragmentTags.h"
#include "Items/Fragments/Bag_ItemFragment.h"
#include "Widgets/Inventory/HoverItem/Bag_HoverItem.h"
#include "Widgets/Inventory/SlottedItems/Bag_EquippedSlottedItem.h"

void UBag_EquippedGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!IsAvailable())
	{
		return;
	}
	UBag_HoverItem* HoverItem = UBag_InventoryStatics::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem))
	{
		return;
	}
	if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
	{
		SetOccupiedTexture();
		Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UBag_EquippedGridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (!IsAvailable())
	{
		return;
	}
	UBag_HoverItem* HoverItem = UBag_InventoryStatics::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem))
	{
		return;
	}
	if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
	{
		SetUnOccupiedTexture();
		Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Visible);
	}
}

FReply UBag_EquippedGridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	EquippedGridSlotClicked.Broadcast(this,	EquipmentTypeTag);
	return FReply::Handled();
}

UBag_EquippedSlottedItem* UBag_EquippedGridSlot::OnItemEquipped(UBag_InventoryItem* Item,
	const FGameplayTag& EquipTypeTag, float TileSize)
{
	// 检查装备类型标签
	if (!EquipTypeTag.MatchesTagExact(EquipmentTypeTag))
	{
		return nullptr;
	};
	// 获取网格尺寸
	const FBag_GridFragment* GridFragment = GetFragment<FBag_GridFragment>(Item, FragmentTags::GridFragment);
	if (!GridFragment)
	{
		return nullptr;
	}
	const FIntPoint GridDimensions = GridFragment->GetGridSize();

	// 计算插槽物品的绘制大小
	const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2;
	const FVector2D DrawSize = GridDimensions * IconTileWidth;

	// 创建装备插槽物品小部件
	EquippedSlottedItem = CreateWidget<UBag_EquippedSlottedItem>(GetOwningPlayer(), EquippedSlottedItemClass);

	// 设置插槽对象的背包物品
	EquippedSlottedItem->SetInventoryItem(Item);

	// 设置插槽对象的装备类型标签
	EquippedSlottedItem->SetEquipmentTypeTag(EquipTypeTag);

	// 在插槽对象上隐藏堆叠计数小部件
	EquippedSlottedItem->UpdateStackCount(0);

	// 设置这个类的背包物品(Equipped Grid Slot)
	SetInventoryItem(Item);

	// 在装备插槽对象设置图像笔刷
	const FBag_ImageFragment* ImageFragment = GetFragment<FBag_ImageFragment>(Item, FragmentTags::IconFragment);
	if (!ImageFragment)
	{
		return nullptr;
	}
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = DrawSize;
	EquippedSlottedItem->SetImageBrush(Brush);

	// 将插槽对象作为一个小部件的子类添加到覆盖层
	Overlay_Root->AddChild(EquippedSlottedItem);
	FGeometry OverlayGeometry = Overlay_Root->GetCachedGeometry();
	auto OverlayPos = OverlayGeometry.Position;
	auto OverlaySize = OverlayGeometry.Size;

	const float LeftPadding = OverlaySize.X / 2.0f - DrawSize.X / 2.0f;
	const float TopPadding = OverlaySize.Y / 2.0f - DrawSize.Y / 2.0f;

	UOverlaySlot* OverlaySlot = UWidgetLayoutLibrary::SlotAsOverlaySlot(EquippedSlottedItem);
	OverlaySlot->SetPadding(FMargin(LeftPadding, TopPadding));

	// 返回装备插槽对象小部件
	return EquippedSlottedItem;
}
