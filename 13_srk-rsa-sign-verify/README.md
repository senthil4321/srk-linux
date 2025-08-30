# RSA Sign and Verify Shell Scripts

## Overview
This folder contains shell scripts for RSA signing, verification, and key generation using OpenSSL. It also includes unit tests for validation.

## Files
- `rsa_sign_verify.sh`: Sign and verify files using RSA keys.
- `rsa_gen_keys.sh`: Generate RSA key pairs for testing.
- `test_rsa_sign_verify.sh`: Unit tests for sign/verify functionality.
- `test_report.txt`: Output report from running the unit tests.

## Usage
### Generate RSA Key Pair
```
bash rsa_gen_keys.sh <keysize> <private_key> <public_key>
```
Example:
```
bash rsa_gen_keys.sh 2048 private.pem public.pem
```

### Sign a File
```
bash rsa_sign_verify.sh sign <private_key> <input_file> <signature_file>
```

### Verify a Signature
```
bash rsa_sign_verify.sh verify <public_key> <input_file> <signature_file>
```

### Run Unit Tests
```
bash test_rsa_sign_verify.sh
```

## Requirements
- OpenSSL must be installed in your environment.
- Scripts are tested in WSL (Ubuntu) and should work in most Linux environments.

## Notes
- All scripts log actions with timestamps.
- Return codes follow standard shell conventions.
- Update the version number in scripts for each change.
