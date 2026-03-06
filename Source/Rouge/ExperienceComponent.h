// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponBase.h"
#include "ExperienceComponent.generated.h"

// 声明经验值改变和升级的委托，方便蓝图UI更新
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnExpChanged, float, CurrentExp, float, MaxExp, int32, CurrentLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int32, CurrentLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpgradeOptionsGenerated, const TArray<FRougeUpgradeOption>&, Options);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROUGE_API UExperienceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UExperienceComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// 当前等级
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RougeExperience")
	int32 CurrentLevel;

	// 当前经验值
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RougeExperience")
	float CurrentExp;

	// 升到下一级所需的经验值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RougeExperience")
	float MaxExp;

	// 填入引擎中创建好的数据表
	UPROPERTY(EditDefaultsOnly, Category = "RougeExperience")
	UDataTable* StatUpgradesTable;

	UPROPERTY(EditDefaultsOnly, Category = "RougeExperience")
	UDataTable* WeaponDropsTable;

public:	
	// 增加经验的函数
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void AddExperience(float ExpAmount);

	// 委托实例
	UPROPERTY(BlueprintAssignable, Category = "Experience|Events")
	FOnExpChanged OnExpChanged;

	UPROPERTY(BlueprintAssignable, Category = "Experience|Events")
	FOnLevelUp OnLevelUp;

	UPROPERTY(BlueprintAssignable, Category = "Experience|Events")
	FOnUpgradeOptionsGenerated OnOptionsGenerated;

	// 蓝图 UI 玩家点击选项后调用此函数
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void ApplyUpgrade(const FRougeUpgradeOption& SelectedOption);

	// 在游戏开始时调用，强制触发一次武器三选一
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void TriggerStartingWeaponSelection();

private:
	// 记录玩家累计升了多少级但还没选强化的次数
	int32 PendingUpgrades = 0;

	// 记录玩家已经拥有的武器类，防止重复抽取
	UPROPERTY()
	TArray<TSubclassOf<AWeaponBase>> AcquiredWeapons;

	// 核心抽卡逻辑
	void GenerateUpgrades();
};
