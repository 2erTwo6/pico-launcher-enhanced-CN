#pragma once
#include "../IRomBrowserController.h"

/// @brief View model for the about sheet. All it can do is close.
class AboutViewModel
{
public:
    explicit AboutViewModel(IRomBrowserController* romBrowserController)
        : _romBrowserController(romBrowserController) { }

    void Close() { _romBrowserController->HideAbout(); }

private:
    IRomBrowserController* _romBrowserController;
};
