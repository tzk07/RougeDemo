// Copyright Epic Games, Inc. All Rights Reserved.

#include "RougeCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h" // 对应 APlayerController
#include "Engine/LocalPlayer.h"             // 对应 ULocalPlayer
#include "InputAction.h"                    // 对应 UInputAction
#include "InputMappingContext.h"            // 对应 UInputMappingContext
#include "HealthComponent.h"
#include "AutoAttackComponent.h"
#include "ExperienceComponent.h"
#include "WeaponBase.h"


// 构造函数
ARougeCharacter::ARougeCharacter()
{
	// 设为true表示每帧都会调用Tick函数。如果不怎么用Tick，设为false可以节省性能。
	PrimaryActorTick.bCanEverTick = true;

	// 创建并设置弹簧臂组件
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent); // 绑定到角色的根组件(胶囊体)上
	SpringArmComp->TargetArmLength = 1000.0f; // 俯视角拉远一点，设定自拍杆长度为10米
	SpringArmComp->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f)); // 摄像机向下看60度
	SpringArmComp->bDoCollisionTest = false; // 俯视角一般不需要自拍杆防碰撞检测

	// 创建并设置摄像机组件
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName); // 挂载到自拍杆末端
	CameraComp->bUsePawnControlRotation = false; // 摄像机不随控制器旋转，保持固定的俯视角

	SpringArmComp->bInheritPitch = false; // 不继承俯仰角
	SpringArmComp->bInheritYaw = false;   // 不继承偏航角
	SpringArmComp->bInheritRoll = false;  // 不继承翻滚角

	// 创建并设置生命值组件
	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
	// 创建武器组件
	// AutoAttackComp = CreateDefaultSubobject<UAutoAttackComponent>(TEXT("AutoAttackComp"));
	// 创建经验条组件
	ExpComp = CreateDefaultSubobject<UExperienceComponent>(TEXT("ExpComp"));

	// 角色移动设置优化
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 让角色自动朝向移动的方向
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // 转身速度
}

void ARougeCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 初始化增强输入系统：将配置好的操作映射(IMC)添加到玩家控制器中
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// 优先级设为0，加载默认按键映射
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

// 绑定输入函数
void ARougeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 将默认的输入组件转换为增强输入组件
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 绑定“移动”动作：当玩家持续按下按键(Triggered)时，调用 Move 函数
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARougeCharacter::Move);
		}
	}
}

// 执行移动逻辑
void ARougeCharacter::Move(const FInputActionValue& Value)
{
	// 从输入值中提取二维向量 (X和Y)
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 以世界绝对方向来移动 (适用于俯视角游戏)
		// X代表向前(W)和向后(S)，Y代表向右(D)和向左(A)
		const FVector ForwardDirection = FVector(1.0f, 0.0f, 0.0f);
		const FVector RightDirection = FVector(0.0f, 1.0f, 0.0f);

		// 添加移动输入
		AddMovementInput(ForwardDirection, MovementVector.Y); // W/S 控制前后
		AddMovementInput(RightDirection, MovementVector.X);   // A/D 控制左右
	}
}

void ARougeCharacter::AddWeapon(TSubclassOf<AWeaponBase> WeaponClass)
{
	// 安全检查：如果传入的类是空的，直接返回
	if (!WeaponClass) return;

	// 设置生成参数
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this; // 极其重要：明确告诉引擎，这把武器的主人是玩家！
	SpawnParams.Instigator = GetInstigator(); //表示 “行为责任方”，侧重行为溯源

	// 在世界上生成这把武器
	AWeaponBase* NewWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, GetActorLocation(), GetActorRotation(), SpawnParams);

	if (NewWeapon)
	{
		// 将生成的武器“吸附”到玩家的根组件上
		// SnapToTargetNotIncludingScale 意味着位置和旋转对齐玩家，但不改变武器原本的缩放
		NewWeapon->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		// 加入数组统一管理（未来可以用来做武器升级、丢弃武器等功能）
		EquippedWeapons.Add(NewWeapon);

		UE_LOG(LogTemp, Warning, TEXT("Successfully equipped new weapon!"));
	}
}

int32 ARougeCharacter::GetWeaponCount() const
{
	return EquippedWeapons.Num();
}

void ARougeCharacter::UpgradeAllWeapons(float DamageBonus, float CooldownReduction, float RangeBonus)
{
	// 遍历玩家身上的所有武器，挨个进行强化
	for (AWeaponBase* Weapon : EquippedWeapons)
	{
		if (Weapon)
		{
			Weapon->UpgradeStats(DamageBonus, CooldownReduction, RangeBonus);
		}
	}
}