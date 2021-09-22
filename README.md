# Chanify Windows Client

[![Release](https://img.shields.io/github/v/release/chanify/chanify-win?logo=github&style=flat-square)](https://github.com/chanify/chanify-win/releases/latest)
[![Workflow](https://img.shields.io/github/workflow/status/chanify/chanify-win/ci?label=build&logo=github&style=flat-square)](https://github.com/chanify/chanify-win/actions?workflow=ci)
[![GitHub](https://img.shields.io/github/license/chanify/chanify-win?style=flat-square)](LICENSE)
[![Downloads](https://img.shields.io/github/downloads/chanify/chanify-win/total?style=flat-square)](https://github.com/chanify/chanify-win/releases/latest)

Chanify is a safe and simple notification tools. This repository is windows clinet for Chanify.

## Getting Started

- Create `Chanify.ini` files in the same directory as `Chanify.exe`.
- Double click `Chanify.exe`, then add `Chanify` into `Explorer Context menus`.

## Configuration

```ini
[client]
name=Test
endpoint=https://<endpoint>:<port>
sound=1
token=<senc node token>
interruption-level=<interruption-level>
```

| Key      | Default | Description                                 |
| -------- | ------- | ------------------------------------------- |
| name     | None    | The node server name.                       |
| endpoint | None    | The node server endpoint.                   |
| sound    | `0`     | `1` enable sound, otherwise disable sound.  |
| token    | None    | The node server token for send.             |

## Contributing

Contributions are what make the open source community such an amazing place to be learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

Distributed under the MIT License. See [`LICENSE`](LICENSE) for more information.
