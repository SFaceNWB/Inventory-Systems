// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Fragments/Bag_ItemFragment.h"
#include "Widgets/Composite/Bag_CompositeBase.h"
#include "Widgets/Composite/Bag_Leaf_Image.h"
#include "Widgets/Composite/Bag_Leaf_labeledValue.h"
#include "Widgets/Composite/Bag_Leaf_Text.h"

void FBag_InventoryItemFragment::Assimilate(UBag_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite)) return;
	Composite->Expand();
}

bool FBag_InventoryItemFragment::MatchesWidgetTag(const UBag_CompositeBase* Composite) const
{
	return Composite->GetFragmentTag().MatchesTagExact(GetFragmentTag());
}

void FBag_ImageFragment::Assimilate(UBag_CompositeBase* Composite) const
{
	FBag_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;

	UBag_Leaf_Image* Image = Cast<UBag_Leaf_Image>(Composite);
	if (!IsValid(Image)) return;

	Image->SetImage(Icon);
	Image->SetBoxSize(IconDimensions);
	Image->SetImageSize(IconDimensions);
}

void FBag_TextFragment::Assimilate(UBag_CompositeBase* Composite) const
{
	FBag_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;

	UBag_Leaf_Text* LeafText = Cast<UBag_Leaf_Text>(Composite);
	if (!IsValid(LeafText)) return;

	LeafText->SetText(FragmentText);
}

void FBag_LabeledNumberFragment::Assimilate(UBag_CompositeBase* Composite) const
{
	FBag_InventoryItemFragment::Assimilate(Composite);

	if (!MatchesWidgetTag(Composite)) return;

	UBag_Leaf_labeledValue* LabeledValue = Cast<UBag_Leaf_labeledValue>(Composite);
	if (!IsValid(LabeledValue)) return;

	LabeledValue->SetText_Label(Text_Label, bCollapseLabel);

	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits = MinFractionalDigits;
	Options.MaximumFractionalDigits = MaxFractionalDigits;

	LabeledValue->SetText_Value(FText::AsNumber(Value, &Options), bCollapseValue);
}

void FBag_LabeledNumberFragment::Manifest()
{
	FBag_InventoryItemFragment::Manifest();

	if (bRandomizeOnManifest)
	{
		Value = FMath::FRandRange(Min, Max);
	}
	bRandomizeOnManifest = false;
}

void FBag_ConsumableFragment::OnConsume(APlayerController* PC)
{
	for (auto& Modifier : ConsumeModifiers)
	{
		auto& ModifierRef = Modifier.GetMutable();
		ModifierRef.OnConsume(PC);
	}
}

void FBag_ConsumableFragment::Assimilate(UBag_CompositeBase* Composite) const
{
	FBag_InventoryItemFragment::Assimilate(Composite);
	for (const auto& Modifier : ConsumeModifiers)
	{
		const auto& ModifierRef = Modifier.Get();
		ModifierRef.Assimilate(Composite);
	}
}

void FBag_ConsumableFragment::Manifest()
{
	FBag_InventoryItemFragment::Manifest();
	for (auto& Modifier : ConsumeModifiers)
	{
		auto& ModifierRef = Modifier.GetMutable();
		ModifierRef.Manifest();
	}

}

void FBag_HealthPotionFragment::OnConsume(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Health  Potion consumed! Healing by: %f"), GetValue()));
}

void FBag_ManaPotionFragment::OnConsume(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Mana  Potion consumed! Healing by: %f"), GetValue()));
}
