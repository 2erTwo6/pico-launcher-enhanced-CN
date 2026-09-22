#pragma once
#include "core/SharedPtr.h"
#include "core/math/Rgb.h"
#include "gui/Alignment.h"
#include "romBrowser/views/BottomSheetView.h"
#include "gui/views/Label2DView.h"
#include "romBrowser/viewModels/AboutViewModel.h"

class MaterialColorScheme;
class IFontRepository;
class IVramManager;

/// @brief The about sheet, behind the small button in the menu's title row:
///        who made what, which build this is, and the controls that have no
///        button of their own.
///
/// Two columns at one level: the LNH team's chip logo, the one from the boot
/// splash, over "Pico Launcher by the LNH team", and the maker's avatar over
/// "Enhanced v1.8.0 by rasalopa". Under both, the repository the build came
/// from and its commit, which a fork of this fork names for itself. Then the
/// controls, the button and what it does, three rows at a time, scrolled with
/// up and down.
class AboutBottomSheetView : public BottomSheetView
{
    SHARED_ONLY(AboutBottomSheetView)

public:
    void InitVram(const VramContext& vramContext) override;
    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;
    bool HandleInput(const InputProvider& inputProvider, FocusManager& focusManager) override;
    SharedPtr<View> MoveFocus(const SharedPtr<View>& currentFocus,
        FocusMoveDirection direction, View* source) override;
    void Focus(FocusManager& focusManager) override;

protected:
    void Close() override;

private:
    static constexpr int VISIBLE_COMMANDS = 3;

    SharedPtr<AboutViewModel> _viewModel;
    const MaterialColorScheme* _materialColorScheme;
    SharedPtr<Label2DView> _upstreamName;
    SharedPtr<Label2DView> _upstreamBy;
    SharedPtr<Label2DView> _forkName;
    SharedPtr<Label2DView> _forkBy;
    SharedPtr<Label2DView> _buildLabel;
    SharedPtr<Label2DView> _commandButtons[VISIBLE_COMMANDS];
    SharedPtr<Label2DView> _commandLabels[VISIBLE_COMMANDS];
    /// "1-3 of 7": where the visible rows sit in the list, and the hint that it scrolls.
    SharedPtr<Label2DView> _rangeLabel;
    u32 _logoVramOffset = 0;
    u32 _avatarVramOffset = 0;
    int _scroll = -1;

    AboutBottomSheetView(SharedPtr<AboutViewModel> viewModel,
        const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository);

    SharedPtr<Label2DView> AddLabel(const IFontRepository* fontRepository, FontType fontType,
        u32 width, u32 maxChars, const char* text, Alignment alignment);
    void ShowCommandsFrom(int scroll);
    u32 LoadSprite(IVramManager& vramManager, const unsigned int* tiles, u32 tilesLength) const;
};
