// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Composite/Bag_Leaf_labeledValue.h"

#include "Components/TextBlock.h"

void UBag_Leaf_labeledValue::SetText_Label(const FText& Text, bool bCollapse) const
{
	if (bCollapse)
	{
		Text_Label->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	Text_Label->SetVisibility(ESlateVisibility::Visible);
	Text_Label->SetText(Text);
}

void UBag_Leaf_labeledValue::SetText_Value(const FText& Text, bool bCollapse) const
{
	if (bCollapse)
	{
		Text_Value->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	Text_Value->SetVisibility(ESlateVisibility::Visible);
	Text_Value->SetText(Text);
}

void UBag_Leaf_labeledValue::NativePreConstruct()
{
	Super::NativePreConstruct();

	FSlateFontInfo FontInfo_Label = Text_Label->GetFont();
	FontInfo_Label.Size = FontSize_Label;

	Text_Label->SetFont(FontInfo_Label);

	FSlateFontInfo FontInfo_Value = Text_Value->GetFont();
	FontInfo_Value.Size = FontSize_Value;

	Text_Value->SetFont(FontInfo_Value);
}
