# Service Examples (01_service)

This directory contains example systemd service units, including a template service (`myapp@.service`) you can instantiate per environment/customer/site.

## Template service: myapp@.service

- Template unit filename ends with `@.service` and uses the instance name in place of `%i`.
- Create instances like `myapp@alpha.service`, `myapp@beta.service`.
- `%i` expands to the instance identifier (e.g. `alpha`).

### Files used

- `myapp@.service` — Template. Uses `%i` in Description, Environment, EnvironmentFile, and ExecStart.
- Optional per-instance config: `/etc/myapp/<instance>.conf` (e.g., `/etc/myapp/alpha.conf`).

### Usage (conditional.service)

1. Copy or symlink `myapp@.service` to `/etc/systemd/system/`.
2. Create per-instance configuration files if needed: `/etc/myapp/<instance>.conf`.
3. Enable and start an instance:

   ```bash
   sudo systemctl enable --now myapp@alpha.service
   ```

4. Check status and logs:

   ```bash
   systemctl status myapp@alpha.service
   journalctl -u myapp@alpha.service -n 50 -f
   ```

### Notes

- `%i` is the raw instance string; `%I` is the escaped variant if your instance contains `/`.
- Use `EnvironmentFile=-/etc/myapp/%i.conf` to make the config optional (`-` ignores missing file).
- You can manage multiple instances independently (start/stop/restart each).

## Conditional service example

`conditional.service` demonstrates gating a service with a unit condition:

- `ConditionPathExists=/etc/myapp/feature.flag` — if the file is missing, the unit is skipped without error.
- This is a gate, not a trigger; it controls whether the unit runs when started/required.

### Usage

1. Copy `conditional.service` to `/etc/systemd/system/`.
2. Create the flag file (optional):

   ```bash
   sudo mkdir -p /etc/myapp
   echo enabled | sudo tee /etc/myapp/feature.flag
   ```

3. Start the service:

   ```bash
   sudo systemctl start conditional.service
   ```

4. Check logs/status:

   ```bash
   systemctl status conditional.service
   journalctl -u conditional.service -n 50 -f
   ```

### Autostart on file appearance (optional)

Use the path unit in `../09_path/conditional.path` to start the service when the flag file appears:

```bash
sudo cp ../09_path/conditional.path /etc/systemd/system/
sudo systemctl enable --now conditional.path
```
