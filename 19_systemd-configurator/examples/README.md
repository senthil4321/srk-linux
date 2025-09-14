# Example systemd unit files

The examples are grouped by unit type for readability. Load any of these in the SPA via the Load button.

- service/
  - hello.service
  - advanced.service
  - forking.service
  - oneshot.service
  - notify.service
- socket/
  - echo.socket, echo.service
  - tcpecho.socket, `tcpecho@.service`
- target/
  - custom.target
- device/
  - sda.device
- mount/
  - mnt-example.mount
- automount/
  - data.automount
- timer/
  - backup.service, backup.timer
  - cron-daily.service, cron-daily.timer
- swap/
  - swap-example.swap
- path/
  - touch.path, touch.service
  - path-changed.path, reloader.service
- slice/
  - resource.slice
- scope/
  - backup.scope
- networkd/
  - br0.netdev, br0.network
  - vlan10.netdev, vlan10.network
  - eth0.network
- overrides/
  - hello.service.d/override.conf

---

## Systemd Unit Types and Example Files

| Unit Type   | Extension   | Example(s) in repo                |
|------------|-------------|-----------------------------------|
| Service    | .service    | hello.service, advanced.service, forking.service, oneshot.service, notify.service |
| Socket     | .socket     | echo.socket, tcpecho.socket        |
| Target     | .target     | custom.target                      |
| Device     | .device     | sda.device                         |
| Mount      | .mount      | mnt-example.mount                  |
| Automount  | .automount  | data.automount                     |
| Timer      | .timer      | backup.timer, cron-daily.timer     |
| Swap       | .swap       | swap-example.swap                  |
| Path       | .path       | touch.path, path-changed.path      |
| Slice      | .slice      | resource.slice                     |
| Scope      | .scope      | backup.scope                       |

---

See `man systemd.unit` for more details on each unit type.
