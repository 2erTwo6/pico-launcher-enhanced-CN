#include "common.h"
#include <libtwl/dma/dmaNitro.h>
#include <libtwl/gfx/gfx.h>
#include <libtwl/gfx/gfxBackground.h>
#include "core/mini-printf.h"
#include "gui/GraphicsContext.h"
#include "settings/SettingsController.h"
#include "themes/IFontRepository.h"
#include "themes/material/MaterialColorScheme.h"
#include "Version.h"
#include "ThemeListTopView.h"

#define VERSION_WIDTH   120
#define VERSION_X       (256 - 6 - VERSION_WIDTH)
#define VERSION_Y       (192 - 4 - 16)

ThemeListTopView::ThemeListTopView(SharedPtr<ThemeListViewModel> viewModel, const MaterialColorScheme* materialColorScheme,
    const IFontRepository* fontRepository)
    : _viewModel(viewModel)
    , _noPreviewLabel(Label2DView::CreateShared(64, 16, 25, fontRepository->GetFont(FontType::Medium11)))
    , _versionLabel(Label2DView::CreateShared(VERSION_WIDTH, 16, 32, fontRepository->GetFont(FontType::Medium7_5)))
{
    _noPreviewLabel->SetHorizontalAlignment(Alignment::Center);
    _noPreviewLabel->SetText("No preview");
    _noPreviewLabel->SetPosition(128 - 32, 96 - 8);
    _noPreviewLabel->SetBackgroundColor(materialColorScheme->inverseOnSurface);
    _noPreviewLabel->SetForegroundColor(materialColorScheme->onSurfaceVariant);
    AddChildTail(_noPreviewLabel.GetPointer());

    // The one screen of the launcher that is about the launcher itself, so the
    // version and the commit it was built from live here, small, in the corner.
    char version[48];
    FormatLauncherVersion(version, sizeof(version), true);
    _versionLabel->SetHorizontalAlignment(Alignment::End);
    _versionLabel->SetText(version);
    _versionLabel->SetPosition(VERSION_X, VERSION_Y);
    _versionLabel->SetBackgroundColor(materialColorScheme->inverseOnSurface);
    _versionLabel->SetForegroundColor(materialColorScheme->onSurfaceVariant);
}

void ThemeListTopView::InitVram(const VramContext& vramContext)
{
    ViewContainer::InitVram(vramContext);
    _versionLabel->InitVram(vramContext);
}

void ThemeListTopView::Update()
{
    ViewContainer::Update();
    _versionLabel->Update();
}

void ThemeListTopView::Draw(GraphicsContext& graphicsContext)
{
    ViewContainer::Draw(graphicsContext);
    u32 oldPriority = graphicsContext.SetPriority(0);
    _versionLabel->Draw(graphicsContext);
    graphicsContext.SetPriority(oldPriority);
}

void ThemeListTopView::VBlank()
{
    ViewContainer::VBlank();
    _versionLabel->VBlank();
    REG_DISPCNT_SUB = (REG_DISPCNT_SUB & ~0xF) | 5 | (4 << 8);
    REG_BG2CNT_SUB = 0x4084;
    REG_BG2HOFS_SUB = 0;
    REG_BG2VOFS_SUB = 0;
    gfx_setSubBg2Affine(256, 0, 0, 256, 0, 0);

    int selectedItem = _viewModel->GetSelectedItem();
    if (selectedItem != _lastSelectedItem)
    {
        auto extraThemeInfo = _viewModel->GetSettingsController()->GetThemeInfoManager().GetExtraThemeInfo(selectedItem);
        if (extraThemeInfo)
        {
            dma_ntrCopy32(3, extraThemeInfo->previewImage, GFX_BG_SUB, 256 * 192 * 2);
            _lastSelectedItem = selectedItem;
        }
    }
}
