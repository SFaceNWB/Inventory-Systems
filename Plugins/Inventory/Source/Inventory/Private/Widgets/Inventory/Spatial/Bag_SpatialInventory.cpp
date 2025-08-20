// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/Bag_SpatialInventory.h"
#include "Widgets/Inventory/Spatial/Bag_InventoryGrid.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "InventoryManagement/Utils/Bag_InventoryStatics.h"
#include "Inventory.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Items/Bag_InventoryItem.h"
#include "Widgets/ItemDescription/Bag_ItemDescription.h"
#include "Blueprint/WidgetTree.h"
#include "Widgets/Inventory/GridSlots/Bag_EquippedGridSlot.h"

void UBag_SpatialInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Equipped->OnClicked.AddDynamic(this, &ThisClass::ShowEquipped);
	Button_Consumables->OnClicked.AddDynamic(this, &ThisClass::ShowConsumables);
	Button_Craftables->OnClicked.AddDynamic(this, &ThisClass::ShowCraftables);

	Grid_Equipped->SetOwningCanvas(CanvasPanel);
	Grid_Consumables->SetOwningCanvas(CanvasPanel);
	Grid_Craftables->SetOwningCanvas(CanvasPanel);

	ShowEquipped();

	WidgetTree->ForEachWidget(
		[this](UWidget* Widget)
		{
			UBag_EquippedGridSlot* EquippedGridSlot = Cast<UBag_EquippedGridSlot>(Widget);
			if (IsValid(EquippedGridSlot))
			{
				EquippedGridSlots.Add(EquippedGridSlot);
				EquippedGridSlot->EquippedGridSlotClicked.AddDynamic(this, &ThisClass::EquippedGridSlotClicked);
			}

		});
}

FReply UBag_SpatialInventory::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	ActiveGrid->DropItem();
	return FReply::Handled();
}

void UBag_SpatialInventory::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(ItemDescription))
	{
		return;
	}
	SetItemDescriptionSizeAndPosition(ItemDescription, CanvasPanel);

}

FBag_SlotAvailabilityResult UBag_SpatialInventory::HasRoomForItem(UBag_ItemComponent* ItemComponent) const
{
	switch (UBag_InventoryStatics::GetItemCategoryFromItemComponent(ItemComponent))
	{
	case EBag_ItemCategory::Equipped:
		return Grid_Equipped->HasRoomForItem(ItemComponent);
	case EBag_ItemCategory::Consumable:
		return Grid_Consumables->HasRoomForItem(ItemComponent);
	case EBag_ItemCategory::Craftable:
		return Grid_Craftables->HasRoomForItem(ItemComponent);
	default:
		UE_LOG(LogInventory, Error, TEXT("ItemComponent doesn't have a valid Item Category."));
		return FBag_SlotAvailabilityResult();
	}
}

void UBag_SpatialInventory::OnItemHovered(UBag_InventoryItem* Item)
{
	const auto& Manifest = Item->GetItemManifest();
	UBag_ItemDescription* DescriptionWidget = GetItemDescription();
	DescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);

	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimer);

	FTimerDelegate DescriptionTimerDelegate;
	DescriptionTimerDelegate.BindLambda(
		[this, &Manifest, DescriptionWidget]()
		{
			Manifest.AssimilateInventoryFragments(DescriptionWidget);
			GetItemDescription()->SetVisibility(ESlateVisibility::HitTestInvisible);
		});
	GetOwningPlayer()->GetWorldTimerManager().SetTimer(DescriptionTimer, DescriptionTimerDelegate, DescriptionTimerDelay, false);
}

void UBag_SpatialInventory::OnItemUnhovered()
{
	GetItemDescription()->SetVisibility(ESlateVisibility::Collapsed);
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimer);
}

bool UBag_SpatialInventory::HasHoverItem() const
{
	if (Grid_Equipped->HasHoverItem())
	{
		return true;
	}
	if (Grid_Consumables->HasHoverItem())
	{
		return true;
	}
	if (Grid_Craftables->HasHoverItem())
	{
		return true;
	}
	return false;
}

UBag_HoverItem* UBag_SpatialInventory::GetHoverItem() const
{
	if (!ActiveGrid.IsValid())
	{
		return nullptr;
	}
	return ActiveGrid->GetHoverItem();
}

UBag_ItemDescription* UBag_SpatialInventory::GetItemDescription()
{
	if (!IsValid(ItemDescription))
	{
		ItemDescription = CreateWidget<UBag_ItemDescription>(GetOwningPlayer(), ItemDescriptionClass);
		CanvasPanel->AddChild(ItemDescription);
	}
	return ItemDescription;
}

void UBag_SpatialInventory::ShowEquipped()
{
	SetActiveGrid(Grid_Equipped,Button_Equipped);
}

void UBag_SpatialInventory::ShowConsumables()
{
	SetActiveGrid(Grid_Consumables,Button_Consumables);
}

void UBag_SpatialInventory::ShowCraftables()
{
	SetActiveGrid(Grid_Craftables,Button_Craftables);
}

void UBag_SpatialInventory::EquippedGridSlotClicked(UBag_EquippedGridSlot* EquippedGridSlot,
	const FGameplayTag& EquipmentTypeTag)
{
	// 检查是否可以装备悬停物品
	// 创建一个装备插槽物品并将其添加到装备网格插槽中
	// 清除悬停物品
	// 让服务器知道装备了这个物品(特别是卸下物品)
}

void UBag_SpatialInventory::DisableButton(UButton* Button)
{
	Button_Equipped->SetIsEnabled(true);
	Button_Consumables->SetIsEnabled(true);
	Button_Craftables->SetIsEnabled(true);
	Button->SetIsEnabled(false);
}

void UBag_SpatialInventory::SetActiveGrid(UBag_InventoryGrid* Grid, UButton* Button)
{
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->HideCursor();
	}
	ActiveGrid = Grid;
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->ShowCursor();
	}
	DisableButton(Button);
	Switcher->SetActiveWidget(Grid);
}

void UBag_SpatialInventory::SetItemDescriptionSizeAndPosition(UBag_ItemDescription* Description,
	UCanvasPanel* Canvas) const
{
	UCanvasPanelSlot* ItemDescripttionCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(Description);
	if (!IsValid(ItemDescripttionCPS))
	{
		return;
	}
	const FVector2D ItemDescriptionSize = Description->GetDesiredSize();
	ItemDescripttionCPS->SetSize(ItemDescriptionSize);

	FVector2D ClampedPosition = UBag_WidgetUtils::GetClampedWidgetPosition(
		UBag_WidgetUtils::GetWidgetSize(Canvas),
		ItemDescriptionSize,
		UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer()));
	ItemDescripttionCPS->SetPosition(ClampedPosition);
}
