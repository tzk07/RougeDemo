// Fill out your copyright notice in the Description page of Project Settings.

#include "ExperienceGem.h"
#include "RougeCharacter.h"       // ARougeCharacter 的头文件
#include "ExperienceComponent.h"  // UExperienceComponent 的头文件

// Sets default values
AExperienceGem::AExperienceGem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 1设置碰撞球为根组件
	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	SetRootComponent(PickupCollision);
	PickupCollision->SetSphereRadius(30.0f); // 拾取范围
	// 设置碰撞预设：只检测重叠，不阻挡物理
	PickupCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// 设置模型组件
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	// 关掉模型的物理碰撞，防止它挡住玩家或怪物
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ExpValue = 20.0f; // 默认每个球提供20点经验
}

// Called when the game starts or when spawned
void AExperienceGem::BeginPlay()
{
	Super::BeginPlay();
	// 绑定拾取事件
	PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &AExperienceGem::OnGemOverlap);
}

void AExperienceGem::OnGemOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 判断碰到它的是不是玩家
	if (ARougeCharacter* PlayerCharacter = Cast<ARougeCharacter>(OtherActor))
	{
		// 尝试从玩家身上获取经验组件
		if (UExperienceComponent* ExpComp = PlayerCharacter->FindComponentByClass<UExperienceComponent>())
		{
			// 给玩家增加经验
			ExpComp->AddExperience(ExpValue);

			// 播放一个吃金币的音效 (可选，后续可以在蓝图里加)

			// 销毁经验球自己
			Destroy();
		}
	}
}

