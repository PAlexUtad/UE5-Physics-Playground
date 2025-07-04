#include "BreakableTarget.h"
#include <GeometryCollection/GeometryCollectionComponent.h>

FOnBrokenSignature ABreakableTarget::OnBroken;

// Sets default values
ABreakableTarget::ABreakableTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMesh);

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	GeometryCollection->SetupAttachment(StaticMesh);
	m_IsBroken = false;
}

void ABreakableTarget::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GeometryCollection)
	{
		GeometryCollection->SetNotifyBreaks(true);
		GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableTarget::GeometryCollectionBroken);
	}
}

void ABreakableTarget::GeometryCollectionBroken(const FChaosBreakEvent& BreakEvent)
{
	if (!m_IsBroken)
	{
		m_IsBroken = true;
		OnBroken.Broadcast(this);
		GeometryCollection->SetNotifyBreaks(false);
	}
}

