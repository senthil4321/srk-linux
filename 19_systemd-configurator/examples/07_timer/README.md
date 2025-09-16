# Timer Examples (07_timer)

This directory contains examples for systemd timer units.

## hello-every10s

Runs a oneshot service every 10 seconds that logs a timestamped message to the journal.

### Files

- `hello-every10s.service` — oneshot service that logs `$(date -Is) hello world`.
- `hello-every10s.timer` — triggers the service every 10 seconds.

### Usage

1. Copy both files to `/etc/systemd/system/`.
2. Enable and start the timer:

   ```bash
   sudo systemctl enable --now hello-every10s.timer
   ```

3. Watch logs:

   ```bash
   journalctl -u hello-every10s.service -f
   ```

### Notes

- Use `OnUnitActiveSec=10s` for “every N seconds after last activation”.
- `AccuracySec=1s` reduces scheduling jitter (optional).
- List active timers:

  ```bash
  systemctl list-timers --all
  ```
