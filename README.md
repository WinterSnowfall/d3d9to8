# d3d9to8

An experimental demake layer from the more "modern" D3D9 to D3D8, wherever/however possible. Support is currently limited to fixed-function only D3D9 titles.

Not to be confused with the much more useful [d3d8to9](https://github.com/crosire/d3d8to9). This wrapper goes the other way around, against all logic and common sense.

Known limitations include:
- D3D9Ex
- Use of any SM2+ programmable shaders
- Calls to StretchRect
- Surface calls to GetDC/ReleaseDC
- Use of various D3D9 exclusive sampler/texture stage states and texture formats
- Multiple swapchain use (thankfully, it is rare even in D3D9)
- SetStreamSource calls using offsets
- DrawIndexedPrimitive calls using a negative BaseVertexIndex (should be relatively rare)
- Other minor D3D9 exclusive API calls

> [!IMPORTANT]
> Please don't submit issues or treat this as a serious project, because it's not. It will work at times, especially with early D3D9 games, but in the vast majority of cases it's not expected to work properly/correctly. Its purpose is mainly for testing and bringing otherworldly things into existence, such as 64-bit D3D8.

## FAQ

### What possessed you to create such an abomination?

Why the dark forces of Chaos, of course. No, it was my love for D3D8 mostly, and an unnatural curiosity. My original goal was to get _W40K: Dawn of War - Definitive Edition_ in a workable state with D3D8, because it is a 64-bit D3D9 game, and 64-bit D3D8 doesn't actually exist. It would have been a nice test use case, however outside of a partially rendered main menu, it crashes when trying to start a game, due to known limitations that have made their way into the remastered version.

### Are there any known working games?

Among known working titles, I can mention: all the games in the original _W40K: Dawn of War_ collection (NOT the Definitive Edition, mind you), _Machinarium_, _Majesty HD_, _Seven Kingdoms: Ancient Adversaries_, _Beyond Divinity_, _Outcast 1.1_, _Sid Meyer's Pirates! (Live the Life)_ and a few others. The list might expand in the future, but probably not by a lot.

Since we report the same capabilities that a D3D8 level card would report to these D3D9 games, we rely on them having fallback paths for such cases. Many later D3D9 games do not, and will outright refuse to run. Some may run with some degree of visual artifacting, or crash at later points in time, when they run into something unexpected.

### Will it work on Windows?

It _should_ work just as well as on Linux/Wine, for the most part.

### How do I use it?

Simply dropping it next to the game executable will work fine in most cases. It will rely on an existing D3D8 implementation (hence a `d3d8.dll`) being present in your system path.

> [!WARNING]
> 64-bit D3D9 titles have no chance of working on Windows, since it does not provide a 64-bit implementation of D3D8.

### Is there any benefit in running those games with D3D8 as opposed to D3D9?

None whatsoever. D3D8-capable cards can still run D3D9 just fine, but with limited capabilites, so many games will refuse to start. In that regard, d3d9to8 can do no magic. In truth, you'd have better luck in such cases by sticking with D3D9.

### Will d3d9to8 work with DXVK?

Yes and no. Not with upstream DXVK, because its `d3d8.dll` will in turn rely on loading DXVK's `d3d9.dll`... but d3d9to8's `d3d9.dll` will already have been loaded, hence that will quickly end up in an infinite recursion and/or crash.

I was able to get it working with a statically linked DXVK `d3d8.dll`, much like D8VK was distributed originally. I may include such binaries as part of any d3d9to8 releases for convenience.

### What about games using SM2+, can't you translate all that to SM1?

No. I may be crazy, but I'm not that crazy. And in many cases that's an actual technical impossibility.

### Are there any options I can tinker with?

Not at this point, and I don't think I will add any in the future. This isn't that useful of a shim to be perfectly honest.

### Will there ever be a d3d9to7?

I sure hope not.

### I have a game that doesn't work, should I report an issue?

There's really no point, it is what it is. Games will either work or they won't, for various reasons. It's a roll of the die when it comes to how game code handles missing D3D9 features, or if that happens at all.

