// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/Bag_HighlightableStaticMesh.h"

void UBag_HighlightableStaticMesh::Highlight_Implementation()
{
	SetOverlayMaterial(HighlightMaterial);
}

void UBag_HighlightableStaticMesh::UnHighlight_Implementation()
{
	SetOverlayMaterial(nullptr);
}
