# wayland-crosshair

A minimal crosshair overlay for Wayland compositors (Hyprland, Sway, etc.).

Displays a small green dot at the exact center of your screen as a fullscreen transparent overlay. Designed for gaming — it is completely invisible to the input system: no focus, no keyboard grabs, no mouse interception. Clicks and cursor movement pass straight through to the game underneath.

## Features

- Zero input interference — uses an empty Wayland input region so the overlay cannot be focused or clicked
- `wlr-layer-shell` overlay layer — renders above fullscreen games on Wayland
- True screen center — anchors to all four edges so the dot is always at exact pixel center regardless of resolution
- Negligible resource usage — static surface, redrawn once at startup

## Dependencies

- `gtk3`
- `gtk-layer-shell`

Install on Arch Linux:

```bash
sudo pacman -S gtk3 gtk-layer-shell
```

Install on Fedora:

```bash
sudo dnf install gtk3-devel gtk-layer-shell-devel
```

## Build

```bash
make
```

## Usage

```bash
# Crosshair only
./crosshair

# Help
./crosshair -h/-help

# Crosshair with custom position, radius, and color
./crosshair [height], [width], [radius], [red], [green], [blue]
./crosshair 0.5, 0.5, 2.0, 0.0, 0.6, 0.9

# Currently every argument needs to be passed in order,
# But you can put an 'x' in place of a value to keep default.
# To just change the color:
./crosshair x, x, x, 0.0, 0.6, 0.9
```

Kill it with:

```bash
pkill crosshair
```

## Why not a Python/Electron/etc. script?

Most simple overlay examples steal input focus because they don't set `keyboard_interactivity = NONE` on the layer shell surface, and don't set an empty `wl_input_region`. This tool sets both, making it safe to run during gameplay.

## License

MIT
