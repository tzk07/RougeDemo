// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h" // 引入增强输入系统的值类型
#include "RougeCharacter.generated.h"

// 前向声明，告诉编译器有这些类，减少头文件依赖，加快编译速度
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class ROUGE_API ARougeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// 构造函数：初始化组件默认值
	ARougeCharacter();

protected:
	// 游戏开始时调用
	virtual void BeginPlay() override;

	// ====== 核心组件 ====== 

	// 弹簧臂组件：像一根自拍杆，用于连接角色和摄像机，处理防穿模
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	// 摄像机组件：玩家的“眼睛”
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	// 生命值组件: 血条
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RougeComponents")
	class UHealthComponent* HealthComp;

	// 武器组件
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RougeComponents")
	// class UAutoAttackComponent* AutoAttackComp;

	// 经验条组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RougeComponents")
	class UExperienceComponent* ExpComp;

	// 存储玩家当前拥有的所有武器
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	TArray<AWeaponBase*> EquippedWeapons;

	// ====== 输入配置 ====== 

	// 映射上下文 (IMC)：定义了按键的整体规则集
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RougeInput")
	UInputMappingContext* DefaultMappingContext;

	// 移动输入动作 (IA)：代表“移动”这个动作本身
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RougeInput")
	UInputAction* MoveAction;

	// ====== 输入处理函数 ====== 

	// 处理移动逻辑的函数。FInputActionValue 包含了玩家按下按键时传递的值（比如摇杆的XY轴）
	void Move(const FInputActionValue& Value);

public:
	// 绑定玩家输入到控制器的函数
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 动态添加武器的函数，开放给蓝图调用
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void AddWeapon(TSubclassOf<AWeaponBase> WeaponClass);

	// 获取当前武器数量
	UFUNCTION(BlueprintPure, Category = "Weapons")
	int32 GetWeaponCount() const;

	// 一键强化所有已装备的武器
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void UpgradeAllWeapons(float DamageBonus, float CooldownReduction, float RangeBonus);
};
