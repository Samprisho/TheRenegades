// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAttributes.h"


#include "Net/UnrealNetwork.h"

UBaseAttributes::UBaseAttributes()
{
	InitMaxHealth(200.f);
	InitHealth(GetMaxHealth());
	InitAttack(50);
	InitDefense(50);
	InitSpeed(50);
	InitAgility(50);
	InitFocus(50);
	InitGuts(50);
}

void UBaseAttributes::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributes, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributes, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributes, Tension, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributes, Attack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributes, Defense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributes, Speed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributes, Agility, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributes, Focus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributes, Guts, COND_None, REPNOTIFY_Always);
}

void UBaseAttributes::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributes, Health, OldHealth);
}

void UBaseAttributes::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxhealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributes, MaxHealth, OldMaxhealth);
}

void UBaseAttributes::OnRep_Tension(const FGameplayAttributeData& OldTension)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributes, Tension, OldTension)
}

void UBaseAttributes::OnRep_Attack(const FGameplayAttributeData& OldAttack)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributes, Attack, OldAttack)
}

void UBaseAttributes::OnRep_Defense(const FGameplayAttributeData& OldDefense)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributes, Defense, OldDefense)
}

void UBaseAttributes::OnRep_Speed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributes, Speed, OldSpeed)
}

void UBaseAttributes::OnRep_Agility(const FGameplayAttributeData& OldAgility)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributes, Agility, OldAgility)
}

void UBaseAttributes::OnRep_Focus(const FGameplayAttributeData& OldFocus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributes, Focus, OldFocus)
}

void UBaseAttributes::OnRep_Guts(const FGameplayAttributeData& OldGuts)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributes, Guts, OldGuts)
}
