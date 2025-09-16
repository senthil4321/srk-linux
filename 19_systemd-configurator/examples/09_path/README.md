# Path Units (09_path)

This directory contains examples of systemd path units that trigger services when filesystem conditions change.

## conditional.path

Starts `conditional.service` when `/etc/myapp/feature.flag` appears.

### Files

- `conditional.path` — watches for the flag file and triggers `conditional.service`.
- `../01_service/conditional.service` — the gated service that runs when started; the path unit starts it for you.

### Usage

1. Copy both files to `/etc/systemd/system/` (preserving names):

   ```bash
   sudo cp conditional.path /etc/systemd/system/
   sudo cp ../01_service/conditional.service /etc/systemd/system/
   ```

2. Enable the path unit:

   ```bash
   sudo systemctl enable --now conditional.path
   ```

3. Create the flag file to trigger the service:

   ```bash
   sudo mkdir -p /etc/myapp
   echo enabled | sudo tee /etc/myapp/feature.flag
   ```

4. Watch logs:

   ```bash
   journalctl -u conditional.service -f
   ```

### Notes

- Path units are triggers; service unit `Condition*=` directives are gates.
- To trigger on changes (not just existence), see `PathChanged=` and friends in `systemd.path(5)`.
