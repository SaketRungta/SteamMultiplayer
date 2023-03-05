// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerRow.h"
#include "Components/Button.h"
#include "Menu.h"

bool UServerRow::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (JoinB)
	{
		JoinB->OnClicked.AddDynamic(this, &ThisClass::JoinBClicked);
	}

	return true;
}

void UServerRow::JoinBClicked()
{
	Menu->ServerJoinButtonClciked(Result);
}
