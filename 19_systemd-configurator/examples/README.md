# Example systemd unit files

The examples are grouped by unit type for readability. Load any of these in the SPA via the Load button.

- 01_services/        (.service)
  - hello.service
  - advanced.service
  - forking.service
  - oneshot.service
  - notify.service
- 02_sockets/         (.socket)
  - echo.socket, echo.service
  - tcpecho.socket, `tcpecho@.service`
- 07_targets/         (.target)
  - custom.target
- 11_devices/         (.device)
  - sda.device
- 05_mounts/          (.mount)
  - mnt-example.mount
  - data.automount
- 05_mounts/          (.automount)
  - data.automount
- 03_timers/          (.timer)
  - backup.service, backup.timer
  - cron-daily.service, cron-daily.timer
- 06_swap/            (.swap)
  - swap-example.swap
- 04_paths/           (.path)
  - touch.path, touch.service
  - path-changed.path, reloader.service
- 08_slices/          (.slice)
  - resource.slice
- 12_scopes/          (.scope)
  - backup.scope
- 09_networkd/
  - br0.netdev, br0.network
  - vlan10.netdev, vlan10.network
  - eth0.network
- 10_overrides/
  - hello.service.d/override.conf
