// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "Bag_ItemFragment.generated.h"

USTRUCT(BlueprintType)
struct FBag_ItemFragment
{
	GENERATED_BODY()

	FBag_ItemFragment(){}
	FBag_ItemFragment(const FBag_ItemFragment&) = default;
	FBag_ItemFragment& operator=(const FBag_ItemFragment&) = default;
	FBag_ItemFragment(FBag_ItemFragment&&) = default;
	FBag_ItemFragment& operator=(FBag_ItemFragment&&) = default;
	virtual ~FBag_ItemFragment() {}

	FGameplayTag GetFragmentTag() const { return FragmentTag; }
	void SetFragmentTag(FGameplayTag Tag) { FragmentTag = Tag; }
private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag FragmentTag = FGameplayTag::EmptyTag;
};

/*
 * 专门用于同化到小部件中的物品片段
 */
class UBag_CompositeBase;
USTRUCT(BlueprintType)
struct FBag_InventoryItemFragment : public FBag_ItemFragment
{
	GENERATED_BODY()

	virtual void Assimilate(UBag_CompositeBase* Composite) const;
protected:
	bool MatchesWidgetTag(const UBag_CompositeBase* Composite) const;
};

USTRUCT(BlueprintType)
struct FBag_GridFragment : public FBag_ItemFragment
{
	GENERATED_BODY()

	FIntPoint GetGridSize() const { return GridSize; }
	void SetGridSize(FIntPoint Size) { GridSize = Size; }
	float GetGridPadding() const { return GridPadding; }
	void SetGridPadding(float Padding) { GridPadding = Padding; }
private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FIntPoint GridSize{ 1, 1 };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float GridPadding{ 0.0f };
};

USTRUCT(BlueprintType)
struct FBag_ImageFragment : public FBag_InventoryItemFragment
{
	GENERATED_BODY()

	UTexture2D* GetIcon() const { return Icon; }
	virtual void Assimilate(UBag_CompositeBase* Composite) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UTexture2D> Icon{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D IconDimensions{ 44.0f, 44.0f };
};

USTRUCT(BlueprintType)
struct FBag_TextFragment : public FBag_InventoryItemFragment
{
	GENERATED_BODY()

	FText GetText() const { return FragmentText; }
	void SetText(const FText& Text) { FragmentText = Text; }
	virtual void Assimilate(UBag_CompositeBase* Composite) const override;

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText FragmentText;
};

USTRUCT(BlueprintType)
struct FBag_StackableFragment : public FBag_ItemFragment
{
	GENERATED_BODY()

	int32 GetMaxStackSize() const { return MaxStackSize; }
	int32 GetStackCount() const { return StackCount; }
	void SetStackCount(int32 Count) { StackCount = Count; }

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxStackSize{ 1 };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 StackCount{ 1 };
};






