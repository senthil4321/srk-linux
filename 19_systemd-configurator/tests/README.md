
# Testing Setup and Documentation

## Requirements

- Node.js (v16 or later recommended)
- npm (comes with Node.js)

## Installation

1. Install Node.js from https://nodejs.org/
2. Open terminal in the `19_systemd-configurator` directory
3. Run `npm install` to install dependencies

## Running Tests

### Option 1: Visual Studio Code Tasks

1. Open Command Palette (`Ctrl+Shift+P`), type `Tasks: Run Task`, and select it.
2. Choose `Run Jest tests` from the list.

## Test Structure

### Core Tests (`core.test.js`)

- Unit type detection from filenames
- Directive validation
- Configuration parsing

### DOM Tests (`dom.test.js`)

- Tab navigation
- Panel switching
- Help panel updates
- Form interactions

## Test Coverage

To run tests with coverage report:

```bash
npm test -- --coverage

```

## Writing New Tests

1. Create new test files in the `tests` directory
2. Follow the existing pattern of `describe` and `test` blocks
3. Use meaningful test descriptions
4. Add test cases to the appropriate test file based on functionality

## Mocking

- DOM elements are mocked using jsdom
- External functions should be mocked in the test files

## CI/CD Integration

TODO: Add CI/CD setup instructions when repository is configured for automated testing
