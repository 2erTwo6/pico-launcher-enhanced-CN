#pragma once
#include "core/math/Point.h"

class CustomTopStripElementInfo
{
public:
    /// @param specified Whether the theme placed this itself. Left out of the
    ///        theme, the launcher picks the spot (see the view factory).
    CustomTopStripElementInfo(const Point& position, bool hidden, bool specified = true)
        : _position(position), _hidden(hidden), _specified(specified) { }

    const Point& GetPosition() const { return _position; }
    bool GetIsHidden() const { return _hidden; }
    bool IsSpecified() const { return _specified; }

private:
    Point _position;
    bool _hidden;
    bool _specified;
};
