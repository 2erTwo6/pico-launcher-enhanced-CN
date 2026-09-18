#pragma once
#include "core/SharedPtr.h"
#include "romBrowser/views/BottomSheetView.h"
#include "gui/views/Label2DView.h"
#include "romBrowser/viewModels/StatisticsViewModel.h"

class MaterialColorScheme;
class IFontRepository;

/// @brief Bottom sheet with library statistics: the folder's game count,
///        played/favorite counts, total launches, most played games and last
///        played game.
class StatisticsBottomSheetView : public BottomSheetView
{
    SHARED_ONLY(StatisticsBottomSheetView)

public:
    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;
    bool HandleInput(const InputProvider& inputProvider, FocusManager& focusManager) override;
    void Focus(FocusManager& focusManager) override;

protected:
    void Close() override;

private:
    // folder count, played, launches, three most played, last played
    static constexpr u32 MAX_LINES = 7;

    SharedPtr<StatisticsViewModel> _viewModel;
    SharedPtr<Label2DView> _titleLabel;
    SharedPtr<Label2DView> _lines[MAX_LINES];
    u32 _lineCount = 0;
    const MaterialColorScheme* _materialColorScheme;

    StatisticsBottomSheetView(SharedPtr<StatisticsViewModel> viewModel,
        const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository);

    void AddLine(const IFontRepository* fontRepository, FontType fontType, const char* text);
};
