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

	// �� ���ʹ� replicate�� �����մϴ�.
	bReplicates = true;

	// ������Ʈ�� ���� �ʱ�ȭ�� �����մϴ�.
	// ���� sphere ������Ʈ�Դϴ�.
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	SphereComponent->InitSphereRadius(37.5f);
	SphereComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = SphereComponent;

	// ���� ��ǻ�Ͷ��
	// ���� ������ Role == ROLE_Authority �̴�.(Docs)
	if (GetLocalRole() == ROLE_Authority)
	{
		SphereComponent->OnComponentHit.AddDynamic(this, &ANetworkTutorialProjectile::OnProjectileImpact);
	}

	// ���� static �޽ø� ���Դϴ�.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMesh->SetupAttachment(RootComponent);

	// ��ġ�� �������� �����մϴ�.
	if (DefaultMesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(DefaultMesh.Object);
		StaticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -37.5f));
		StaticMesh->SetRelativeScale3D(FVector(0.75f, 0.75f, 0.75f));
	}

	// �׸��� ��ƼŬ �ý����� ����ϴ�.
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DefaultExplosionEffect.Succeeded())
	{
		ExplosionEffect = DefaultExplosionEffect.Object;
	}

	// ����ü �����ӿ� ���� ������Ʈ�� �����ϰ� �̸� �����մϴ�.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);
	ProjectileMovementComponent->InitialSpeed = 1500.0f;
	ProjectileMovementComponent->MaxSpeed = 1500.0f;	
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	// ���������� ������ Ÿ�Կ� ���� �����մϴ�.
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
	// ���� ��ġ�� �޴´�.
	FVector spawnLocation = GetActorLocation();
	// �׸��� �� ��ġ���� ȿ���� ����� �� ���͸� �Ҹ��Ų��.
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, 
		spawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);

}


void ANetworkTutorialProjectile::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// ���� Ÿ���� ���Ϳ� �¾Ҵٸ�
	// �� �������� �����Ѵ�.
	if (OtherActor)
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, NormalImpulse, Hit,
			GetInstigatorController(), this, DamageType);
	}

	Destroy();
}

