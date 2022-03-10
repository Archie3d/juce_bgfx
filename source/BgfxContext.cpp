//
//  Copyright (C) 2022 Arthur Benilov <arthur.benilov@gmail.com>
//

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include "BgfxContext.h"

#if JUCE_WINDOWS

#ifndef NOMINMAX
#   define NOMINMAX
#endif

#include <Windows.h>

namespace juce {
    extern ComponentPeer* createNonRepaintingEmbeddedWindowsPeer (Component&, void* parent);
}

#endif // JUCE_WINDOWS

namespace bgfx {
    extern InternalData g_internalData;
    extern PlatformData g_platformData;
} // namespace bgfx

//==============================================================================

BgfxContext::Overlay::Overlay()
{
    setOpaque (false);
    setInterceptsMouseClicks (true, true);
}

void BgfxContext::Overlay::setForwardComponent (juce::Component* comp) noexcept
{
    forwardComponent = comp;
}

juce::ComponentPeer* BgfxContext::Overlay::beginForward()
{
    if (!forwarding && forwardComponent != nullptr)
    {
        if (auto* peer = forwardComponent->getPeer())
        {
            // Prevent self-forwarding
            if (peer != getPeer())
            {
                forwarding = true;
                return peer;
            }
        }
    }

    return nullptr;
}

void BgfxContext::Overlay::endForward()
{
    jassert (forwarding);
    forwarding = false;
}

void BgfxContext::Overlay::forwardMouseEvent (const MouseEvent& event)
{
    if (auto* peer {beginForward()})
    {
        peer->handleMouseEvent(event.source.getType(), event.position, event.mods, event.pressure,
                               event.orientation, event.eventTime.toMilliseconds());
        endForward();
    }
}

void BgfxContext::Overlay::forwardMouseWheelEvent (const MouseEvent& event, const MouseWheelDetails& wheel)
{
    if (auto* peer {beginForward()})
    {
        peer->handleMouseWheel (event.source.getType(), event.position, event.eventTime.toMilliseconds(), wheel);
        endForward();
    }
}

void BgfxContext::Overlay::forwardMouseMagnifyEvent (const MouseEvent& event, float scaleFactor)
{
    if (auto* peer {beginForward()})
    {
        peer->handleMagnifyGesture (event.source.getType(), event.position, event.eventTime.toMilliseconds(), scaleFactor);
        endForward();
    }
}

void BgfxContext::Overlay::mouseMove (const MouseEvent& event)
{
    forwardMouseEvent (event);
}

void BgfxContext::Overlay::mouseEnter (const MouseEvent& event)
{
    forwardMouseEvent (event);
}

void BgfxContext::Overlay::mouseExit (const MouseEvent& event)
{
    forwardMouseEvent (event);
}

void BgfxContext::Overlay::mouseDown (const MouseEvent& event)
{
    forwardMouseEvent (event);
}

void BgfxContext::Overlay::mouseDrag (const MouseEvent& event)
{
    forwardMouseEvent (event);
}

void BgfxContext::Overlay::mouseUp   (const MouseEvent& event)
{
    forwardMouseEvent (event);
}

void BgfxContext::Overlay::mouseDoubleClick (const MouseEvent& event)
{
    forwardMouseEvent (event);
}

void BgfxContext::Overlay::mouseWheelMove (const MouseEvent& event, const MouseWheelDetails& wheel)
{
    forwardMouseWheelEvent (event, wheel);
}

void BgfxContext::Overlay::mouseMagnify (const MouseEvent& event, float scaleFactor)
{
    forwardMouseMagnifyEvent (event, scaleFactor);
}

//==============================================================================

BgfxContext::BgfxContext()
{
#if JUCE_MAC
    overlay.addToDesktop (ComponentPeer::windowRepaintedExplictly);
#endif
}

BgfxContext::~BgfxContext()
{
    detach();

    if (initialised)
        bgfx::shutdown();

    memset (&bgfx::g_internalData, 0, sizeof (bgfx::InternalData));
    memset (&bgfx::g_platformData, 0, sizeof (bgfx::PlatformData));
}

void BgfxContext::attachTo (juce::Component* component)
{
    if (attachedComponent != nullptr)
    {
        detach();
        attachedComponent = nullptr;
    }


    if (component != nullptr)
    {
        attachedComponent = component;
        attachedComponent->addComponentListener (this);
#if JUCE_MAC
        attachedComponent->addAndMakeVisible (embeddedView);
#elif JUCE_WINDOWS
        attachedComponent->addAndMakeVisible (overlay);
#endif
        overlay.setForwardComponent (attachedComponent);
    }
}

