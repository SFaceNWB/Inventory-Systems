// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Fragments/Bag_ItemFragment.h"
#include "Widgets/Composite/Bag_CompositeBase.h"
#include "Widgets/Composite/Bag_Leaf_Image.h"
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
