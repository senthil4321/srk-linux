/* Systemd Configurator SPA v1.0.0 (2025-09-14) */
(function () {
  'use strict';

  const fileInput = document.getElementById('fileInput');
  const downloadBtn = document.getElementById('downloadBtn');
  const editor = document.getElementById('editor');
  const preview = document.getElementById('preview');
  const filenameEl = document.getElementById('filename');
  const newUnitBtn = document.getElementById('newUnitBtn');
  const newUnitDialog = document.getElementById('newUnitDialog');
  const unitTypeSelect = document.getElementById('unitTypeSelect');
  const unitTypeForm = document.getElementById('unitTypeForm');
  const manpageLink = document.getElementById('manpageLink');
  const newUnitBtn = document.getElementById('newUnitBtn');
  const newUnitDialog = document.getElementById('newUnitDialog');
  const unitTypeSelect = document.getElementById('unitTypeSelect');
  const unitTypeForm = document.getElementById('unitTypeForm');

  // Unit type templates and form fields
  const unitTypes = {
    service: {
      name: 'Service',
      extension: '.service',
      description: 'Service/Daemon Process',
      template: `[Unit]
Description={description}
After={after}
Wants={wants}

[Service]
Type={type}
ExecStart={execStart}
{additionalDirectives}

[Install]
WantedBy={target}`,
      fields: [
        { name: 'description', label: 'Description', type: 'text', required: true },
        { name: 'type', label: 'Service Type', type: 'select', options: ['simple', 'forking', 'oneshot', 'notify'], required: true },
        { name: 'execStart', label: 'ExecStart Command', type: 'text', required: true },
        { name: 'after', label: 'After Units', type: 'text', placeholder: 'network.target' },
        { name: 'wants', label: 'Wants Units', type: 'text', placeholder: 'network.target' },
        { name: 'target', label: 'Install Target', type: 'select', options: ['multi-user.target', 'graphical.target', 'network.target'] }
      ]
    },
    socket: {
      name: 'Socket',
      extension: '.socket',
      template: `[Unit]
Description={description}

[Socket]
ListenStream={listenStream}
Accept={accept}

[Install]
WantedBy={target}`,
      fields: [
        { name: 'description', label: 'Description', type: 'text', required: true },
        { name: 'listenStream', label: 'Listen Stream', type: 'text', required: true },
        { name: 'accept', label: 'Accept', type: 'select', options: ['yes', 'no'] },
        { name: 'target', label: 'Install Target', type: 'select', options: ['sockets.target'] }
      ]
    }
  };

  // Common systemd sections and directives (subset)
  const directives = {
    'Unit': [
      'Description', 'Documentation', 'Requires', 'Wants', 'Before', 'After', 'ConditionPathExists'
    ],
    'Service': [
      'Type', 'ExecStart', 'ExecStartPre', 'ExecStartPost', 'ExecStop', 'ExecReload', 'Restart', 'RestartSec', 'User', 'Group', 'WorkingDirectory', 'Environment', 'EnvironmentFile'
    ],
    'Install': [
      'WantedBy', 'RequiredBy', 'Alias'
    ],
    'Socket': ['ListenStream', 'ListenDatagram', 'Service'],
    'Timer': ['OnCalendar', 'OnUnitActiveSec', 'OnBootSec', 'Unit'],
    'Path': ['PathExists', 'PathChanged', 'Unit']
  };

  // Populate section dropdown
  function populateSections() {
    Object.keys(directives).forEach(sec => {
      const opt = document.createElement('option');
      opt.value = sec; opt.textContent = `[${sec}]`;
      sectionSelect.appendChild(opt);
    });
  }

  // Populate directive dropdown for a section
  function populateDirectives(section) {
    directiveSelect.innerHTML = '';
    const placeholder = document.createElement('option');
    placeholder.value = ''; placeholder.textContent = 'Select directive…';
    directiveSelect.appendChild(placeholder);

    if (!section) { directiveSelect.disabled = true; return; }
    directives[section].forEach(dir => {
      const opt = document.createElement('option');
      opt.value = dir; opt.textContent = dir;
      directiveSelect.appendChild(opt);
    });
    directiveSelect.disabled = false;
  }

  // Insert directive template into the editor at cursor
  function insertDirective(section, directive) {
    if (!section || !directive) return;
    const tmpl = `\n[${section}]\n${directive}=`;
    const start = editor.selectionStart;
    const end = editor.selectionEnd;
    const before = editor.value.substring(0, start);
    const after = editor.value.substring(end);
    editor.value = `${before}${tmpl}${after}`;
    editor.focus();
    editor.selectionStart = editor.selectionEnd = before.length + tmpl.length;
    updatePreview();
  }

  function updatePreview() {
    preview.textContent = editor.value;
  }

  function loadFile(file) {
    if (!file) return;
    const reader = new FileReader();
    reader.onload = () => {
      editor.value = reader.result;
      filenameEl.value = file.name || '';
      updatePreview();
    };
    reader.readAsText(file);
  }

  function saveToDisk() {
    const name = filenameEl.value.trim() || 'unit.service';
    const blob = new Blob([editor.value], { type: 'text/plain;charset=utf-8' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = name;
    document.body.appendChild(a);
    a.click();
    setTimeout(() => {
      document.body.removeChild(a);
      URL.revokeObjectURL(url);
    }, 0);
  }

  // Wire up events
  fileInput.addEventListener('change', (e) => loadFile(e.target.files[0]));
  editor.addEventListener('input', updatePreview);
  saveBtn.addEventListener('click', saveToDisk);
  downloadBtn.addEventListener('click', saveToDisk);
  sectionSelect.addEventListener('change', (e) => {
    populateDirectives(e.target.value);
    insertDirectiveBtn.disabled = !e.target.value;
  });
  directiveSelect.addEventListener('change', (e) => {
    insertDirectiveBtn.disabled = !(sectionSelect.value && e.target.value);
  });
  insertDirectiveBtn.addEventListener('click', () => insertDirective(sectionSelect.value, directiveSelect.value));

  populateSections();
  updatePreview();
})();
