// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "UObject/ConstructorHelpers.h"
#include "ServerRow.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"

UMenu::UMenu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/SteamSessionsMultiplayer/WBP_ServerRow"));
	if (ServerRowBPClass.Class == nullptr) return;
	ServerRowClass = ServerRowBPClass.Class;
}

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	
	if (HostMenuButton)
	{
		HostMenuButton->OnClicked.AddDynamic(this, &ThisClass::HostMenuButtonClicked);
	}
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	if (HostToMainMenuButton)
	{
		HostToMainMenuButton->OnClicked.AddDynamic(this, &ThisClass::HostToMainMenuButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}
	if (JoinToMainMenuButton)
	{
		JoinToMainMenuButton->OnClicked.AddDynamic(this, &ThisClass::JoinToMainMenuButtonClicked);
	}
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &ThisClass::QuitButtonClicked);
	}

	if (SearchStatus == nullptr)
	{
		SearchStatus = (UTextBlock*)(WidgetTree->FindWidget(FName()));
	}
		
	return true;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(PathToLobby);
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Failed to create session!"))
			);
		}
		HostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (MultiplayerSessionsSubsystem == nullptr)
	{
		return;
	}

	ServerList->ClearChildren();

	for (auto Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (SettingsValue == MatchType)
		{
			SetServerList(Result);	// For the player to select the server he wishes to join
		}
	}
	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		if (SearchStatus)
		{
			SearchStatus->SetText(FText::FromString("No active sessions found!"));
		}
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::SetServerList(const FOnlineSessionSearchResult& Result)
{
	ServerRow = CreateWidget<UServerRow>(GetWorld(), ServerRowClass);
	if (ServerRow == nullptr) return;
	ServerRow->Menu = this;
	FString ServerName;
	if (Result.Session.SessionSettings.Get(FName("ServerName"), ServerName))
	{
		ServerRow->ServerName->SetText(FText::FromString(ServerName));
	}
	else
	{
		ServerRow->ServerName->SetText(FText::FromString(Result.Session.OwningUserName));
	}
	ServerRow->Result = Result;
	if (SearchStatus)
	{
		SearchStatus->SetText(FText::FromString(""));
	}	
	ServerList->AddChild(ServerRow);
}

void UMenu::ServerJoinButtonClciked(const FOnlineSessionSearchResult& Result)
{
	if (SearchStatus)
	{
		SearchStatus->SetText(FText::FromString("Joining session please wait"));
	}
	MultiplayerSessionsSubsystem->JoinSession(Result);
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::HostMenuButtonClicked()
{
	MenuWidgetSwitcher->SetActiveWidget(HostMenu);
}

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		// TODO: Add the server name prama to pass on
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType, ServerNameBox->GetText().ToString());
	}
}

void UMenu::HostToMainMenuButtonClicked()
{
	MenuWidgetSwitcher->SetActiveWidget(MainMenu);
}

void UMenu::JoinButtonClicked()
{
	MenuWidgetSwitcher->SetActiveWidget(JoinMenu);
	if (SearchStatus)
	{
		SearchStatus->SetText(FText::FromString("Searching for available sessions..."));
	}
	// JoinButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void UMenu::JoinToMainMenuButtonClicked()
{
	MenuWidgetSwitcher->SetActiveWidget(MainMenu);
}

void UMenu::QuitButtonClicked()
{
	GetWorld()->GetFirstPlayerController()->ConsoleCommand("Quit");
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}
