// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Character/PBPlayerCharacter.h"
#include "../Character Movement/CustomPBPlayerMovement.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "BasePlayerState.h"

#include "PBBaseCharacter.generated.h"

class UCustomPBPlayerMovement;
class UAbilitySystemComponent;
struct FDamageEvent;


/**
 * 
 */
UCLASS(Config = Game)
class THERENEGADES_API APBBaseCharacter : public APBPlayerCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> Abilities;
	

public:

	UFUNCTION(BlueprintImplementableEvent)
	void PlayerStateRep();

protected:

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		auto PS = GetPlayerState<ABasePlayerState>();
		if (PS) {
			return PS->AbilitySystemComponent;
		}

		return nullptr;
	}
	;

	APBBaseCharacter(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	UCustomPBPlayerMovement* GetCustomCharacterMovement() const;

	FCollisionQueryParams GetIgnoreCharacterParams() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;
};
