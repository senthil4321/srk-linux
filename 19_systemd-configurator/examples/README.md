# Example systemd unit files

The examples are grouped by unit type for readability. Load any of these in the SPA via the Load button.

- 01_service/
  - hello.service
  - advanced.service
  - forking.service
  - oneshot.service
  - notify.service
- 02_socket/
  - echo.socket, echo.service
  - tcpecho.socket, `tcpecho@.service`
- 03_target/
  - custom.target
- 04_device/
  - sda.device
- 05_mount/
  - mnt-example.mount
- 06_automount/
  - data.automount
- 07_timer/
  - backup.service, backup.timer
  - cron-daily.service, cron-daily.timer
- 08_swap/
  - swap-example.swap
- 09_path/
  - touch.path, touch.service
  - path-changed.path, reloader.service
- 10_slice/
  - resource.slice
- 11_scope/
  - backup.scope
- 12_networkd/
  - br0.netdev, br0.network
  - vlan10.netdev, vlan10.network
  - eth0.network
- 13_overrides/
  - hello.service.d/override.conf

---

## Systemd Unit Types and Example Files

| Unit Type   | Extension   | Example(s) in repo                |
|------------|-------------|-----------------------------------|
| Service    | .service    | 01_service/hello.service, advanced.service, forking.service, oneshot.service, notify.service |
| Socket     | .socket     | 02_socket/echo.socket, tcpecho.socket        |
| Target     | .target     | 03_target/custom.target                      |
| Device     | .device     | 04_device/sda.device                         |
| Mount      | .mount      | 05_mount/mnt-example.mount                  |
| Automount  | .automount  | 06_automount/data.automount                 |
| Timer      | .timer      | 07_timer/backup.timer, cron-daily.timer     |
| Swap       | .swap       | 08_swap/swap-example.swap                  |
| Path       | .path       | 09_path/touch.path, path-changed.path      |
| Slice      | .slice      | 10_slice/resource.slice                     |
| Scope      | .scope      | 11_scope/backup.scope                       |

---

See `man systemd.unit` for more details on each unit type.
