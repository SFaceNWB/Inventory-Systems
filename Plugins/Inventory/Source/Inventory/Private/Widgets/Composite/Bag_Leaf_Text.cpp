// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Composite/Bag_Leaf_Text.h"

#include "Components/TextBlock.h"

void UBag_Leaf_Text::SetText(const FText& Text) const
{
	Text_LeafText->SetText(Text);
}

void UBag_Leaf_Text::NativePreConstruct()
{
	Super::NativePreConstruct();

	FSlateFontInfo FontInfo = Text_LeafText->GetFont();
	FontInfo.Size = FontSize;

	Text_LeafText->SetFont(FontInfo);
}
