#include "common.h"
#include "settings/ISettingsController.h"
#include "themes/ThemeRepository.h"
#include "ThemeListViewModel.h"

ThemeListViewModel::ThemeListViewModel(ISettingsController* settingsController, const char* activeThemeFolderName)
    : _settingsController(settingsController)
    , _selectedItem(settingsController->GetThemeRepository().FindThemeIndex(activeThemeFolderName)) { }

void ThemeListViewModel::NavigateUp() const
{
    _settingsController->NavigateUp();
}
