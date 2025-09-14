
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

    test('should detect timer unit type', () => {
        expect(detectUnitType('cleanup.timer')).toBe('timer');
    });

    test('should detect path unit type', () => {
        expect(detectUnitType('watch.path')).toBe('path');
    });
});

describe('Directive Validation', () => {
    test('valid ExecStart path is accepted', () => {
        expect(validateDirective('ExecStart', '/usr/bin/app')).toBe(true);
    });

    test('numeric ListenStream is accepted', () => {
        expect(validateDirective('ListenStream', '80')).toBe(true);
    });

    test('invalid Restart value is rejected', () => {
        expect(validateDirective('Restart', 'invalid')).toBe(false);
    });
});