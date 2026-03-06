// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

class ARougeCharacter;
class UExperienceComponent;

#include "ExperienceGem.generated.h"

UCLASS()
class ROUGE_API AExperienceGem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExperienceGem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 碰撞球：玩家碰到它就被拾取
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RougeComponents")
	USphereComponent* PickupCollision;

	// 经验球的网格体模型
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RougeComponents")
	UStaticMeshComponent* MeshComp;

	// 这个经验球包含多少经验
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RougeExperience")
	float ExpValue;

	// 碰到玩家时的重叠事件
	UFUNCTION()
	void OnGemOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
