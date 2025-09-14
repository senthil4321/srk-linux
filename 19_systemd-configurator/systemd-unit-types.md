<!-- DO NOT MODIFY THIS FILE OR ITS CONTENTS WITH COPILOT OR AUTOMATED EDITS -->
<!-- Version: 1.0 | Created: 2025-09-14 -->

# list of **systemd unit types** (from `man systemd.unit`):

---

## 🔹 Unit Types in systemd

1. **Service unit** → `.service`
   Defines a service/daemon process.

2. **Socket unit** → `.socket`
   Defines an IPC or network socket. Can trigger a `.service` when a connection arrives.

3. **Target unit** → `.target`
   Synchronization point or milestone, groups units together.

4. **Device unit** → `.device`
   Represents a device in `/dev`. Usually managed by udev.

5. **Mount unit** → `.mount`
   Defines a filesystem mount.

6. **Automount unit** → `.automount`
   On-demand mounting of a filesystem.

7. **Timer unit** → `.timer`
   Triggers activation of another unit (commonly `.service`) at a given time/interval.

8. **Swap unit** → `.swap`
   Configures swap partitions or swap files.

9. **Path unit** → `.path`
   Watches a path in the filesystem and activates a unit when conditions are met.

10. **Slice unit** → `.slice`
    Groups units into Linux cgroups for resource management.

11. **Scope unit** → `.scope`
    Represents external processes not started by systemd, but registered with it (via `systemd-run`).

---

✅ These **11 unit types** are the building blocks of systemd.

