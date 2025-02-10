# Session-lock binding for Qt

This lib is the binding of ext-session-lock, for qt. This repo is used to build a lock program for wayland wm.

## Dependiences

If you are using archlinux, In order to build this lib, you need to run

```bash
sudo pacman -S cmake base-devel qt6-wayland
```

## Usage in cmake

```cmake
find_package(SessionLockQt REQUIRED)

target_link_libraries(YourProgram PRIVATE
  Qt6::Quick
  Qt6::WaylandClient
  SessionLockQt::Interface
)
```


## Example repo

[waycratelock](https://github.com/waycrate/waycratelock)
