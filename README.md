# d3d9to8

An experimental demake layer from the more "modern" D3D9 to D3D8, wherever/however possible. Support is currently limited to fixed-function only D3D9 titles.

Not to be confused with the much more useful [d3d8to9](https://github.com/crosire/d3d8to9). This wrapper goes the other way around, against all logic and common sense.

Known limitations include:
- Use of any SM2+ programmable shaders
- Calls to StretchRect
- D3D9Ex
- Other calls which have no earlier D3D8 equivalent

> [!IMPORTANT]
> Please don't submit issues or treat this as a serious project, because it's not. It will work at times, but in the vast majority of cases it's not expected to work properly/correctly. Its purpose is mainly for testing and bringing otherworldly things into existence, such as 64-bit d3d8.

