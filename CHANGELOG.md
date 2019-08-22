![Boden Logo](https://github.com/AshampooSystems/boden/blob/master/README/boden-github.svg)

<h3 align="center">BODEN CROSS-PLATFORM FRAMEWORK</h3>

<p align="center"><a href="https://twitter.com/bodenhq">Follow us on Twitter</a></p>

<p align="center">Find our new <a href="http://www.boden.io/reference">Documentation</a> at <a href="https://www.boden.io">boden.io</a>!

## [0.4]

### 🎉 Added

* **ui/Button**: The [`Button`](https://www.boden.io/reference/ui/button/#properties) now supports an imageURL to display an Image instead of Text.
* **ui/TextField**: The [`TextField`](https://www.boden.io/reference/ui/text_field/#properties) now supports a placeholder text that is displayed while no text was entered.
* **ui/Label**: The [`Label`](https://www.boden.io/reference/ui/label/#properties)'s method of handling overflow can now be set with the `textOverflow` property.
* **Application**: The new function [`Application::copyToClipboard`](https://www.boden.io/reference/foundation/application#os-services) copies a string to the Global Clipboard. Thanks [@mrexodia](https://github.com/mrexodia)!

### ⚠️ Changed

* **ui/View**: The View hierarchy functions have been reworked extensively. [`View::childViews`](https://www.boden.io/reference/ui/view/#view-hierarchy) returns a `std::vector` instead of a `std::list` and the parent [`View`](https://www.boden.io/reference/ui/view/) is accessible via a read-only Property [`View::parentView`](https://www.boden.io/reference/ui/view/#properties). Functions that change the hierarchy have been moved out of [`View`](https://www.boden.io/reference/ui/view/).
* **ui/View**: The `offerLayout` function has been renamed to [`setFallbackLayout`](https://www.boden.io/reference/ui/view/#layout)

### 🔥 Deprecated

* **String**: The [`String`](https://www.boden.io/reference/foundation/string/#types) alias has been deprecated and replaced with `std::string`

### 🐞 Fixed

* **ui/Label**: The [`Label`](https://www.boden.io/reference/ui/label/#properties)'s wrap property is now by default `false` on all platforms. 


## [0.3]

### 🎉 Added

* **foundation/AttributedString**: Added the [`AttributedString`](https://www.boden.io/reference/foundation/attributed_string/) class. `AttributedString` wraps the native platform implementation, i.e. `NSAttributedString` on iOS and `Spanned` on Android. You can use the `fromHTML()` and `toHTML()` functions to convert HTML markup code to/from an attributed strings.
* **ui/TextField**: Added the `font` property to the [`TextField`](https://www.boden.io/reference/ui/text_field) class. You can now set a custom [`Font`](https://www.boden.io/reference/foundation/font) on a text field using this property.
* **ui/TextField**: The [`TextField`](https://www.boden.io/reference/ui/text_field/#properties)'s return key type can now be set using the `returnKeyType` property.
* **ui/TextField**: The [`TextField`](https://www.boden.io/reference/ui/text_field/#properties)'s auto correction can now be turned on/off explicitly using the `autocorrectionType` property.
* **ui/TextField**: Text fields can now be focussed programatically using the [`setFocus()`](https://www.boden.io/reference/ui/text_field/#actions) action method. We've also added an example of how to implement navigation through multiple text fields within a form using the software keyboard's "next" button.

### ⚠️ Changed

* **ui/TriState**: `TriState` enum values are now capitalized to remain consistent with Boden's enum value capitalization style. Please make sure to change your code accordingly when updating to v0.3.

### 🐞 Fixed

* Symlinks in the Boden working tree's parent path are now supported by the `boden` command line tool.


## [0.2]

### 🎉 Added

* **layout**: New Flexbox layout engine based on Facebook's [Yoga](https://yogalayout.com/)
* **foundation**: [Properties](https://www.boden.io/guides/fundamentals/properties/) as data members: intuitive data bindings, change notifications, comprehensive type support
* **ui/ListView**: Fully native [`ListView`](https://www.boden.io/reference/ui/list_view/) with support for custom item views
* **ui/Slider**: [`Slider`](https://www.boden.io/reference/ui/slider/)
* **ui/NavigationView**: [`NavigationView`](https://www.boden.io/reference/ui/navigation_view/)
* **ui/ImageView**: [`ImageView`](https://www.boden.io/reference/ui/image_view/)
* **ui/WebView**: [`WebView`](https://www.boden.io/reference/ui/web_view/)
* **ui/LottieView**: [`LottieView`](https://www.boden.io/reference/extra-modules/lottie/lottie_view/)
* **platform**: Support for [bundling resources](https://www.boden.io/guides/fundamentals/resources/) (images, binary assets, etc.)
* **platform**: Support for device orientation
* **net**: [HTTP request API](https://www.boden.io/reference/net/http/) — easily make requests to web services without non-native net library dependencies
* **ui/View**: Generalized stylesheet support for views
* **ui/View**: Support for conditional stylesheet properties, similar to CSS media queries (e.g. different layouts per device)
* **ui/View**: Background color support
* **platform**: Support for App Icons
* **platform/android**: Support for day/night mode
* **ui**: [`ViewCoreFactory`](https://www.boden.io/reference/ui/view_core_factory/) mechanism to allow for independent UI modules
* **foundation**: [`init()` pattern via `std::make_shared`](https://www.boden.io/reference/foundation/needs_init/)
* **foundation**: nlohmann::json
* **ui**: Improved [`ViewCore`](https://www.boden.io/reference/ui/view_core/) initialization: controls can now be fully used before added to the view graph
* **ui**: [`CoreLess`](https://www.boden.io/reference/ui/core_less/) class for easier subclassing of container views that do not use `ViewCore`
* **foundation/java** New, more convenient template-based Java wrapping classes
* **ui/ListView**: Improved single selection set/get
* **ui/ListView**: Support for swipe down gesture
* **ui/ListView**: Support for custom item views 
* **foundation**: Consolidated Notifier architecture
* **website**: Built [boden.io](https://www.boden.io)
* **documentation**: Improved getting started guides
* **documentation**: LottieView documentation
* **documentation**: Net module documentation
* **documentation**: Switch documentation
* **documentation**: WebView documentation
* **documentation**: Property documentation
* **documentation**: Guides on how to write new views
* **examples**: Improved demo app
* **examples**: Exemplary Reddit browser app

### ⚠️ Changed

* **foundation**: Use `std::shared_ptr` instead of custom reference pointer
* **foundation**: Use `std::string` instead of custom string implementation
* **foundation**: Removed `Base` class
* **ui/Label**: Renamed `TextView` to `Label`


### 🐞 Fixed

* **platform/ios**: Soft keyboard overlaps textfields
* **tooling**: Fixed `boden new` command

### ⚖️ License

We added [LGPL](https://www.boden.io/legal/licensing/) as a new licensing option! Boden can now be licensed under GPL 2/3 or LGPL 2.1/3. This allows you to release iOS apps based on Boden on the iOS App Store without running into any licensing issues.

[Let us know what you think!](https://www.boden.io/feedback)