// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/Bag_InventoryGrid.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "InventoryManagement/Components/Bag_InventoryComponent.h"
#include "InventoryManagement/Utils/Bag_InventoryStatics.h"
#include "Items/Bag_InventoryItem.h"
#include "Widgets/Inventory/GridSlots/Bag_GridSlot.h"
#include "Widgets/Utils/Bag_WidgetUtils.h"

void UBag_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ConstructGrid();

	InventoryComponent = UBag_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this, &ThisClass::AddItem);
}

void UBag_InventoryGrid::AddItem(UBag_InventoryItem* Item)
{
	if (!MatchesCategory(Item))
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("InventoryGrid::AddItem"));
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
