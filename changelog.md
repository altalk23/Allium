# Changelog

## v1.0.10
 * Mod rotation values by 360 and make it positive to fix a bug where saving a negative decimal rotation offsets by 0.01 every time you close and reopen
 * Update to Geode v5

## v1.0.9
 * Add a setting to increase padding on triangles for the Polygon and Font importer tools to help with slight gaps being visible

## v1.0.8
 * Fix some fonts randomly switching winding order cause why not by reversing them if needed
 * Fix the parser counting the first curve twice if its cut off at the middle
 * Fix the parser not handling broken loca tables properly

## v1.0.7
 * Fix some fonts with multiple overlapping intersecting points by literally adding a random value to the points
 * Implement a parallelogram combination step thats pretty simple to shut up some people

## v1.0.6
 * Remove debug log in ttf parser
 * Fix broken ttf parsing shifting glyphs

## v1.0.5
 * Add support for non-BMP characters
 * Add support for Windows Symbol fonts (Wingdings etc.)
 * Fix lots of tessellation issues
 * Fix a bug where you would be able to write after finalizing the text

## v1.0.4
 * I hate MacOS (impl linking differently to fix another macos crash)

## v1.0.3
 * Fix MacOS crash
 * Enable iOS build

## v1.0.2
 * Fixed objects getting deselected on random bar changes
 * Made text defaults much less obj intensive

## v1.0.1
 * Fixed a bug where holes would get filled instead of not filled in text

## v1.0.0
 * Added support for custom text with a ttf font file
 * Added a text editor to edit the text
 * Added a polygon drawing tool
 * Holding shift while drawing aligns to the last point
 * Holding alt while drawing aligns to the grid

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
