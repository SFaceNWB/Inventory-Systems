// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Composite/Bag_Leaf.h"

void UBag_Leaf::ApplyFunction(FuncType Function)
{
	Function(this);
}
