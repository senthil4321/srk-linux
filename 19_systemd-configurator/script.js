/* Systemd Unit File Viewer v1.0.0 (2025-09-14) */
(function () {
  'use strict';

  const fileInput = document.getElementById('fileInput');
  const fileContent = document.getElementById('fileContent');
  const currentFileName = document.getElementById('currentFileName');
  const currentFileType = document.getElementById('currentFileType');
  const unitInfoDialog = document.getElementById('unitInfoDialog');
  const unitTypeDescription = document.getElementById('unitTypeDescription');
  const unitManPage = document.getElementById('unitManPage');

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
    return match ? match[1] : null;
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
      fileContent.textContent = e.target.result;
      currentFileName.textContent = file.name;
      
      const unitType = detectUnitType(file.name);
      if (unitType) {
        currentFileType.textContent = unitTypes[unitType].name;
        showUnitTypeInfo(unitType);
      }
    };
    reader.readAsText(file);
  }

  // Event Listeners
  fileInput.addEventListener('change', handleFileSelect);

  // Tab click listeners to update help panel
  document.querySelectorAll('.mdl-layout__tab').forEach(tab => {
    tab.addEventListener('click', function (e) {
      const href = tab.getAttribute('href');
      if (href && href.endsWith('-tab')) {
        const type = href.replace('#', '').replace('-tab', '');
        showUnitTypeInfo(type);
      }
    });
  });

  // Initialize Material Design Lite components
  if (window.componentHandler) {
    window.componentHandler.upgradeAllRegistered();
  }

})();
