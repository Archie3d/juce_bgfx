//
//  Copyright (C) 2022 Arthur Benilov <arthur.benilov@gmail.com>
//

#pragma once

#include <JuceHeader.h>

#include <bgfx/bgfx.h>
#include <nanovg.h>

/**
    JUCE low level graphics context backed by nanovg.

    @note This is not a perfect translation of the JUCE
          graphics, but its still quite usable.
*/
class NanovgGraphicsContext : public LowLevelGraphicsContext
{
public:
    NanovgGraphicsContext (NVGcontext* nanovgContext, int width, int height);
    ~NanovgGraphicsContext();

    // juce::LowLevelGraphicsContext
    bool isVectorDevice() const override;
    void setOrigin (juce::Point<int>) override;
    void addTransform (const AffineTransform&) override;
    float getPhysicalPixelScaleFactor() override;

    bool clipToRectangle (const Rectangle<int>&) override;
    bool clipToRectangleList (const RectangleList<int>&) override;
    void excludeClipRectangle (const Rectangle<int>&) override;
    void clipToPath (const Path&, const AffineTransform&) override;
    void clipToImageAlpha (const Image&, const AffineTransform&) override;

    bool clipRegionIntersects (const Rectangle<int>&) override;
    Rectangle<int> getClipBounds() const override;
    bool isClipEmpty() const override;

    void saveState() override;
    void restoreState() override;

    void beginTransparencyLayer (float opacity) override;
    void endTransparencyLayer() override;

    void setFill (const FillType&) override;
    void setOpacity (float) override;
    void setInterpolationQuality (Graphics::ResamplingQuality) override;

    void fillRect (const Rectangle<int>&, bool) override;
    void fillRect (const Rectangle<float>&) override;
    void fillRectList (const RectangleList<float>&) override;
    void fillPath (const Path&, const AffineTransform&) override;
    void drawImage (const Image&, const AffineTransform&) override;
    void drawLine (const Line<float>&) override;

    void setFont (const Font&) override;
    const Font& getFont() override;
    void drawGlyph (int glyphNumber, const AffineTransform&) override;
    bool drawTextLayout (const AttributedString&, const Rectangle<float>&) override;

    void resized (int w, int h);

    void removeCachedImages();

    const static String defaultTypefaceName;

    const static int imageCacheSize;

private:

    bool loadFontFromResources (const String& typefaceName);
    void applyFillType();
    void applyStrokeType();
    void applyFont();

    int getNvgImageId (const Image& image);
    void reduceImageCache();

    NVGcontext* nvg{};

    int width{};
    int height{};

    FillType fillType{};
    Font font{};

    // Mapping glyph number to a character
    using GlyphToCharMap = std::map<int, wchar_t>;

    GlyphToCharMap getGlyphToCharMapForFont (const Font& f);

    // Mapping font names to glyph-to-character tables
    std::map<String, GlyphToCharMap> loadedFonts{};
    const GlyphToCharMap* currentGlyphToCharMap{};

    // Tracking images mapped tomtextures.
    struct NvgImage
    {
        int id {-1};            ///< Image/texture ID.
        int accessCounter {0};  ///< Usage counter.
    };

    std::map<uint64, NvgImage> images;
};
