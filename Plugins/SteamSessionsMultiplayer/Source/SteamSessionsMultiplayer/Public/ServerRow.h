// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "OnlineSessionSettings.h"

#include "ServerRow.generated.h"

/**
 * 
 */
UCLASS()
class STEAMSESSIONSMULTIPLAYER_API UServerRow : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;

	UFUNCTION()
		void JoinBClicked();

private:
	UPROPERTY(meta = (BindWidget))
		class UButton* JoinB;

public:
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ServerName;

	FOnlineSessionSearchResult Result;

	class UMenu* Menu;
};
