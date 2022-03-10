//
//  Copyright (C) 2022 Arthur Benilov <arthur.benilov@gmail.com>
//

#include "BgfxComponent.h"

BgfxComponent::RenderCache::RenderCache (BgfxComponent& comp)
    : component {comp}
{
}

BgfxComponent::RenderCache::~RenderCache()
{
    cancelPendingUpdate();
}

void BgfxComponent::RenderCache::paint (Graphics&)
{
    component.paintComponent();
}

bool BgfxComponent::RenderCache::invalidateAll()
{
#if JUCE_WINDOWS
    triggerAsyncUpdate();
#endif

    return true;
}

bool BgfxComponent::RenderCache::invalidate (const juce::Rectangle<int>&)
{
    return invalidateAll();
}

void BgfxComponent::RenderCache::releaseResources()
{
}

void BgfxComponent::RenderCache::handleAsyncUpdate()
{
    component.paintComponent();
}

//==============================================================================

BgfxComponent::BgfxComponent()
{
    setOpaque (true);
    setCachedComponentImage (new RenderCache (*this));
    bgfxContext.attachTo (this);
}

BgfxComponent::~BgfxComponent()
{
}

bool BgfxComponent::isInitialised() const
{
    return bgfxContext.isInitialised();
}

void BgfxComponent::setBackgroundColour (Colour c)
{
    // The background colour must be set before the component gets painter
    // for the very first time. Once initialised, changing the backgrounf colour
    // will have no effect.
    jassert(!isInitialised());
    backgroundColour = c;
}

void BgfxComponent::enableRenderStats()
{
    // Enabling the rendering statistics can be done only before the component
    // is initialised. Enabling it afterwards will have no effect.
    jassert(!isInitialised());
    showRenderStats = true;
}

void BgfxComponent::startPeriodicRepaint (int fps)
{
    if (fps > 0)
        startTimerHz (fps);
    else
        stopTimer();
}

void BgfxComponent::stopPeriodicRepaint()
{
    stopTimer();
}

void BgfxComponent::paintComponent()
{
    if (currentlyPainting)
        return;

    currentlyPainting = true;

    if (! bgfxContext.isInitialised())
    {
        if (getBounds().isEmpty())
        {
            // Component placement is not ready yet - postpone initialisation.
            currentlyPainting = false;
            return;
        }

        bgfxContext.setBackgroundColour (backgroundColour);

        float scale {1.0f};

        if (auto* display {Desktop::getInstance().getDisplays().getPrimaryDisplay()})
            scale = display->scale;

        bgfxContext.init (scale, showRenderStats);
    }


    const float scale {bgfxContext.getScaleFactor()};
    const float width {getWidth() * scale};
    const float height {getHeight() * scale};

    bgfx::touch (0);
    bgfx::setViewRect (0, 0, 0, (uint16_t)width, (uint16_t)height);

    renderFrame();

    bgfx::frame();

    currentlyPainting = false;
}

void BgfxComponent::timerCallback()
{
    repaint();
}
