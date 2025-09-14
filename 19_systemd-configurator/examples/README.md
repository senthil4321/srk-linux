# Example systemd unit files

The examples are grouped by unit type for readability. Load any of these in the SPA via the Load button.

- services/
  - hello.service
  - advanced.service
  - forking.service
  - oneshot.service
  - notify.service
- sockets/
  - echo.socket, echo.service
  - tcpecho.socket, `tcpecho@.service`
- timers/
  - backup.service, backup.timer
  - cron-daily.service, cron-daily.timer
- paths/
  - touch.path, touch.service
  - path-changed.path, reloader.service
- mounts/
  - mnt-example.mount
- swap/
  - swap-example.swap
- targets/
  - custom.target
- slices/
  - resource.slice
- networkd/
  - br0.netdev, br0.network
  - vlan10.netdev, vlan10.network
  - eth0.network
- overrides/
  - hello.service.d/override.conf
