Security Risk Assessment Library Tests CII Best Practices

## Table of Contents

- [Intro](#intro)
- [About](#about)
- [Installing and Updating](#installing-and-updating)
    - [Install & Update Script](#install--update-script)
    - [Additional Notes](#additional-notes)
    - [Installing in Docker](#installing-in-docker)
    - [Troubleshooting on Linux](#troubleshooting-on-linux)
    - [Verify Installation](#verify-installation)
- [Usage](#usage)
    - [API Reference](#api-reference)
    - [Example](#example)
- [Running Tests](#running-tests)
    - [Environment variables](#environment-variables)
- [Compatibility](#compatibility)
- [Project Support](#project-support)
- [License](#license)
- [Copyright notice](#copyright-notice)

## Intro

**Security** allows you to evaluate authentication and authorization risk via a deterministic, fail-closed security decision engine implemented in C11. It provides a robust mechanism for enforcing security policies by evaluating multiple risk signals—such as data integrity, device reputation, and historical failures—to produce a definitive access decision.

Example:

```c
#include "engine/evaluate.h"
#include "decision.h"

RiskInput input = {
    .device_id = "device_123",
    .ip_address = "192.168.1.100",
    .user_id = "user_456"
};

RiskDecision decision = evaluate_risk(&input, EXPECTED_HASH);

if (decision == RISK_ALLOW) {
    printf("Access Granted.\n");
} else if (decision == RISK_STEP_UP) {
    printf("MFA Required.\n");
} else {
    printf("Access Denied.\n");
}
```

Simple as that!

## About

**Security** is a deterministic, fail-closed security decision engine implemented in C11 for evaluating authentication and authorization risk. It is designed to be secure by default, ensuring any failure results in a safe deny state.

Key Features:
- 🛡️ **Fail-Closed Architecture**: Secure by default. If any part of the evaluation process fails or encounters an error, the system defaults to a `RISK_DENY` state, preventing unauthorized access during failure modes.
- ⚡ **Deterministic Evaluation**: Guaranteed consistent outcomes. Rules are evaluated in a strict order (Integrity → Device → Failures) to ensure that the same input always produces the same decision.
- 📝 **Immutable Audit Logging**: Cryptographically verifiable audit trails. Every decision is logged with a SHA-256 hash of the inputs, creating a tamper-evident record of all security evaluations.
- 🚀 **High Performance**: Zero dynamic memory allocation on the hot path. The engine uses stack-based allocation for request processing, eliminating heap fragmentation and memory leaks.
- 🧩 **Modular Rule Design**: Extensible architecture. New risk signals can be added as independent modules without modifying the core evaluation logic.
- ✅ **Strict Input Validation**: Comprehensive schema validation. All inputs are validated against strict schemas before processing to prevent injection attacks and malformed data issues.

## Installing and Updating

### Install & Update Script

To install or update **Security**, you should clone the repository and build the project. To do that, use the following commands:

```bash
# Clone the repository
git clone https://github.com/yourusername/risk-engine.git security-engine

# Navigate to the project directory
cd security-engine

# Build the project using Make
make
```

Running the above commands compiles the source code and produces the `risk_engine.o` object file ready for linking. The build process uses `gcc` by default but respects the `CC` environment variable.

### Additional Notes

- **C11**: The project is written in C11 standard.
- **GNU Make**: Used for orchestrating the build process.
- **GCC / Clang**: Both compilers are fully supported and tested.
- **Standard C Library**: The library depends only on the standard C library (libc), ensuring high portability.
- **SHA-256**: Includes a self-contained SHA-256 implementation for cryptographic operations.

### Installing in Docker

You can easily include **Security** in your Docker container builds. Ensure your base image has a C compiler (GCC usually) and Make installed.

```dockerfile
FROM ubuntu:latest

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    git \
    && rm -rf /var/lib/apt/lists/*

# Clone and build Security
WORKDIR /opt
RUN git clone https://github.com/yourusername/risk-engine.git security
WORKDIR /opt/security
RUN make

# The object file is now available at /opt/security/risk_engine.o
```

### Troubleshooting on Linux

If `make` fails with "command not found", ensure you have the build tools installed.

On Debian/Ubuntu:
```bash
sudo apt-get update
sudo apt-get install build-essential
```

On Fedora/RHEL:
```bash
sudo dnf groupinstall "Development Tools"
```

### Verify Installation

To verify that **Security** has been built successfully, check for the presence of the `risk_engine.o` object file in the root directory:

```bash
ls -l risk_engine.o
```

You should see an output similar to:
`-rw-r--r-- 1 user user 12345 Feb 9 12:00 risk_engine.o`

## Usage

To integrate the Risk Engine into your application, you need to include the public headers and link against the compiled object file.

### API Reference

**Risk Decisions (`decision.h`)**
The engine returns one of the following decisions:
- `RISK_ALLOW` (0): The request is safe and should be allowed.
- `RISK_STEP_UP` (1): The request shows some risk; additional authentication (e.g., MFA) is required.
- `RISK_DENY` (2): The request is high risk or invalid and must be blocked.

**Risk Errors (`error.h`)**
Function calls may return error codes if validation fails:
- `RISK_OK` (0): Operation successful.
- `RISK_ERR_INVALID_HEADER`: The input header is malformed.
- `RISK_ERR_MISSING_SIGNAL`: A required risk signal is missing.
- `RISK_ERR_DUPLICATE_SIGNAL`: A signal was provided multiple times.
- `RISK_ERR_INVALID_ORDER`: Signals are not in the expected canonical order.
- `RISK_ERR_OUT_OF_RANGE`: A value exceeds the allowed range.
- `RISK_ERR_VERSION_MISMATCH`: The input schema version does not match the engine version.

### Example

Here is a complete example of how to invoke the engine:

```c
#include <stdio.h>
#include "engine/evaluate.h"
#include "decision.h"
#include "error.h"

int main() {
    // 1. Prepare Input
    RiskInput input;
    // ... initialize input fields ...

    // 2. Expected Hash (for integrity check)
    uint32_t expected_hash = 0xAABBCCDD;

    // 3. Evaluate Risk
    RiskDecision decision = evaluate_risk(&input, expected_hash);

    // 4. Act on Decision
    switch (decision) {
        case RISK_ALLOW:
            printf("✅ Access Granted\n");
            break;
        case RISK_STEP_UP:
            printf("⚠️  Step-Up Authentication Required\n");
            break;
        case RISK_DENY:
        default:
            printf("❌ Access Denied\n");
            break;
    }

    return 0;
}
```

## Running Tests

The project includes a comprehensive suite of unit tests covering all rules and core logic. Tests are automated using Bash scripts located in the root directory.

To run all unit tests:

```bash
./run_all_unit_tests.sh
```

This script will compile and execute the following test binaries:
- `test_integrity`: Verifies the integrity rule logic.
- `test_device`: Tests device reputation scoring.
- `test_failures`: Checks failure history logic.
- `test_hash`: Validates SHA-256 implementation.
- `test_validate`: Ensures schema validation works as expected.

To clean up all build artifacts and test binaries:

```bash
make clean
```

### Environment variables

The build system and tests respect the following environment variables:

- `CC`: The C compiler to use (default: `gcc`). You can set this to `clang` to use LLVM.
- `CFLAGS`: Additional compiler flags (e.g., `-O3` for optimization or `-g` for debugging).

## Compatibility

**Security** is designed to be portable and has been tested on:

- **Linux**: GCC 4.8+, Clang 3.8+ (glibc, musl)
- **macOS**: Apple Clang (Xcode Command Line Tools)
- **Architectures**: x86_64, arm64 (Apple Silicon)

It strictly adheres to the **C11** standard and does not rely on OS-specific APIs, making it suitable for embedded environments and older systems.

## Project Support

Supported on Linux and macOS environments with GCC or Clang. We prioritize support for the latest LTS releases of Ubuntu and macOS.

## License

[MIT](LICENSE.md)

## Copyright notice

Copyright Risk Engine contributors. All rights reserved.
