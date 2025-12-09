# AES Side-Channel Attack Demo using perf events

This directory demonstrates cache timing side-channel attacks for AES key extraction using Linux perf events on Raspberry Pi.

## Overview

This demo shows how performance counters can leak cryptographic keys through cache timing measurements during AES encryption operations.

## Components

- `aes_victim.c` - Victim process performing AES encryption
- `perf_spy.c` - Attacker process using perf events to monitor cache activity
- `key_extractor.c` - Automated key recovery using cache timing measurements
- `Makefile` - Build configuration

## Requirements

- Raspberry Pi (tested on Pi 4)
- Linux kernel with perf events support
- Root access for perf monitoring
- OpenSSL development libraries

## Build

```bash
make
```

## Usage

### Terminal 1 - Run victim (performs AES encryption)

```bash
sudo ./aes_victim
```

### Terminal 2 - Run spy (monitors cache events)

```bash
sudo ./perf_spy $(pgrep aes_victim)
```

### Automated Key Recovery

```bash
sudo ./key_extractor
```

This program attempts to recover the first byte of the AES key by measuring cache misses for each possible key value (0x00-0xFF).

## What it demonstrates

1. **Cache timing**: AES table lookups create measurable cache access patterns
2. **Perf events**: Kernel perf subsystem exposes cache miss/hit counters
3. **Side-channel leakage**: Cache behavior correlates with secret key bits
4. **Key recovery**: Statistical analysis of timing data can recover cryptographic keys

## Security implications

- Even isolated processes can leak secrets via shared CPU resources
- Hardware performance counters provide high-resolution timing
- Demonstrates why constant-time crypto implementations are critical

## References

- [Flush+Reload: a High Resolution, Low Noise, L3 Cache Side-Channel Attack](https://eprint.iacr.org/2013/448.pdf)
- [Prime+Probe: A Novel Side-Channel Attack on ARM Caches](https://eprint.iacr.org/2016/702.pdf)
