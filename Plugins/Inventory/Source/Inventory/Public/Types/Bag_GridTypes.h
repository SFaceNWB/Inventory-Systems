// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bag_GridTypes.generated.h"

UENUM(BlueprintType)
enum class EBag_ItemCategory : uint8
{
	Equipped,
	Consumable,
	Craftable,
	None
};
