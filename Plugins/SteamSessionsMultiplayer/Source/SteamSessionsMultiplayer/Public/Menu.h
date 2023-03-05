// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class STEAMSESSIONSMULTIPLAYER_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UMenu(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
		void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/ThirdPersonCPP/Maps/Lobby")));

	void ServerJoinButtonClciked(const FOnlineSessionSearchResult& Result);

protected:

	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	//
	// Callbacks for the custom delegates on the MultiplayerSessionsSubsystem
	//
	UFUNCTION()
		void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
		void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
		void OnStartSession(bool bWasSuccessful);

private:
	UPROPERTY(meta = (BindWidget))
		class UButton* HostMenuButton;

	UPROPERTY(meta = (BindWidget))
		UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
		UButton* HostToMainMenuButton;

	UPROPERTY(meta = (BindWidget))
		UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
		UButton* JoinToMainMenuButton;

	UPROPERTY(meta = (BindWidget))
		UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* MenuWidgetSwitcher;

	UPROPERTY(meta = (BindWidget))
		UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
		UWidget* HostMenu;

	UPROPERTY(meta = (BindWidget))
		UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
		class UPanelWidget* ServerList;

	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* ServerNameBox;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* SearchStatus;

	TSubclassOf<UUserWidget> ServerRowClass;

	class UServerRow* ServerRow;

	UFUNCTION()
		void HostMenuButtonClicked();

	UFUNCTION()
		void HostButtonClicked();

	UFUNCTION()
		void HostToMainMenuButtonClicked();

	UFUNCTION()
		void JoinButtonClicked();

	UFUNCTION()
		void JoinToMainMenuButtonClicked();

	UFUNCTION()
		void QuitButtonClicked();	

	void MenuTearDown();

	void SetServerList(const FOnlineSessionSearchResult& Result);

	// The subsystem designed to handle all online session functionality
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	int32 NumPublicConnections{4};
	FString MatchType{TEXT("FreeForAll")};
	FString PathToLobby{TEXT("")};
};
