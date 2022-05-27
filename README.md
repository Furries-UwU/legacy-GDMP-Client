# GDMP-Client

A Multiplayer mod for a game called "Geometry Dash"

## Build Instruction
- Install Geode First

```shell
git clone https://github.com/Furries-UwU/GDMP-Client.git --recursive
cmake . -A Win32 -DCMAKE_OSX_ARCHITECTURES=x86_64 # or x86_64, It need to be 32 bit
cmake --build . --config Release
```

## Credits

- [GD Programming Discord](https://discord.gg/jEwtDBK) - They help me a lot, They made this possible
- [ReplayBot](https://github.com/matcool/ReplayBot) - Lots of reference and resources

<!--
  "dependencies": [
    {
      "id": "com.geode.api",
      "required": true
    }
  ]
-->