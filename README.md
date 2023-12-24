![logo](https://user-images.githubusercontent.com/4752473/185670169-8b27dcab-a6a9-4a9d-b1a7-ab4b136fdd65.jpg)

# Illumination

## Overview

Mix of [defold-illumination](https://github.com/astrochili/defold-illumination) and [defold-light-and-shadows](https://github.com/Dragosha/defold-light-and-shadows)
This example contains ready-to-use forward shading lighting mixed with realtime shadow from one source (the sun)

🎮 [Play HTML5 demo](https://d954mas.github.io/defold-illumination-new/) with 🔦 on the `E` key.

**Pros:**

1. Worked in mobile( defold-illumination not worked in my mobile looks like shader precision problem)
2. Frustum culling to light sources(from [scene3d](https://github.com/indiesoftby/defold-scene3d))
3. [Clustered forward shading](https://github.com/astrochili/defold-illumination/issues/1)
2. Dynamic shadow matrix(update the shadow matrix whenever the camera moves so that it ideally describes the camera's frustum)

**Cons:**

1. Remove normal and specular map. I do not use it in my games. So if you need them you can add them by yourself(look in illumination sources)
2. Web build contains some precision issues(need find and fix them):(

**How to use**

This is not library this is example so you need manualy add it to you project.
1)Copy illumination folder
2)Use illumination render script or add logic to your script
3)Look at example.collection and add necessary objects to your collection

**Support me:)**

If you like the extension you can support me on patreon.
It will help me make more items for defold.

[![](https://c5.patreon.com/external/logo/become_a_patron_button.png)](https://www.patreon.com/d954mas)

[!["Buy Me A Coffee"](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://www.buymeacoffee.com/d954mas)



## Credits

- Textures in the demo are from Tiny Texture Pack [1](https://screamingbrainstudios.itch.io/tiny-texture-pack)-[2](https://screamingbrainstudios.itch.io/tiny-texture-pack-2) by [Screaming Brain Studios](https://screamingbrainstudios.itch.io/).
- Specular and normal maps generated with [Normal Map Online](https://cpetry.github.io/NormalMap-Online/) by [@cpetry](https://github.com/cpetry).
- The header background picture by [Thor Alvis](https://unsplash.com/photos/sgrCLKYdw5g).
