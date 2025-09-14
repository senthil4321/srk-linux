# Example systemd unit files

The examples are grouped by unit type for readability. Load any of these in the SPA via the Load button.

- 01_services/
  - hello.service
  - advanced.service
  - forking.service
  - oneshot.service
  - notify.service
- 02_sockets/
  - echo.socket, echo.service
  - tcpecho.socket, `tcpecho@.service`
- 03_timers/
  - backup.service, backup.timer
  - cron-daily.service, cron-daily.timer
- 04_paths/
  - touch.path, touch.service
  - path-changed.path, reloader.service
- 05_mounts/
  - mnt-example.mount
  - data.automount
- 06_swap/
  - swap-example.swap
- 07_targets/
  - custom.target
- 08_slices/
  - resource.slice
- 09_networkd/
  - br0.netdev, br0.network
  - vlan10.netdev, vlan10.network
  - eth0.network
- 10_overrides/
  - hello.service.d/override.conf
- 11_devices/
  - sda.device
- 12_scopes/
  - backup.scope
