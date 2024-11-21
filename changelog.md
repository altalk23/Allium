# Changelog

## v0.3.5-alpha
 * 2.2074 support
 * what else alk adds idk

## v0.3.4-alpha
 * MacOS support (excluding the fact Custom Keybinds still isn't on MacOS) (thank SpaghettDev on Github)
 * Fix tab opacity on the icon when not selected
 * Uses Alpha's Editor Tab API for the mod tab

## v0.3.3-alpha
 * Properly fix lines to origin in curve drawing for Android

## v0.3.2-alpha
 * Added Custom Keybinds as a dependency for all platforms
 * Prevent scale resetting by not generating tiny scaled objects (< 0.001)
 * Fix the unnecessary lines to origin in Android

## v0.3.1-alpha
 * Fixed the touch prio in support popup

## v0.3.0-alpha
 * Implemented freeform brush drawing using the [Ramer–Douglas–Peucker algorithm] (https://en.wikipedia.org/wiki/Ramer–Douglas–Peucker_algorithm)
 * New UI! Moved everything into a build tab, with new buttons and design
 * Added a support button, I appreciate any amount of support!
 * Fixed crash related to finalizing curved drawing early
 * Changed the behavior of line drawing endpoints for consistency
 * Line thickness, line color, curve detail and freeform threshold are all adjustable now!

## v0.2.0-alpha
 * Implemented simple curve drawing support with the use of [Anti-Grain Geometry library] (https://github.com/ghaerr/agg-2.6)
 * Added a finalize button to finalize the curve easier
 * Added a panning button to enable pannign while doing drawing
 * Fixed a crash issue related to leaving the brush open and exiting the editor

## v0.1.1-alpha
 * Added Android support in mod.json

## v0.1.0-alpha
 * Initial release, only supports line drawings.
