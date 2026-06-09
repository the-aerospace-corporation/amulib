# AMU Library

A C/C++, Arduino-compatible library for communicating with AMU (Aerospace Measurement Unit)
devices over I2C and USB/SCPI. It pairs a high-level `AMU` C++ class with full SCPI support for
measuring and controlling solar-cell test hardware, and supports multiple AMUs on one I2C bus.

## Documentation

**[Full documentation &amp; API reference](https://the-aerospace-corporation.github.io/amulib/html/index.html)**
— quick start, examples, the complete command reference, and hardware details.

## Installation

### Arduino IDE
1. Download this repository as a ZIP.
2. Sketch → Include Library → Add .ZIP Library.
3. Select the downloaded ZIP.

### PlatformIO
Add to your `platformio.ini`:
```ini
lib_deps =
    https://github.com/the-aerospace-corporation/amulib.git
```

## License

Released under the MIT License — see [LICENSE](LICENSE).

## Author

Colin Mann — [The Aerospace Corporation](https://aero.org).

## Contributing

Contributions are welcome — please open an issue or pull request for bugs and feature requests.
