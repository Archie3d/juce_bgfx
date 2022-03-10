//
//  Copyright (C) 2022 Arthur Benilov <arthur.benilov@gmail.com>
//

#pragma once

#include <JuceHeader.h>

#include "BgfxContext.h"
#include "NanovgGraphics.h"

/**
    JUCE UI Component that performs bgfx rendering.

    The actual rendering must be performed in renderFrame()
    method of inherited component.
*/
class BgfxComponent : public juce::Component,
                      private juce::Timer
{
public:

    class RenderCache : public CachedComponentImage,
                        private AsyncUpdater
    {
    public:
        RenderCache (BgfxComponent& comp);
        ~RenderCache();

        void paint (Graphics&) override;
        bool invalidateAll() override;
        bool invalidate (const juce::Rectangle<int>&) override;
        void releaseResources() override;

    private:
        void handleAsyncUpdate() override;

        BgfxComponent& component;
    };

    //------------------------------------------------------

    BgfxComponent();
    ~BgfxComponent();

    bool isInitialised() const;

    void setBackgroundColour (Colour c);
    void enableRenderStats();
    void startPeriodicRepaint(int fps = 30);
    void stopPeriodicRepaint();

    // Component is opaque and JUCE requires it to implement paint method.
    void paint (Graphics&) override {}

    virtual void renderFrame() {}

    BgfxContext& getBgfxContext() noexcept { return bgfxContext; }

private:

    friend class BgfxComponent::RenderCache;

    void paintComponent();

    void timerCallback() override;

    BgfxContext bgfxContext;
    bool currentlyPainting {false};

    Colour backgroundColour {};
    bool showRenderStats {false};
};
