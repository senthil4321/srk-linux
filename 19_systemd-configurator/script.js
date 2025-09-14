/* Systemd Configurator SPA - No server side components */
(function () {
  'use strict';

  const fileInput = document.getElementById('fileInput');
  const saveBtn = document.getElementById('saveBtn');
  const downloadBtn = document.getElementById('downloadBtn');
  const editor = document.getElementById('editor');
  const preview = document.getElementById('preview');
  const filenameEl = document.getElementById('filename');
  const sectionSelect = document.getElementById('sectionSelect');
  const directiveSelect = document.getElementById('directiveSelect');
  const insertDirectiveBtn = document.getElementById('insertDirectiveBtn');

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
