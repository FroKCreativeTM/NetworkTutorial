// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkTutorialProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ANetworkTutorialProjectile::ANetworkTutorialProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 이 액터는 replicate가 가능합니다.
	bReplicates = true;

	// 컴포넌트에 대한 초기화를 진행합니다.
	// 먼저 sphere 컴포넌트입니다.
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SphereComponent->InitSphereRadius(37.5f);
	SphereComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = SphereComponent;

	// 서버 컴퓨터라면
	// 오직 서버만 Role == ROLE_Authority 이다.(Docs)
	if (GetLocalRole() == ROLE_Authority)
	{
		SphereComponent->OnComponentHit.AddDynamic(this, &ANetworkTutorialProjectile::OnProjectileImpact);
	}

	// 이후 static 메시를 붙입니다.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMesh->SetupAttachment(RootComponent);

	// 위치와 스케일을 지정합니다.
	if (DefaultMesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(DefaultMesh.Object);
		StaticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -37.5f));
		StaticMesh->SetRelativeScale3D(FVector(0.75f, 0.75f, 0.75f));
	}

	// 그리고 파티클 시스템을 만듭니다.
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DefaultExplosionEffect.Succeeded())
	{
		ExplosionEffect = DefaultExplosionEffect.Object;
	}

	// 투사체 움직임에 대한 컴포넌트를 생성하고 이를 설정합니다.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);
	ProjectileMovementComponent->InitialSpeed = 1500.0f;
	ProjectileMovementComponent->MaxSpeed = 1500.0f;	
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	// 마지막으로 데미지 타입에 대해 설정합니다.
	DamageType = UDamageType::StaticClass();
	Damage = 10.0f;
}

// Called when the game starts or when spawned
void ANetworkTutorialProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANetworkTutorialProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANetworkTutorialProjectile::Destroyed()
{
	// 현재 위치를 받는다.
	FVector spawnLocation = GetActorLocation();
	// 그리고 그 위치에서 효과를 재생한 뒤 액터를 소멸시킨다.
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, 
		spawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);

}


void ANetworkTutorialProjectile::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 만약 타당한 액터에 맞았다면
	// 점 데미지를 적용한다.
	if (OtherActor)
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, NormalImpulse, Hit,
			GetInstigatorController(), this, DamageType);
	}

	Destroy();
}

