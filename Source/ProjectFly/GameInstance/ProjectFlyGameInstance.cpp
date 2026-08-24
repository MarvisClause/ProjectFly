#include "ProjectFlyGameInstance.h"
#include "ProjectFly/SaveGame/ProjectFlySaveGame.h"
#include "Kismet/GameplayStatics.h"

static constexpr TCHAR SaveSlotName[] = TEXT("ProjectFly");

void UProjectFlyGameInstance::Init()
{
    Super::Init();
    LoadGameProgress();
}

void UProjectFlyGameInstance::LoadGameProgress()
{
    if (UGameplayStatics::DoesSaveGameExist(TEXT("ProjectFly"), 0))
    {
        UProjectFlySaveGame* LoadInstance = Cast<UProjectFlySaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("ProjectFly"), 0));
        if (LoadInstance)
        {
            HighestUnlockedLevel = LoadInstance->HighestUnlockedLevel;
            CurrentLevelIndex = LoadInstance->CurrentLevelIndex;
            return;
        }
    }

    // In case of file absence, set first level as a highest one
    HighestUnlockedLevel = 0;
}

void UProjectFlyGameInstance::SaveGameProgress()
{
    UProjectFlySaveGame* SaveInstance = Cast<UProjectFlySaveGame>(UGameplayStatics::CreateSaveGameObject(UProjectFlySaveGame::StaticClass()));
    if (SaveInstance)
    {
        SaveInstance->HighestUnlockedLevel = HighestUnlockedLevel;
        SaveInstance->CurrentLevelIndex = CurrentLevelIndex;
        UGameplayStatics::SaveGameToSlot(SaveInstance, TEXT("ProjectFly"), 0);
    }
}

void UProjectFlyGameInstance::CompleteCurrentLevel()
{
    // Progress highest level index, in case of completion of latest level
    if (CurrentLevelIndex == HighestUnlockedLevel)
    {
        HighestUnlockedLevel++;
    }

    SaveGameProgress();
}

void UProjectFlyGameInstance::StartNewGame()
{
    HighestUnlockedLevel = CurrentLevelIndex = 0;
    LoadCurrentLevel();
}

void UProjectFlyGameInstance::LoadCurrentLevel()
{
    if (LevelList.IsValidIndex(CurrentLevelIndex))
    {
        const FSoftObjectPath& LevelPath = LevelList[CurrentLevelIndex].ToSoftObjectPath();
        UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelPath.GetAssetName()));
    }
}

void UProjectFlyGameInstance::LoadNextLevel()
{
    // Load next level
    if (LevelList.IsValidIndex(CurrentLevelIndex + 1))
    {
        CompleteCurrentLevel();
        CurrentLevelIndex++;
        LoadCurrentLevel();
    }
    else
    {
        LoadMainMenuLevel();
    }
}

void UProjectFlyGameInstance::RestartLevel()
{
    if (LevelList.IsValidIndex(CurrentLevelIndex))
    {
        const FSoftObjectPath& LevelPath = LevelList[CurrentLevelIndex].ToSoftObjectPath();
        UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelPath.GetAssetName()));
    }
}

void UProjectFlyGameInstance::LoadMainMenuLevel()
{
    const FSoftObjectPath& LevelPath = MainMenuLevel.ToSoftObjectPath();
    UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelPath.GetAssetName()));
}

bool UProjectFlyGameInstance::IsCurrentLevelLast() const
{
    return CurrentLevelIndex == (LevelList.Num() - 1);
}
