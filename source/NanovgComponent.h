//
//  Copyright (C) 2022 Arthur Benilov <arthur.benilov@gmail.com>
//

#pragma once

#include <JuceHeader.h>

#include "BgfxComponent.h"
#include "NanovgGraphics.h"

/**
    JUCE UI component rendered usin nanovg/bgfx.

    @note All normal JUCE components placed within this one will be
          rendered with nanovg as well.
*/
class NanovgComponent : public BgfxComponent
{
public:

    NanovgComponent();
    ~NanovgComponent();

    void renderFrame() override;

    virtual void renderNanovgFrame(NVGcontext* nvg);

    // juce::Component
    void resized() override;

private:

    NVGcontext* nvg {nullptr};
    std::unique_ptr<NanovgGraphicsContext> nvgGraphicsContext {nullptr};

    Colour backgroundColour {};
};
