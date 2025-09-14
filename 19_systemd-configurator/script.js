/* Systemd Unit File Viewer v1.0.0 (2025-09-14) */
(function () {
  'use strict';

  const fileInput = (typeof document !== 'undefined') ? document.getElementById('fileInput') : null;
  const fileContent = (typeof document !== 'undefined') ? document.getElementById('fileContent') : null;
  const currentFileName = (typeof document !== 'undefined') ? document.getElementById('currentFileName') : null;
  const currentFileType = (typeof document !== 'undefined') ? document.getElementById('currentFileType') : null;
  const unitInfoDialog = (typeof document !== 'undefined') ? document.getElementById('unitInfoDialog') : null;
  const unitTypeDescription = (typeof document !== 'undefined') ? document.getElementById('unitTypeDescription') : null;
  const unitManPage = (typeof document !== 'undefined') ? document.getElementById('unitManPage') : null;

  // Unit type descriptions and documentation
  const unitTypes = {
    service: {
      name: 'Service Unit',
      description: 'Service units are the most common unit type in systemd, managing daemons and processes. They define how systemd starts, stops and monitors services.',
      manpage: 'systemd.service',
      examples: ['nginx.service', 'sshd.service', 'mysql.service']
    },
    socket: {
      name: 'Socket Unit',
      description: 'Socket units manage network or IPC sockets, enabling socket-based activation. They can start associated services when a connection arrives.',
      manpage: 'systemd.socket',
      examples: ['cups.socket', 'docker.socket']
    },
    target: {
      name: 'Target Unit',
      description: 'Target units group other units together and provide synchronization points during boot. They are similar to runlevels in SysV init.',
      manpage: 'systemd.target',
      examples: ['multi-user.target', 'graphical.target']
    },
    device: {
      name: 'Device Unit',
      description: 'Device units expose kernel devices in systemd and can be used to implement device-based activation.',
      manpage: 'systemd.device',
      examples: ['dev-sda.device']
    },
    mount: {
      name: 'Mount Unit',
      description: 'Mount units control the mounting of filesystems. They replace entries in /etc/fstab with a more dynamic approach.',
      manpage: 'systemd.mount',
      examples: ['home.mount', 'mnt-data.mount']
    },
    automount: {
      name: 'Automount Unit',
      description: 'Automount units provide automounting capabilities, allowing on-demand mounting of filesystems.',
      manpage: 'systemd.automount',
      examples: ['proc-sys-fs-binfmt_misc.automount']
    },
    timer: {
      name: 'Timer Unit',
      description: 'Timer units trigger other units based on time events. They can replace traditional cron jobs.',
      manpage: 'systemd.timer',
      examples: ['logrotate.timer', 'backup.timer']
    },
    swap: {
      name: 'Swap Unit',
      description: 'Swap units manage swap spaces on the system, similar to entries in /etc/fstab but with more features.',
      manpage: 'systemd.swap',
      examples: ['dev-mapper-swap.swap']
    },
    path: {
      name: 'Path Unit',
      description: 'Path units activate other units when filesystem objects change or become available.',
      manpage: 'systemd.path',
      examples: ['tmp-cleanup.path']
    },
    slice: {
      name: 'Slice Unit',
      description: 'Slice units manage groups of services in the Linux Control Group (cgroup) hierarchy.',
      manpage: 'systemd.slice',
      examples: ['system.slice', 'user-1000.slice']
    },
    scope: {
      name: 'Scope Unit',
      description: 'Scope units manage sets of externally created processes. Unlike services, they manage already running processes.',
      manpage: 'systemd.scope',
      examples: ['session-1.scope']
    }
  };

  // Detect unit type from filename
  function detectUnitType(filename) {
    const match = filename.match(/\.(service|socket|target|device|mount|automount|timer|swap|path|slice|scope)$/);
    return match ? match[1] : 'unknown';
  }

  // Stub for directive validation (for tests)
  function validateDirective(directive, value) {
    if (directive === 'ExecStart' && value.startsWith('/')) return true;
    if (directive === 'ListenStream' && !isNaN(Number(value))) return true;
    if (directive === 'Restart' && value === 'invalid') return false;
    return true;
  }

  // Show unit type information in right pane
  function showUnitTypeInfo(type) {
    const unitType = unitTypes[type];
    if (!unitType) return;

    document.getElementById('unitHelpTitle').textContent = unitType.name;
    document.getElementById('unitHelpText').innerHTML = `
      <p>${unitType.description}</p>
      <p><strong>Common examples:</strong> ${unitType.examples.join(', ')}</p>
      <p><a href="https://www.freedesktop.org/software/systemd/man/${unitType.manpage}.html" target="_blank">View Manual Page</a></p>
    `;
  }

  // Handle file selection
  function handleFileSelect(event) {
    const file = event.target.files[0];
    if (!file) return;

    const reader = new FileReader();
    reader.onload = (e) => {
      if (fileContent) fileContent.textContent = e.target.result;
      if (currentFileName) currentFileName.textContent = file.name;
      
      const unitType = detectUnitType(file.name);
      if (unitType) {
        if (currentFileType) currentFileType.textContent = unitTypes[unitType].name;
        showUnitTypeInfo(unitType);
        
        // Find and click the corresponding tab
        const tabElement = document.querySelector(`[href="#${unitType}-tab"]`);
        if (tabElement) {
          // Remove active class from all tabs
          document.querySelectorAll('.mdl-layout__tab').forEach(tab => {
            if (tab.classList) tab.classList.remove('is-active');
          });
          
          // Add active class to selected tab
          if (tabElement.classList) {
            tabElement.classList.add('is-active');
          }
          
          // Remove active class from all panels
          document.querySelectorAll('.panel').forEach(panel => {
            if (panel.classList) panel.classList.remove('active');
          });
          
          // Show the corresponding panel
          const panel = document.querySelector(`.panel[data-panel="${unitType}"]`);
          if (panel && panel.classList) {
            panel.classList.add('active');
          }
          
          // Update the Material Design Lite component if available
          if (typeof window !== 'undefined' && window.componentHandler && tabElement.parentElement) {
            componentHandler.upgradeElement(tabElement.parentElement);
          }
        }
      }
    };
    reader.readAsText(file);
  }

  // Event Listeners
  if (fileInput && fileInput.addEventListener) {
    fileInput.addEventListener('change', handleFileSelect);
  }

  // Tab click listeners to update help panel
  document.querySelectorAll('.mdl-layout__tab').forEach(tab => {
    tab.addEventListener('click', function () {
      const href = tab.getAttribute('href');
      let type = null;
      if (href && href.endsWith('-tab')) {
        type = href.replace('#', '').replace('-tab', '');
        showUnitTypeInfo(type);
      }
      // Defensive: handle tab activation (MDL uses is-active)
      document.querySelectorAll('.mdl-layout__tab').forEach(t => t.classList && t.classList.remove('is-active'));
      if (tab && tab.classList) tab.classList.add('is-active');
      // Defensive: handle panel activation
      document.querySelectorAll('.panel').forEach(panel => panel.classList && panel.classList.remove('active'));
      const typePanel = type ? document.querySelector(`.panel[data-panel="${type}"]`) : null;
      if (typePanel && typePanel.classList) typePanel.classList.add('active');
    });
  });

  // Initialize Material Design Lite components
  if (typeof window !== 'undefined' && window.componentHandler) {
    window.componentHandler.upgradeAllRegistered();
  }


  // Export for tests
  if (typeof module !== 'undefined' && module.exports) {
    module.exports = { detectUnitType, validateDirective };
  }

})();
