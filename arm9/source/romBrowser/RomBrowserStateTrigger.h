#pragma once

enum class RomBrowserStateTrigger
{
    None,
    Navigate,
    ChangeDisplayMode,
    ShowGameInfo,
    HideGameInfo,
    FolderLoadDone,
    Launch,
    ShowDisplaySettings,
    HideDisplaySettings,
    GotoSettingsScreen,
    ShowRecents,
    HideRecents,
    ShowFavorites,
    HideFavorites,
    ShowStatistics,
    HideStatistics,
    ShowDeleteConfirm,
    HideDeleteConfirm,
    ShowMenu,
    HideMenu
};
