# AMU Library Documentation

Doxygen-based docs for the AMU (Aerospace Measurement Unit) library.

## Build

```bash
python docs/build.py    # all platforms
```

The script pulls the theme submodule on first run, then runs Doxygen
(output -> `docs/html/index.html`). It can be run from any directory.

Requires [Doxygen](https://www.doxygen.nl/download.html).

## Files

- `build.py` — one-step build (submodule + Doxygen), cross-platform.
- `Doxyfile` — Doxygen config; defines the command-card aliases and wires in the theme.
- `mainpage.dox` — landing page (Introduction + subsections).
- `custom_enum_style.css` — command-card styling and theme layout tweaks.
- `header.html` — HTML header; loads the doxygen-awesome JS (dark-mode toggle, etc.).
- `DoxygenLayout.xml` — renames the "Topics" tab to "Commands".
- `themes/doxygen-awesome-css/` — theme (git submodule).

Command docs live **inline** in `src/amulibc/amu_commands.h`, above each enum value.
Each enum type is `@ingroup` one of eight `cmd_*` groups (system, dut, exec, sweep,
aux, adc, meas, memory) that appear directly under the **Commands** tab.

## Command-card format

Each command renders as a card: a title on the left; Description / Parameters /
Returns full-width on the right; then two interface panels side by side — **SCPI**
(green, the USB/text interface) and **I2C** (blue, the `CMD_t` id driven by the C++
`AMU` class). `CMD_USB_*` commands are USB-only: SCPI panel + a "USB only" badge, no
I2C panel.

| Alias | Purpose |
|-------|---------|
| `@amutitle{title}` | Card title (use `Category — Specific`, not `@brief`). |
| `@amudesc{text}` | One-line description. |
| `@amuusbonly` | "USB only" badge (on `CMD_USB_*` commands). |
| `@param` / `@return` | Native Doxygen params/returns; place **before** `@amupanels`. |
| `@amupanels` / `@endamupanels` | Open/close the panel row. |
| `@amuscpi{cmd}` / `@amui2c{enum}` | Open the SCPI / I2C panel. |
| `@amupanelex` / `@endamupanelex` | Example block inside a panel. |
| `@endamupanel` | Close the current panel. |

Rules:
- A blank `*` doc line **must** precede `@amupanels` (else the panels get absorbed
  into the Returns section).
- Escape literal commas inside `{...}` args as `\,` (Doxygen splits args on commas) — see the
  `@amudesc` in the example below.
- Open `@amui2c` only for `CMD_I2C_USB` commands.
- Order: title → desc → param/return → blank `*` → panels → notes/warnings.

Example:

```c
/** @amutitle{System — Temperature}
 *  @amudesc{Reads the built-in MCU temperature sensor\, factory calibrated}
 *  @return Temperature in °C (range -40 to +85)
 *
 *  @amupanels
 *  @amuscpi{SYSTem:TEMPerature?}
 *  @amupanelex
 *  SYSTem:TEMPerature?
 *  23.5
 *  @endamupanelex
 *  @endamupanel
 *  @amui2c{CMD_SYSTEM_TEMPERATURE}
 *  @amupanelex
 *  float tempC = amu.query<float>(CMD_SYSTEM_TEMPERATURE);
 *  @endamupanelex
 *  @endamupanel
 *  @endamupanels
 *  @note Accurate to ±3°C at 25°C.
 */
CMD_SYSTEM_TEMPERATURE = CMD_SYSTEM_OFFSET + 0x08,
```

Each card documents one command, and its I2C example follows two rules:

1. **Show the command being issued.** Skip pure getters like `getDutModel()` — they return
   cached state without issuing the command, so they wouldn't show what the card describes.
2. **Use the generic form**, `query<T>(CMD_t)` for reads and `sendCommand(CMD_t)` for actions,
   with the command's real return type. Some commands also have a named wrapper (e.g.
   `readFirmwareStr()`) that issues the command correctly, but only some do; using the generic
   form on every card keeps all of them uniform. (One exception: parameterized reads like
   `measureChannel(n)`, where the explicit `query<float>(CMD_MEAS_CH_VOLTAGE + n)` is more
   confusing than helpful.)

SCPI examples are the raw command-then-response transcript.

## Adding a command

1. Add the enum to `amu_commands.h` with a card as above (`@ingroup` the right `cmd_*`).
2. Add the matching `SCPI_COMMAND(pattern, handler, CMD_ID)` in `scpi.h` (same `CMD_ID`).
3. Rebuild with the [Build](#build) script (`python docs/build.py`).
