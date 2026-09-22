#pragma once
#include <memory>
#include "romBrowser/SdFolder.h"
#include "themes/ThemeInfo.h"
#include "themes/ThemeInfoFactory.h"

class ThemeRepository
{
public:
    void Initialize();

    u32 GetThemeCount() const;
    std::unique_ptr<ThemeInfo> LoadThemeInfo(u32 themeIndex) const;

    /// @brief Finds the list position of a theme by its folder name (ASCII case-insensitive).
    /// @return The index, or -1 when no listed theme folder has that name.
    int FindThemeIndex(const TCHAR* folderName) const;

private:
    ThemeInfoFactory _themeInfoFactory;
    std::unique_ptr<SdFolder> _themesFolder;
    u32 _numberOfThemes = 0;
    std::unique_ptr<const FileInfo*[]> _themeFolders;
};
