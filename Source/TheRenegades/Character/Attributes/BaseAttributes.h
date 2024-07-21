// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BaseAttributes.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * The bare minimum attributes any character/enemy should have
 */
UCLASS()
class THERENEGADES_API UBaseAttributes : public UAttributeSet
{
	GENERATED_BODY()

public:

	// Resources
	
	/*
	* Character Health
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Resources", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBaseAttributes, Health);
	/*
	* Character Max Health
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Resources", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth{200.f};
	ATTRIBUTE_ACCESSORS(UBaseAttributes, MaxHealth);
	/*
	* The resource used to cast abilities
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Resources", ReplicatedUsing = OnRep_Tension)
	FGameplayAttributeData Tension{ 0.f };
	ATTRIBUTE_ACCESSORS(UBaseAttributes, Tension);

	// Class Attributes

	/*
	* Adds to base weapon attack power
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Class Attributes", ReplicatedUsing = OnRep_Attack)
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(UBaseAttributes, Attack);
	/*
	* Reduces incoming base damage before applying other damage modifiers
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Class Attributes", ReplicatedUsing = OnRep_Defense)
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UBaseAttributes, Defense);
	/*
	* Ground and air speed max speed
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Class Attributes", ReplicatedUsing = OnRep_Speed)
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(UBaseAttributes, Speed);
	/*
	* Controls jump velocity, ground and air control/acceleration
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Class Attributes", ReplicatedUsing = OnRep_Agility)
	FGameplayAttributeData Agility;
	ATTRIBUTE_ACCESSORS(UBaseAttributes, Agility);
	/*
	* Controls rate at which tension is lost
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Class Attributes", ReplicatedUsing = OnRep_Focus)
	FGameplayAttributeData Focus;
	ATTRIBUTE_ACCESSORS(UBaseAttributes, Focus);
	/*
	* Controls Health passive regen speed, hard damage decay, and chances of surviving mortal blow
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Class Attributes", ReplicatedUsing = OnRep_Guts)
	FGameplayAttributeData Guts;
	ATTRIBUTE_ACCESSORS(UBaseAttributes, Guts);

public: // OnRep_ Functions

	UFUNCTION()
	virtual void OnRep_Health (const FGameplayAttributeData& OldHealth);
	UFUNCTION()
	virtual void OnRep_MaxHealth (const FGameplayAttributeData& OldMaxhealth);
	UFUNCTION()
	virtual void OnRep_Tension (const FGameplayAttributeData& OldTension);
	UFUNCTION()
	virtual void OnRep_Attack(const FGameplayAttributeData& OldAttack);
	UFUNCTION()
	virtual void OnRep_Defense(const FGameplayAttributeData& OldDefense);
	UFUNCTION()
	virtual void OnRep_Speed(const FGameplayAttributeData& OldSpeed);
	UFUNCTION()
	virtual void OnRep_Agility(const FGameplayAttributeData& OldAgility);
	UFUNCTION()
	virtual void OnRep_Focus(const FGameplayAttributeData& OldFocus);
	UFUNCTION()
	virtual void OnRep_Guts(const FGameplayAttributeData& OldGuts);

public: // Magic numbers

	const float MaxTension = 2000.f;

public:

	UBaseAttributes();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
