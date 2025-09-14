# Example systemd unit files

Use these to try out the Systemd Configurator SPA:

## Services

- hello.service: Simple service that echoes a message
- advanced.service: Demonstrates many service options (env, limits, protections)
- forking.service: Classic daemon using PIDFile
- oneshot.service: Run a single command and exit
- notify.service: Example of Type=notify (needs sd_notify capable binary)

## Socket activation

- echo.socket + echo.service: Socket-activated echo (stdin)
- tcpecho.socket + `tcpecho@.service`: Per-connection templated service

## Timers

- backup.service + backup.timer: Nightly backup job
- cron-daily.service + cron-daily.timer: Daily oneshot

## Path units

- touch.path + touch.service: Path-triggered oneshot
- path-changed.path + reloader.service: React to file changes

## Mount/Swap/Target/Slice

- mnt-example.mount: Mount example
- swap-example.swap: Swap on /swapfile
- custom.target: Custom target
- resource.slice: Resource limits via cgroups

## Networkd

- br0.netdev + br0.network: Bridge device and config
- vlan10.netdev + vlan10.network: VLAN device and config
- eth0.network: Basic DHCP network config

## Drop-in override

- hello.service.d/override.conf: Override environment via drop-in