void BgfxContext::detach()
{
    if (attachedComponent != nullptr)
    {
        attachedComponent->removeComponentListener (this);
#if JUCE_MAC
        attachedComponent->removeChildComponent (&embeddedView);
#endif
        attachedComponent = nullptr;
        overlay.setForwardComponent (nullptr);
        overlay.setVisible (false);
    }
}

void BgfxContext::setBackgroundColour (const Colour& c)
{
    const auto argb {c.getARGB()};
    backgroundColour = (argb << 8) | (argb >> 24);
}

void BgfxContext::reset()
{
        const auto width = uint32_t (scale * overlay.getWidth());
    const auto height = uint32_t (scale * overlay.getHeight());
    bgfx::reset (width, height, BGFX_RESET_VSYNC | BGFX_RESET_HIDPI);

    bgfx::setViewClear (0,
                        BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL,
                        backgroundColour,
                        1.0f,
                        0);
}

void BgfxContext::componentMovedOrResized (juce::Component& component, bool wasMoved, bool wasResized)
{
    if (!initialised)
        return;

    trackOverlay (wasMoved, wasResized);
}

void BgfxContext::componentBeingDeleted (juce::Component& component)
{
    if (attachedComponent == &component)
        attachedComponent = nullptr;
}

void BgfxContext::repaintPeer()
{
#if JUCE_WINDOWS
    if (nativeWindow != nullptr)
        nativeWindow->repaint (juce::Rectangle<int> (0, 0, overlay.getWidth(), overlay.getHeight()));
#endif
}

void BgfxContext::init (float scaleFactor, bool showRenderStats)
{
    if (attachedComponent == nullptr || initialised)
        return;

    scale = jmax (1.0f, scaleFactor);

    bgfx::Init init;

    // Here we can select the rendering back-end
    //init.type = bgfx::RenderType::OpenGL;

    init.resolution.width = uint32_t (scale * attachedComponent->getWidth());
    init.resolution.height = uint32 (scale * attachedComponent->getHeight());
    init.resolution.reset = BGFX_RESET_VSYNC | BGFX_RESET_HIDPI;

    overlay.setVisible (true);

#if JUCE_WINDOWS
    nativeWindow.reset (createNonRepaintingEmbeddedWindowsPeer (overlay,
                                                                overlay.getTopLevelComponent()->getWindowHandle()));
    nativeWindow->setVisible (true);
#endif

    auto* peer =
#if JUCE_MAC
        overlay.getPeer();
#elif JUCE_WINDOWS
        nativeWindow.get();
#else
        nillptr;
#endif

    jassert (peer != nullptr);

    init.platformData.nwh = peer->getNativeHandle();

    // NOTE: this may block on MacOS if BGFX_CONFIG_MULTITHREADED == 1
    bgfx::init (init);

    if (showRenderStats)
    {
        bgfx::setDebug (BGFX_DEBUG_PROFILER | BGFX_DEBUG_STATS);
    }

    bgfx::setViewClear (0,
                       BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL,
                       backgroundColour,
                       1.0f,
                       0);

    bgfx::setViewMode (0, bgfx::ViewMode::Sequential);

#if JUCE_MAC
    embeddedView.setView (peer->getNativeHandle());
#endif

    initialised = true;

    trackOverlay (false, true);

#if JUCE_WINDOWS
    overlay.getTopLevelComponent()->repaint();
#endif
}

void BgfxContext::trackOverlay (bool moved, bool resized)
{
    if (attachedComponent)
    {
        juce::Rectangle<int> bounds (0, 0, attachedComponent->getWidth(), attachedComponent->getHeight());
#if JUCE_MAC
        embeddedView.setBounds (bounds);
#elif JUCE_WINDOWS

        overlay.setBounds (bounds); // TODO: Do we need to do this?

        auto* topComp = overlay.getTopLevelComponent();

        if (auto* peer = topComp->getPeer())
            updateWindowPosition (peer->getAreaCoveredBy (overlay));
#endif

        if (moved)
        {
            // Update scale for the current display
            if (auto* display = Desktop::getInstance().getDisplays().getPrimaryDisplay())
                scale = (float) display->scale;
        }

        if (resized)
            reset();
    }
}

#if JUCE_WINDOWS

void BgfxContext::updateWindowPosition (juce::Rectangle<int> bounds)
{
    if (nativeWindow != nullptr)
    {
        double nativeScaleFactor = 1.0;

        if (auto* peer = overlay.getTopLevelComponent()->getPeer())
            nativeScaleFactor = peer->getPlatformScaleFactor();

        if (! approximatelyEqual (nativeScaleFactor, 1.0))
            bounds = (bounds.toDouble() * nativeScaleFactor).toNearestInt();

        SetWindowPos ((HWND) nativeWindow->getNativeHandle(), 0,
            bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
            SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER);
    }
}

#endif
