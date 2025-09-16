# Socket Activation Examples (02_socket)

This directory demonstrates systemd socket activation: listening sockets that spawn a service per connection.

## port-watcher

Listens on TCP port 9099 and spawns a per-connection service that greets and echoes input to the journal.

### Files

- `port-watcher.socket` — `ListenStream=0.0.0.0:9099`, `Accept=true`.
- `port-watcher@.service` — spawned per connection, reads from `StandardInput=socket`, writes to journal.

### Usage

1. Copy both files to `/etc/systemd/system/`.
2. Enable and start the socket:

   ```bash
   sudo systemctl enable --now port-watcher.socket
   ```

3. Connect from a client (same machine):

   ```bash
   nc 127.0.0.1 9099
   ```

4. Watch logs for spawned instances:

   ```bash
   journalctl -u 'port-watcher@*.service' -f
   ```

### Notes

- With `Accept=true`, systemd accepts the connection and spawns an instance `port-watcher@<conn-id>.service`.
- Your real server can replace the shell snippet in the service.
- For UDP, use `ListenDatagram=` and omit `Accept=true` (datagram sockets behave differently).
