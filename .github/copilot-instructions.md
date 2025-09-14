# Copilot Instructions for systemd-configurator

## Project Overview
Single-page application (SPA) for viewing and editing systemd unit files, built with vanilla JavaScript and Material Design Lite.

## Architecture

### Core Components
- `script.js`: Main application logic
  - Unit type detection
  - File handling
  - Help panel updates
  - Tab navigation
- `index.html`: SPA structure with Material Design components
- `styles.css`: Custom styling for split layout and panels

### Key Patterns
1. Unit Type Handling:
```javascript
function detectUnitType(filename) {
  const match = filename.match(/\.(service|socket|target|device|mount|automount|timer|swap|path|slice|scope)$/);
  return match ? match[1] : 'unknown';
}
```

2. DOM Updates with Null Checks:
```javascript
if (element && element.classList) {
  element.classList.add('active');
}
```

## Development Workflow

### Testing
#### Local Testing with WSL
1. Install Node.js and npm in WSL:
   ```bash
   sudo apt update
   sudo apt install nodejs npm -y
   ```
2. Navigate to project directory in WSL:
   ```bash
   cd /mnt/c/Users/senth/Documents/projects/srk-linux/19_systemd-configurator
   ```
3. Install dependencies and run tests:
   ```bash
   npm install
   npm test
   ```
4. For coverage report:
   ```bash
   npm test -- --coverage
   ```

#### Test Structure
- Tests are in `tests/` directory:
  - `core.test.js`: Unit type detection, directive validation
  - `dom.test.js`: Tab navigation, panel interactions

### GitHub Actions
- Automated testing on push/PR to main
- Tests run against 20.x
- Coverage reports uploaded as artifacts

## Project Conventions

### Directory Structure
```
19_systemd-configurator/
├── examples/           # Example unit files by type
├── tests/             # Test files and setup
├── index.html         # Main SPA
├── script.js          # Core logic
└── styles.css         # Custom styling
```

### File Naming
- Example files follow numbered prefix: `01_service/`, `02_socket/`, etc.
- Unit files use descriptive names: `hello.service`, `echo.socket`

### Code Style
- Use defensive programming with null checks for DOM operations
- Document complex functions with comments
- Handle all unit type variants explicitly

## Integration Points
- Material Design Lite (MDL) for UI components
- GitHub Actions for CI/CD
- Jest + jsdom for testing

## Common Tasks
1. Adding new unit type support:
   - Update `unitTypes` object in `script.js`
   - Add corresponding tab and panel in `index.html`
   - Add example files in `examples/` directory
   
2. Updating tests:
   - Add unit tests to `core.test.js` for type detection
   - Add DOM tests to `dom.test.js` for UI interactions
   
3. Deploying changes:
   - Run tests locally: `npm test`
   - Push to main branch
   - Monitor GitHub Actions for test results