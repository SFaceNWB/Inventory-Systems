// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CharacterDisplay/Bag_CharacterDisplay.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "EquipmentManagement/ProxyMesh/Bag_ProxyMesh.h"
#include "Kismet/GameplayStatics.h"

FReply UBag_CharacterDisplay::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	CurrentPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	LastPosition = CurrentPosition;

	bIsDragging = true;
	return FReply::Handled();
}

FReply UBag_CharacterDisplay::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bIsDragging = false;
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UBag_CharacterDisplay::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	bIsDragging = false;
}

void UBag_CharacterDisplay::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, ABag_ProxyMesh::StaticClass(), Actors);

	if (!Actors.IsValidIndex(0))
	{
		return;
	}
	ABag_ProxyMesh* ProxyMesh = Cast<ABag_ProxyMesh>(Actors[0]);
	if (!IsValid(ProxyMesh))
	{
		return;
	}
	Mesh = ProxyMesh->GetMesh();

}

void UBag_CharacterDisplay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!bIsDragging)
	{
		return;
	}
	LastPosition = CurrentPosition;
	CurrentPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	const float HorizontalDelta = LastPosition.X - CurrentPosition.X;
	if (!Mesh.IsValid())
	{
		return;
	}
	Mesh->AddRelativeRotation(FRotator(0.0f, HorizontalDelta, 0.0f));
}
