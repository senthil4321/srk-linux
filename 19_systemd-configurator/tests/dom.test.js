/**
 * DOM interaction tests
 */

describe('Tab Navigation', () => {
    beforeEach(() => {
        document.body.innerHTML = `
        <div>
          <div class="mdl-layout__tab-bar">
            <a href="#service-tab" class="mdl-layout__tab" id="tab-service">Service</a>
            <a href="#socket-tab" class="mdl-layout__tab" id="tab-socket">Socket</a>
          </div>
          <div class="panel" data-panel="service"></div>
          <div class="panel" data-panel="socket"></div>
          <div id="unitHelpTitle"></div>
          <div id="unitHelpText"></div>
        </div>`;
        // Import script to attach listeners
        jest.isolateModules(() => {
            require('../script');
        });
    });

    test('should switch tabs when clicked', () => {
        const serviceTab = document.getElementById('tab-service');
        serviceTab.click();
        expect(serviceTab.classList.contains('is-active')).toBe(true);
    });

    test('should show correct panel when tab is clicked', () => {
        const socketTab = document.getElementById('tab-socket');
        socketTab.click();
        const activePanel = document.querySelector('.panel.active');
        expect(activePanel).not.toBeNull();
        expect(activePanel.getAttribute('data-panel')).toBe('socket');
    });
});

describe('Help Panel', () => {
    beforeEach(() => {
        document.body.innerHTML = `
        <div>
          <div id="unitHelpTitle"></div>
          <div id="unitHelpText"></div>
          <a href="#service-tab" class="mdl-layout__tab" id="tab-service">Service</a>
          <a href="#socket-tab" class="mdl-layout__tab" id="tab-socket">Socket</a>
        </div>`;
        jest.isolateModules(() => {
            require('../script');
        });
    });

    test('should update help content when tab is clicked', () => {
        const serviceTab = document.getElementById('tab-service');
        serviceTab.click();
        expect(document.getElementById('unitHelpTitle').textContent).toContain('Service');
    });
});