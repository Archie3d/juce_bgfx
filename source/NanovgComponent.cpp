//
//  Copyright (C) 2022 Arthur Benilov <arthur.benilov@gmail.com>
//

#include "NanovgComponent.h"

NanovgComponent::NanovgComponent()
    : BgfxComponent()
{
}

NanovgComponent::~NanovgComponent()
{
    if (nvg != nullptr)
    {
        nvgGraphicsContext->removeCachedImages();
        nvgDelete (nvg);
    }
}

void NanovgComponent::renderFrame()
{
    const float scale {getBgfxContext().getScaleFactor()};

    if (nvg == nullptr)
    {
        nvg = nvgCreate (1, 0);

        const float width {getWidth() * scale};
        const float height {getHeight() * scale};

        nvgGraphicsContext.reset (new NanovgGraphicsContext (nvg, (int)width, (int)height));
    }

    nvgBeginFrame (nvg, getWidth(), getHeight(), scale);

    renderNanovgFrame (nvg);

    nvgEndFrame (nvg);
}

void NanovgComponent::resized()
{
    if (nvgGraphicsContext != nullptr)
    {
        const auto scale {getBgfxContext().getScaleFactor()};
        nvgGraphicsContext->resized (int (getWidth() * scale), int (getHeight() * scale));
    }
}

void NanovgComponent::renderNanovgFrame(NVGcontext* nvg)
{
    Graphics g (*nvgGraphicsContext.get());
    paintEntireComponent (g, true);
}

