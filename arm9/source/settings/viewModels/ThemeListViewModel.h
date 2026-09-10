#pragma once

class ISettingsController;

class ThemeListViewModel
{
public:
    /// @param activeThemeFolderName Folder name of the theme in use; the list opens on it when listed.
    ThemeListViewModel(ISettingsController* settingsController, const char* activeThemeFolderName);

    void NavigateUp() const;

    ISettingsController* GetSettingsController() const
    {
        return _settingsController;
    }

    constexpr int GetSelectedItem() const { return _selectedItem; }
    void SetSelectedItem(int selectedItem) { _selectedItem = selectedItem; }

private:
    ISettingsController* _settingsController;
    int _selectedItem = -1;
};
