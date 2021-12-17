# CEF-storage

CEF distributables are stored in 7-Zip archives for easier version control. When necessary they're all extracted via `unpack.ps1` by the build scripts.

# Patches

Uranium might require some modifications of CEF or Chromium. These are stored in `Patches` folder in the form of Git patch files, which has to be applied to the CEF repository inside `<path to chromium_cef work dir>/chromium_git/chromium/src/cef`. Order of applying these patches might matter, so please follow the `###_*.patch` pattern for the patch files. Continuity of prefix numbers doesn't matter.