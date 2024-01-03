# RC Cockpit

The RC Cockpit (RCC) is an open source toolkit for building and controlling remote controlled vehicles over a network or other connection.

RCC uses a server-client model, where a microcontroller or computer on the vehicle runs a server program that controls the motors and other elements, while a controller device connects to the vehicle to send it commands and receive data about the vehicle state.

## License

Project available under GPLv3 (see `LICENSE` for full license text).

### Dependency Licenses

- Cockpit UI built using [Dear ImGui](https://github.com/ocornut/imgui) available under MIT license
- Camera control implemented using [`libcamera`](https://github.com/libcamera-org/libcamera/) available under LGPL 2.1 or later
