// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Bag_SlottedItem.generated.h"

class UBag_InventoryItem;
class UImage;

UCLASS()
class INVENTORY_API UBag_SlottedItem : public UUserWidget
{
	GENERATED_BODY()
public:

	void SetIsStackable(bool bStackable) { bIsStackable = bStackable; }
	bool IsStackable() const { return bIsStackable; }
	UImage* GetImageIcon() const { return Image_Icon; }
	void SetGridIndex(int32 Index) { GridIndex = Index; }
	int32 GetGridIndex() const { return GridIndex; }
	void SetGridDimensions(const FIntPoint& Dimensions) { GridDimensions = Dimensions; }
	FIntPoint GetGridDimensions() const { return GridDimensions; }
	void SetInventoryItem(UBag_InventoryItem* Item);
	UBag_InventoryItem* GetInventoryItem() const { return InventoryItem.Get(); }
	void SetImageBrush(const FSlateBrush& Brush) const;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	int32 GridIndex;
	FIntPoint GridDimensions;
	TWeakObjectPtr<UBag_InventoryItem> InventoryItem;
	bool bIsStackable{ false };
};
