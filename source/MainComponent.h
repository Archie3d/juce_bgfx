//
//  Copyright (C) 2022 Arthur Benilov <arthur.benilov@gmail.com>
//

#pragma once

#include <JuceHeader.h>

#include "NanovgComponent.h"

class MainComponent : public NanovgComponent
{
public:

    MainComponent();
    ~MainComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:

    juce::Label label;
    juce::TextButton button;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
