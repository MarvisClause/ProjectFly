#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ProjectFlyGameInstance.generated.h"

UCLASS()
class PROJECTFLY_API UProjectFlyGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Levels")
    TSoftObjectPtr<UWorld> MainMenuLevel;

    // Level list
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Levels")
    TArray<TSoftObjectPtr<UWorld>> LevelList;

    UPROPERTY(BlueprintReadWrite, Category = "Progression")
    int32 CurrentLevelIndex;

    UPROPERTY(BlueprintReadOnly, Category = "Progression")
    int32 HighestUnlockedLevel;

    // Functionallity for save and loading game progress
    UFUNCTION(BlueprintCallable, Category = "Progression")
    void SaveGameProgress();

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void LoadGameProgress();

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void CompleteCurrentLevel();

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void StartNewGame();

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void LoadCurrentLevel();

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void LoadNextLevel();

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void RestartLevel();

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void LoadMainMenuLevel();

    UFUNCTION(BlueprintCallable, Category = "Progression")
    bool IsCurrentLevelLast() const;

    virtual void Init() override;
};