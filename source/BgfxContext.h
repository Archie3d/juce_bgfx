//
//  Copyright (C) 2022 Arthur Benilov <arthur.benilov@gmail.com>
//

#pragma once

#include <JuceHeader.h>

/**
    Bgfx context is a low-level binding between bgfx
    and the native window system. The implementation is
    platform dependent.

    @note Due to bgfx limitations currently there can be only
          one bgfx context within the application. Creating another
          context will most likely fail.
*/
class BgfxContext final : public juce::ComponentListener
{
public:

    /** Overlay component that will have bgfx attached to it.

        The overlay component will pe placed over the component
        that has bgfx context attached. It will forward mouse events
        down to its peer underneath.
    */
    class Overlay final : public juce::Component
    {
    public:
        Overlay();

        void setForwardComponent (juce::Component*) noexcept;

        // juce::Component
        void mouseMove        (const MouseEvent&) override;
        void mouseEnter       (const MouseEvent&) override;
        void mouseExit        (const MouseEvent&) override;
        void mouseDown        (const MouseEvent&) override;
        void mouseDrag        (const MouseEvent&) override;
        void mouseUp          (const MouseEvent&) override;
        void mouseDoubleClick (const MouseEvent&) override;
        void mouseWheelMove   (const MouseEvent&, const MouseWheelDetails&) override;
        void mouseMagnify     (const MouseEvent&, float scaleFactor) override;

    private:

        juce::ComponentPeer* beginForward();
        void endForward();
        void forwardMouseEvent (const MouseEvent&);
        void forwardMouseWheelEvent (const MouseEvent&, const MouseWheelDetails&);
        void forwardMouseMagnifyEvent (const MouseEvent&, float scaleFactor);

        juce::Component* forwardComponent {nullptr};
        bool forwarding {false};
    };

    //==========================================================================

    BgfxContext();
    ~BgfxContext();

    /** Initialize the context.

        This will create the bgfx context and create the overlay
        component if required.
    */
    void init (float scaleFactor = 1.0f, bool showRenderStats = false);

    /** Attach the context to the given component. */
    void attachTo (juce::Component* component);

    /** Detach the context from the currently attached component. */
    void detach();

    /** Assign the background colour.

        The background colour is used upon bgfx context creation and
        re-creation. Once created the backround colour does not change.
    */
    void setBackgroundColour (const Colour& c);

    bool isInitialised() const noexcept { return initialised; }
    float getScaleFactor() const noexcept { return scale; }

    void reset();

    // juce::ComponentListener
    void componentMovedOrResized (juce::Component& component, bool wasMoved, bool wasResized) override;
    void componentBeingDeleted (juce::Component& component) override;

    void repaintPeer();

private:

    void trackOverlay (bool moved, bool resized);

#if JUCE_WINDOWS
    void updateWindowPosition (juce::Rectangle<int> bounds);
#endif

    bool initialised {false};
    float scale {1.0f};

    juce::Component* attachedComponent {nullptr};

    Overlay overlay{};

#if JUCE_MAC
    NSViewComponent embeddedView;
#elif JUCE_WINDOWS
    std::unique_ptr<ComponentPeer> nativeWindow {nullptr};
#else
#   error Unsupported platform
#endif

    uint32_t backgroundColour {0x000000ff};
};