# TxMake

A toolkit that allows for the extraction and creation of Kula texture files:

-   `txdump` - Dump .TGI files.
-   `txgenerate` - Generate compatible texture images before TGI creation.
-   `txmake` - Build final TGI file from a specified configuration file.

**ONLY .PNG IMAGES ARE SUPPORTED**

## Usage

Video tutorial coming soon.

## Building

TxMake uses [CMake](https://cmake.org/) for building and compiling the binaries.

## Libraries

TxMake uses **4** libraries which can be seen below:

-   [freeImage](https://freeimage.sourceforge.io/) - Image manipulation library. Used for opening, editing, and saving images. Licensed [here](https://freeimage.sourceforge.io/license.html).
-   [libimagequant](https://github.com/ImageOptim/libimagequant) - Image quantization library, used for reducing colors of an image to an indexed palette. Licensed [here](https://github.com/ImageOptim/libimagequant#license)
-   [ghc filesystem](https://github.com/gulrak/filesystem) - A cross-platform **std::filesystem** implementation for C++11, licensed under [MIT](https://github.com/gulrak/filesystem/blob/master/LICENSE).
-   [toml11](https://github.com/ToruNiina/toml11) - TOML configuration file library, licensed under [MIT](https://github.com/ToruNiina/toml11#licensing-terms).

**GHC and toml11** have been added to the repository as git submodules, since their usage are only header files. I _tried_ to add **FreeImage and libimagequant** as a CMake integration for easier usage, but I was unsuccessful. Thus, I've added the pre-compiled binaries for them inside of the `lib` folder.

## Changelog

**v1.0-beta.1**

-   Initial release.
