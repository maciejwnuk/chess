# Chess

Project for educational purposes only.

## Compiling

### MacOS

```bash
g++ main.cpp Game.cpp -o run -I include -lallegro -lallegro_main -lallegro_primitives -lallegro_image -lallegro_font -lallegro_ttf -std=c++17
```

## Todos

- [ ] Do some refractoring
  - [ ] There is too much interaction between too many objects. Need to get rid of directly accessing registry, build some sort of API.
  - [ ] Extract some pieces of code into smaller compilation units. Break down Game class into independent pieces and make them work together.
  - [ ] Make use of observers.
- [ ] Make some moves possible
  - [ ] Castling
  - [ ] En passant
  - [ ] Promotion
- [ ] Add dragging possibility