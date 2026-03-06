// Fill out your copyright notice in the Description page of Project Settings.


#include "ExperienceComponent.h"
#include "RougeCharacter.h"
#include "WeaponBase.h"
#include "RougeTypes.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UExperienceComponent::UExperienceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// 初始化数值
	CurrentLevel = 1;
	CurrentExp = 0.0f;
	MaxExp = 100.0f; // 第一级需要100点经验
	// ...
}


// Called when the game starts
void UExperienceComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UExperienceComponent::AddExperience(float ExpAmount)
{
	if (ExpAmount <= 0) return;

	CurrentExp += ExpAmount;

	bool bDidLevelUp = false;

	while (CurrentExp >= MaxExp)
	{
		CurrentExp -= MaxExp;
		CurrentLevel++;
		MaxExp *= 1.2f;
		PendingUpgrades++; // 记录待升级次数
		bDidLevelUp = true;

		OnLevelUp.Broadcast(CurrentLevel);
	}

	OnExpChanged.Broadcast(CurrentExp, MaxExp, CurrentLevel);

	// 如果升级了，且当前没有正在显示的升级UI（假设 PendingUpgrades == 1 代表刚触发第一轮）
	// 开始生成选项
	if (bDidLevelUp && PendingUpgrades > 0)
	{
		GenerateUpgrades();
	}
}

void UExperienceComponent::GenerateUpgrades()
{
	TArray<FRougeUpgradeOption*> ValidOptions;

	// 每 5 级尝试从武器表中抽
	if (CurrentLevel % 5 == 0 && WeaponDropsTable)
	{
		TArray<FRougeUpgradeOption*> AllWeapons;
		WeaponDropsTable->GetAllRows<FRougeUpgradeOption>(TEXT("UpgradeContext"), AllWeapons);

		// 过滤掉玩家已经拥有的武器
		for (FRougeUpgradeOption* WeaponOption : AllWeapons)
		{
			// 检查是否有效，且 AcquiredWeapons 数组中不包含该武器类
			if (WeaponOption && WeaponOption->WeaponClassToSpawn != nullptr)
			{
				if (!AcquiredWeapons.Contains(WeaponOption->WeaponClassToSpawn))
				{
					ValidOptions.Add(WeaponOption);
				}
			}
		}

		// 如果所有武器都拿到了（过滤后池子空了），则无缝回退到属性强化表
		if (ValidOptions.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("All weapons acquired! Falling back to stat upgrades."));
			if (StatUpgradesTable)
			{
				StatUpgradesTable->GetAllRows<FRougeUpgradeOption>(TEXT("UpgradeContext"), ValidOptions);
			}
		}
	}
	else if (StatUpgradesTable)
	{
		// 正常等级，先读取所有的属性表选项
		TArray<FRougeUpgradeOption*> AllStats;
		StatUpgradesTable->GetAllRows<FRougeUpgradeOption>(TEXT("UpgradeContext"), AllStats);

		// 严格过滤：只保留“数值强化”类型，排除掉任何可能混入的武器
		for (FRougeUpgradeOption* StatOption : AllStats)
		{
			if (StatOption && StatOption->Type == EUpgradeType::StatBonus)
			{
				ValidOptions.Add(StatOption);
			}
		}
	}

	if (ValidOptions.Num() == 0) return;

	// 打乱数组来实现随机 (Fisher-Yates Shuffle)
	const int32 NumShuffles = ValidOptions.Num() - 1;
	for (int32 i = 0; i < NumShuffles; ++i)
	{
		int32 SwapIndex = FMath::RandRange(i, NumShuffles);
		ValidOptions.Swap(i, SwapIndex);
	}

	// 取前 3 个（如果表里不足 3 个，则有几个取几个，防止越界）
	TArray<FRougeUpgradeOption> SelectedOptions;
	int32 OptionsCount = FMath::Min(3, ValidOptions.Num());
	for (int32 i = 0; i < OptionsCount; ++i)
	{
		SelectedOptions.Add(*ValidOptions[i]);
	}

	// 广播给 UI 去生成三选一卡片
	OnOptionsGenerated.Broadcast(SelectedOptions);
}

void UExperienceComponent::ApplyUpgrade(const FRougeUpgradeOption& SelectedOption)
{
	ARougeCharacter* MyCharacter = Cast<ARougeCharacter>(GetOwner());
	if (!MyCharacter) return;

	if (SelectedOption.Type == EUpgradeType::StatBonus)
	{
		TArray<AActor*> AttachedActors;
		MyCharacter->GetAttachedActors(AttachedActors);

		for (AActor* Actor : AttachedActors)
		{
			AWeaponBase* Weapon = Cast<AWeaponBase>(Actor);
			if (Weapon)
			{
				Weapon->UpgradeStats(SelectedOption.DamageBonus, SelectedOption.CooldownReduction, SelectedOption.RangeBonus);
			}
		}
	}
	else if (SelectedOption.Type == EUpgradeType::NewWeapon && SelectedOption.WeaponClassToSpawn)
	{
		// 生成新武器
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = MyCharacter;
		AWeaponBase* NewWeapon = GetWorld()->SpawnActor<AWeaponBase>(SelectedOption.WeaponClassToSpawn, MyCharacter->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);

		if (NewWeapon)
		{
			NewWeapon->AttachToComponent(MyCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponSocket"));

			// 将这个武器类记录到已获取数组中，下次就不会再刷出来了
			AcquiredWeapons.AddUnique(SelectedOption.WeaponClassToSpawn);
		}
	}

	PendingUpgrades--;

	if (PendingUpgrades > 0)
	{
		GenerateUpgrades();
	}
}

void UExperienceComponent::TriggerStartingWeaponSelection()
{
	if (!WeaponDropsTable) return;

	TArray<FRougeUpgradeOption*> AllWeapons;
	WeaponDropsTable->GetAllRows<FRougeUpgradeOption>(TEXT("UpgradeContext"), AllWeapons);

	TArray<FRougeUpgradeOption*> ValidOptions;
	for (FRougeUpgradeOption* WeaponOption : AllWeapons)
	{
		if (WeaponOption && WeaponOption->WeaponClassToSpawn != nullptr)
		{
			// 即使是开局也走一遍查重逻辑以防万一
			if (!AcquiredWeapons.Contains(WeaponOption->WeaponClassToSpawn))
			{
				ValidOptions.Add(WeaponOption);
			}
		}
	}

	if (ValidOptions.Num() == 0) return;

	// 洗牌算法
	const int32 NumShuffles = ValidOptions.Num() - 1;
	for (int32 i = 0; i < NumShuffles; ++i)
	{
		int32 SwapIndex = FMath::RandRange(i, NumShuffles);
		ValidOptions.Swap(i, SwapIndex);
	}

	TArray<FRougeUpgradeOption> SelectedOptions;
	int32 OptionsCount = FMath::Min(3, ValidOptions.Num());
	for (int32 i = 0; i < OptionsCount; ++i)
	{
		SelectedOptions.Add(*ValidOptions[i]);
	}

	// 关键：给 PendingUpgrades 加 1，这样 UI 选完调用 ApplyUpgrade 时扣减后才不会出错
	PendingUpgrades++;

	// 广播，通知 UI 弹出
	OnOptionsGenerated.Broadcast(SelectedOptions);
}