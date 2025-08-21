// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bag_ProxyMesh.generated.h"

class UBag_EquipmentComponent;

UCLASS()
class INVENTORY_API ABag_ProxyMesh : public AActor
{
	GENERATED_BODY()
	
public:	
	ABag_ProxyMesh();
	USkeletalMeshComponent* GetMesh() const { return Mesh; }

protected:
	virtual void BeginPlay() override;

private:

	// 这是玩家控制器的角色网格
	TWeakObjectPtr<USkeletalMeshComponent> SourceMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBag_EquipmentComponent> EquipmentComponent;

	// 这是在背包中看见的角色代理网格
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh;

	FTimerHandle TimerForNextTick;
	void DelayedInitializeOwner();
	void DelayedInitialization();
};
