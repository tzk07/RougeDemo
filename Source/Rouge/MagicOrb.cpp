// Fill out your copyright notice in the Description page of Project Settings.


#include "MagicOrb.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "EnemyBase.h"
#include "HealthComponent.h"

// Sets default values
AMagicOrb::AMagicOrb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 设置碰撞球
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	SetRootComponent(CollisionComp);
	CollisionComp->SetSphereRadius(15.0f);
	CollisionComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// 设置模型
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 碰撞不需要模型处理

	// 设置抛体运动
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 800.f; // 初始飞行速度
	ProjectileMovement->MaxSpeed = 800.f;     // 最大飞行速度
	ProjectileMovement->ProjectileGravityScale = 0.0f; // 设为0代表不受重力影响，沿直线飞行

	OrbDamage = 10.0f;
	MaxRange = 800.0f;
}

// Called when the game starts or when spawned
void AMagicOrb::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation(); // 存下出生点

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AMagicOrb::OnOrbOverlap);
}

void AMagicOrb::OnOrbOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 判断碰到的是不是怪物
	if (AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor))
	{
		if (UHealthComponent* EnemyHealth = Enemy->FindComponentByClass<UHealthComponent>())
		{
			// 对怪物造成伤害
			EnemyHealth->TakeDamage(OrbDamage);

			// 击中第一个怪物后，法球销毁自己（不穿透）
			Destroy();
		}
	}
}

// 实现初始化函数
void AMagicOrb::InitializeOrb(float InDamage, float InRange)
{
	OrbDamage = InDamage;

	// 核心优化：时间 = 距离 / 速度
	if (ProjectileMovement && ProjectileMovement->InitialSpeed > 0.0f)
	{
		float LifeTime = InRange / ProjectileMovement->InitialSpeed;

		// SetLifeSpan 是虚幻自带的高效销毁函数，时间一到自动销毁 Actor
		SetLifeSpan(LifeTime);
	}
}
