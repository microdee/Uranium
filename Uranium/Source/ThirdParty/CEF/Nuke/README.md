# Using couple of Nuke targets to automate building CEF for Uranium

It's recommended to work with CEF/Chromium sources in their own dedicated folder outside of this repository. These targets are not necessary to execute in order to build Uranium for UE4 as a regular usage. They're stored here as the facility which helps creating the CEF distributables Uranium can work with.

Uranium can work with official distributables but it cannot use sandboxed sub-processes in that case, it cannot use proprietary codecs (h.264, h.265) and it won't be able to use OnAcceleratedPaint (shared textures) (as of time of writing). For those CEF needs to be built from source.