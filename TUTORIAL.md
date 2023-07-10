# TxMake Tutorial

This will be a simple text tutorial explaining how to use the TxMake toolkit.

Before we start, you need bare minimum 2 things, the **skybox image** and the **tileset image**:

-   The skybox image **needs** to be a 775x384 PNG image. This is what will be wrapped around the skybox and used as the background of the theme. An example can be found [here](/.github/skybox.png).
-   The tileset image **needs** to be a 896x256 PNG image, where every tile is 64x64 pixels. An example can be found [here](/.github/tileset.png).

## TxGenerate

Now that we have our two images, we need to use the `txgenerate` program to take these images and generate ones compatible for importing into the game. For simplicity sake, create a folder with the name of your theme in the same directory as the tools. This is where we will output our generated images in. Your file structure should look like this:

![Screenshot 1](/.github/screenshot-1.jpg?raw=true)

For this example, I am going to import a theme called **Space** into the game. Credits to Murphy for the tileset, the source of the skybox image is unknown.

First, we will use txgenerate to take our skybox image and generate a compatible skybox image. Open a command line in the same directory, and run the following command:
`txgenerate skybox skybox.png space/skybox.png`

Replace `skybox.png` and `space/skybox.png` with the **input** and **output** paths of your skybox respectively. Wherever you specified the output path of your skybox image, the command should've generate an image like so:

![Screenshot 2](/.github/screenshot-2.jpg?raw=true)

Now that we've generate our compatible skybox image, run the following similar command to generate compatible tilesets from our tileset image:
`txgenerate tilesets tileset.png space`

Replace `tileset.png` and `space` with the **input** path of your tileset image, and the **output** folder of where you'd like the generated tileset contents to be put inside of, respectively. Inside the output folder, the following contents should've been generated, including our skybox that we also generated:

![Screenshot 3](/.github/screenshot-3.jpg?raw=true)

-   `tileset-64.png` - Our 64x64 that has been automatically [quantized](<https://en.wikipedia.org/wiki/Quantization_(image_processing)>).
-   `tileset-32.png` - A 32x32 tileset image that has been automatically generated.
-   `tileset-16.png` - A 16x16 tileset image that has been automatically generated.
-   `tileset-8.png` - An 8x8 tileset image that has been automatically generated.
-   `palette-64.png` - A palette image for the 64x64 tileset that has been automatically generated.
-   `palette-64.light.png` - A light-palette image for the 64x64 tileset that has been automatically generated.
-   `palette-64.dark.png` - A dark-palette image for the 64x64 tileset that has been automatically generated.
-   `palette-mip.png` - A palette image for the mip tilesets that has been automatically generated.
-   `palette-mip.light.png` - A light-palette image for the mip tilesets that has been automatically generated.
-   `palette-mip.dark.png` - A dark-palette image for the mip tilesets that has been automatically generated.

The 32x32, 16x16, and 8x8 are auto generated smaller versions of your 64x64 tileset. These are known as [MIP steps or MIP maps](https://en.wikipedia.org/wiki/Mipmap), and are used by the game as an optimization feature.

You'll notice that the **dark and light** versions of the palettes that were generated look the exact same. Open your preferred image editing software, and darken / lighten the dark / light versions of the palettes. Once you've edited the lighter and darker versions of the palettes, your folder should look like this:

![Screenshot 4](/.github/screenshot-4.jpg?raw=true)
_Look closely at the darker and lighter versions of the palettes._

## TxMake

Now that our compatible texture images have been generated, copy the `txtemplate.toml` configuration file into the same directory as all of your texture images. You can optionally rename the config file to your liking.

This config file will tell the `txmake` tool the paths to all of our texture images and the options for our theme. If you've never had experience editing .TOML files, they're kind of like .INI files. Here's an excerpt towards the end of the config file:

```toml
#...

# Paths to skybox and tileset textures
[textures]
skybox     = "skybox.png"
tileset-64 = "tileset-64.png"
tileset-32 = "tileset-32.png"
tileset-16 = "tileset-16.png"
tileset-8  = "tileset-8.png"

# Paths to 64x64 tileset palettes
[palettes-64]
normal = "palette-64.png"
light  = "palette-64.light.png"
dark   = "palette-64.dark.png"

# Paths to mip-step tileset palettes (32x32, 16x16, 8x8)
[palettes-mip]
normal = "palette-mip.png"
light  = "palette-mip.light.png"
dark   = "palette-mip.dark.png"
```

Here you can specify the paths to all of the generated texture image files. **Assuming that you didn't rename them, you can leave these by default.**

In the beginning of the config file, you can change properties like lighting adjustments, which sides of the blocks are darkened or lit, and whether the tiles are randomly rotated or not (only used in **Hills**). Unfortunately as of right now, I cannot document every one of these properties in this document, so you'll have to play with them yourself, but they're pretty easy to figure out and there are comments on each property.

Once you've edited the config file to your preference, run the following and final command to generate the TGI file from the config file:
`txmake space/space.toml SPACE.TGI`

Replace `space/space.toml` and `SPACE.TGI` with the **input** path to your config file and the **output** path for the generated TGI file, respectively. Simply import the TGI file into the game, and you're done!

## Notes

1. Make sure that if you modify any texture images in your theme, **do not** edit the ones automatically generated by `txgenerate`! Only modify the original sources, and re-generate!
2. When editing the lighter and darker versions of the palettes, make sure to **apply the same** light and dark filters and effects on the MIP and 64 palettes, otherwise it would look weird.
