// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Composite/Bag_Composite.h"

#include "Blueprint/WidgetTree.h"

void UBag_Composite::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	WidgetTree->ForEachWidget(
		[this](UWidget* Widget)
		{
			if (UBag_CompositeBase* Composite = Cast<UBag_CompositeBase>(Widget); IsValid(Composite))
			{
				Children.Add(Composite);
				Composite->Collapse();
			}
		});
}

void UBag_Composite::ApplyFunction(FuncType Function)
{
	for (auto& Child : Children)
	{
		Child->ApplyFunction(Function);
	}
}

void UBag_Composite::Collapse()
{
	for (auto& Child : Children)
	{
		Child->Collapse();
	}
}
