### sort-visualizer

Various sorting algorithms visualized.

Currently included algorithms:

- Bubble Sort
- Quick Sort (LR ptr)
- Insertion Sort


**Keybindings**

Note: all these bindings have a GUI button equivalent.

 - p, space         - pause/play
 - r                - restart
 - left, right      - cycle algorithms
 - up, down         - change number of elements
 - period, comma    - change delay


**Dependencies**:

- SDL2
- SDL2_ttf
- SDL2_gfx
- CMake

Build:

```bash
mkdir build && cd build && cmake .. && make
```

Run:

```bash
./sdlsort
```
#### License

This program is free software released under the MIT license.
For the full text of the license, see the 'LICENSE' file.
