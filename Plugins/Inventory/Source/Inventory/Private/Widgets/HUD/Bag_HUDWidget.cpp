// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/HUD/Bag_HUDWidget.h"

#include "InventoryManagement/Components/Bag_InventoryComponent.h"
#include "InventoryManagement/Utils/Bag_InventoryStatics.h"
#include "Widgets/HUD/Bag_InfoMessage.h"

void UBag_HUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UBag_InventoryComponent* InventoryComponent = UBag_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->NoRoomInInventory.AddDynamic(this, &ThisClass::OnNoRoom);
	}
}

void UBag_HUDWidget::OnNoRoom()
{
	if (!IsValid(InfoMessage))
	{
		return;
	}
	InfoMessage->SetMessage(FText::FromString("No Room In Inventory"));

}
