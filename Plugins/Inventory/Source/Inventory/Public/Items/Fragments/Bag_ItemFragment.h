// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"

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
	virtual void Manifest() {}
private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "FragmentTags"))
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
struct FBag_LabeledNumberFragment : public FBag_InventoryItemFragment
{
	GENERATED_BODY()

	virtual void Assimilate(UBag_CompositeBase* Composite) const override;
	virtual void Manifest() override;
	float GetValue() const { return Value; }


	//当第一次显示时，这个片段将随机化。但装备和丢下时，应该保持相同的值，所以不应该发生随机化。
	bool bRandomizeOnManifest{ true };

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText Text_Label{};

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	float Value{ 0.f };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float Min{ 0 };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float Max{ 0 };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseLabel{ false };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseValue{ false };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MinFractionalDigits{ 1 };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxFractionalDigits{ 1 };
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

// 使用片段

USTRUCT(BlueprintType)
struct FBag_ConsumeModifier : public FBag_LabeledNumberFragment
{
	GENERATED_BODY()

	virtual void OnConsume(APlayerController* PC) {}

};

USTRUCT(BlueprintType)
struct FBag_ConsumableFragment : public FBag_InventoryItemFragment
{
	GENERATED_BODY()

	virtual void OnConsume(APlayerController* PC);
	virtual void Assimilate(UBag_CompositeBase* Composite) const override;
	virtual void Manifest() override;
private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FBag_ConsumeModifier>> ConsumeModifiers;
};


USTRUCT(blueprintType)
struct FBag_HealthPotionFragment : public FBag_ConsumeModifier
{
	GENERATED_BODY()

	virtual void OnConsume(APlayerController* PC) override;
};


USTRUCT(blueprintType)
struct FBag_ManaPotionFragment : public FBag_ConsumeModifier
{
	GENERATED_BODY()

	virtual void OnConsume(APlayerController* PC) override;
};