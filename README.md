
<p align="center">
    <img src="Docs/ur_logo_atom_onlight_bgcircle.png"
        style="mix-blend-mode: difference;"
    />
</p>
<h1 align="center"> Uranium</h1>
<p align="center">Fast and versatile implementation of CEF for Unreal Engine</p>

## Features

* **Blueprint exposed API for manipulating CEF browsers**
* **Use shared texture directly from Chromium**
  * 4K+ HTML content at 60 FPS? No problem!
* **Browser View is environment agnostic:**
  * Use it with your own logic (texture on 3D object)
  * Use it in Slate/UMG
* **Convenient Slate widget and UMG wrapper**
  * Callbacks for managing multiple windows with UMG widgets
  * Customizable Slate or UMG widget for dropdowns (native popups)
  * Devtools can be also opened inside Unreal (for VR/AR apps)
* **Versatile input support**
  * Inject keyboard/mouse/touch events with exposed API

### WIP

* **Javascript interop**
  * Call arbitrary javascript code on the HTML content
  * Read or Manipulate any UObject or UStruct from Javascript
  * Callbacks on property changes.

### Planned

* **High level asset types using Uranium**
  * SVG assets (with animation) (+ Slate/UMG widget)
  * Lottie/BodyMovin assets (+ Slate/UMG widget)
  * Plain HTML assets
* **Browser Atlas**
  * Manage multiple smaller HTML components in a single large browser and use the result as a texture atlas rendered on many Unreal objects.
* **Audio output as USoundWave**

---

## Usage

### Requirements

* Unreal Engine 4.24 + (last tested with 4.26)
* Visual Studio 2019
* CMake (`choco install cmake` then make sure `cmake` is in PATH)
* .NET 5 for Nuke (only when working with [UraniumDeposit](https://github.com/microdee/UraniumDeposit))

### Installation

At the moment only supported way of using this plugin is via submoduling it into your code project. Beware this repository contains submodules which are not included in Github's *Download as Zip* functionality. It's highly recommended to
* use this plugin as a submodule in your project's `Plugins` folder
* OR clone it first with git to a temporary location, in case you don't want to use submodules or you use a different VCS.

### Getting started

TODO

### Documentation

TODO

#### CEF distribution

CEF distributables used by this plugin are not stored in a github repository, but they're stored on a self-hosted gitlab instance. [Link to CEF storage](https://lauch.10d.space/uranium/cef-storage)

## Test project (UraniumDeposit)

This plugin is being developed inside its [official test project (UraniumDeposit)](https://github.com/microdee/UraniumDeposit). It's using [Nuke.Unreal](https://github.com/microdee/Nuke.Unreal) for automation. See the [README.md](https://github.com/microdee/UraniumDeposit/blob/main/README.md) there for further documentation.

---

## Credits
* Originally started development at [MESO Digital Interiors GmbH.](https://meso.design)