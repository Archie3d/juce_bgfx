# Using bgfx & nanovg with JUCE

This code demonstrates how to use [bgfx](https://github.com/bkaradzic/bgfx) rendering from within a [JUCE](https://github.com/juce-framework/JUCE) application. Additionally an alternative rendering of JUCE components is implemented via [nanovg](https://github.com/memononen/nanovg) (over bgfx).

> When compiled as is bgfx will be using Direct3D on Windows and Metal on MacOS.

## Implementation notes

- On MacOS interaction between JUCE UI and bgfx is possible only on the main thread. Because of this bgfx multithreading must be disabled via `BGFX_CONFIG_MULTITHREADED=0`.

- Nanovg rendering is done via custom `juce::LowLevelGraphicsContext` that forwards drawing calls to nanovg. Unfortunately not all call from the JUCE graphics can be mapped directly to nanovg, but custom paint can be done by accessing the `NVGcontext` directly.

