
const { detectUnitType, validateDirective } = require('../script');

/**
 * Unit tests for core functionality
 */

describe('Unit Type Detection', () => {
    test('should detect service unit type from filename', () => {
        // TODO: Implement after extracting core functions
        expect(detectUnitType('my-app.service')).toBe('service');
    });

    test('should detect socket unit type from filename', () => {
        // TODO: Implement after extracting core functions
        expect(detectUnitType('my-app.socket')).toBe('socket');
    });

    test('should handle unknown file extensions', () => {
        // TODO: Implement after extracting core functions
        expect(detectUnitType('my-app.unknown')).toBe('unknown');
    });
});

describe('Directive Validation', () => {
    test('should validate ExecStart directive', () => {
        // TODO: Implement after extracting core functions
        expect(validateDirective('ExecStart', '/usr/bin/app')).toBe(true);
    });

    test('should validate socket Listen directive', () => {
        // TODO: Implement after extracting core functions
        expect(validateDirective('ListenStream', '80')).toBe(true);
    });

    test('should reject invalid directive values', () => {
        // TODO: Implement after extracting core functions
        expect(validateDirective('Restart', 'invalid')).toBe(false);
    });
});