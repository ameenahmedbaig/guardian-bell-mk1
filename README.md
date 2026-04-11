# Guardian Bell MK1

GuardianBell MK1 is an ESP32-CAM based smart IoT doorbell and surveillance system featuring motion-activated monitoring, intelligent alerts, and scheduled cloud uploads. It supports deep sleep optimization, multiple wake modes, MQTT and Telegram notifications, Home Assistant integration, and OTA firmware updates.

## OTA Update System

Firmware updates are delivered over-the-air (OTA) using GitHub Releases. The device checks for updates automatically on every cold boot (power-on reset) by comparing its local firmware version against the latest release hosted on GitHub.

### How It Works

1. **Trigger** — `checkForFirmwareUpdate()` runs during `setup()` on cold boot (`ESP_SLEEP_WAKEUP_UNDEFINED`). It is not called after PIR or timer wakeups.
2. **Version check** — The device fetches `version.txt` from the latest GitHub Release over HTTPS and compares its content against the local `FW_VERSION` string defined in `firmware/src/config/settings.cpp`. A simple string-equality check is used; if the strings match, the device is already up to date and no update is performed.
3. **Firmware download** — When the remote version differs from the local version, the device downloads `firmware.bin` from the same GitHub Release.
4. **Flash** — The binary is streamed directly into the ESP32's OTA partition using the Arduino `Update` library. The device uses a dual OTA partition layout (`app0` / `app1`) defined in `firmware/partitions/default_ota.csv`.
5. **Notification** — On success, a Telegram message is sent containing the old version, the new version, and the release notes fetched from `update_notes.txt`.
6. **Restart** — The device calls `ESP.restart()` to boot into the newly flashed firmware.

### Release Assets

Each GitHub Release must include the following three files:

| Asset | Purpose |
|---|---|
| `version.txt` | Plain-text version string (e.g. `v1.0.0-beta.3.2`) that the device compares against `FW_VERSION` |
| `firmware.bin` | Compiled firmware binary to be flashed OTA |
| `update_notes.txt` | Human-readable release notes sent to the user via Telegram after a successful update |

The OTA URLs point to `https://github.com/<user>/<repo>/releases/latest/download/<asset>` and are configured in the user's `secrets.cpp` file (see `firmware/src/config/secrets_example.cpp` for reference).

### Failure Handling

All OTA errors are treated as **non-fatal** — the device logs the error, sends a Telegram notification, and continues normal operation rather than halting. The following failure cases are handled:

| Failure | Behaviour |
|---|---|
| Version fetch HTTP error | Logs `OTA firmware version fetch failed`, skips update |
| Empty remote version string | Logs `No remote firmware version available`, skips update |
| Firmware download HTTP error | Logs `OTA firmware download failed` |
| Invalid content length (≤ 0) | Logs `OTA invalid content length` |
| `Update.begin()` fails | Logs `OTA firmware update start failed` |
| Incomplete write (bytes written ≠ content length) | Calls `Update.abort()`, logs `OTA firmware incomplete write` |
| `Update.end()` fails | Logs `OTA firmware update end failed` |
| `Update.isFinished()` returns false | Logs `OTA firmware update not finished` |

Because all errors are non-fatal, a failed OTA attempt will not brick the device. The existing firmware remains intact on the current OTA partition, and the device will retry on the next cold boot.

### Network & Security

- Communication uses `WiFiClientSecure` with HTTPS and strict redirect following (`HTTPC_STRICT_FOLLOW_REDIRECTS`), which is required for GitHub Release download URLs.
- Certificate validation is currently disabled (`setInsecure()`) to avoid TLS certificate management on the ESP32.