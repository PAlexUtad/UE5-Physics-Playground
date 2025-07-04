// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BreakableTarget.generated.h"


class UGeometryCollectionComponent;
struct FChaosBreakEvent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBrokenSignature, ABreakableTarget*, Target);


UCLASS()
class PHYSICS_API ABreakableTarget : public AActor
{
	GENERATED_BODY()


public:	
	static FOnBrokenSignature OnBroken;
	ABreakableTarget();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UGeometryCollectionComponent* GeometryCollection;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	bool m_IsBroken = false;


public:	
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void GeometryCollectionBroken(const struct FChaosBreakEvent& BreakEvent);
};
