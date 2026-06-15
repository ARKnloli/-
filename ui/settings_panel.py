from PyQt6.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QLabel, QComboBox,
    QPushButton, QProgressBar, QCheckBox
)
from PyQt6.QtCore import Qt, pyqtSignal

from database import get_settings, set_setting, get_image_total_size, cleanup_by_age, cleanup_by_size


class SettingsPanel(QWidget):
    settings_changed = pyqtSignal()

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle('设置')
        self.setFixedSize(300, 260)
        self.setWindowFlags(Qt.WindowType.Window | Qt.WindowType.WindowStaysOnTopHint)
        self.setStyleSheet('background: #FFFFFF;')
        self._build_ui()
        self._load_settings()

    def _build_ui(self):
        layout = QVBoxLayout(self)
        layout.setContentsMargins(20, 16, 20, 16)
        layout.setSpacing(16)

        title = QLabel('设置')
        title.setStyleSheet('font-size: 16px; font-weight: bold; color: #1A1A1A;')
        layout.addWidget(title)

        row1 = QHBoxLayout()
        row1.addWidget(QLabel('存储期限'))
        row1.addStretch()
        self.retention_combo = QComboBox()
        self.retention_combo.addItems(['1', '3', '5'])
        self.retention_combo.setCurrentText('3')
        self.retention_combo.setStyleSheet('padding: 4px 8px;')
        self.retention_combo.currentTextChanged.connect(self._on_retention_changed)
        row1.addWidget(self.retention_combo)
        layout.addLayout(row1)

        img_layout = QVBoxLayout()
        img_header = QHBoxLayout()
        img_header.addWidget(QLabel('图片存储空间'))
        img_header.addStretch()
        self.img_label = QLabel('0 / 500 MB')
        self.img_label.setStyleSheet('color: #999; font-size: 12px;')
        img_header.addWidget(self.img_label)
        img_layout.addLayout(img_header)
        self.img_progress = QProgressBar()
        self.img_progress.setMaximum(500)
        self.img_progress.setStyleSheet('''
            QProgressBar {
                border: 1px solid #EBEBEB;
                border-radius: 4px;
                height: 8px;
                text-align: center;
            }
            QProgressBar::chunk {
                background: #4A90D9;
                border-radius: 3px;
            }
        ''')
        img_layout.addWidget(self.img_progress)
        layout.addLayout(img_layout)

        self.autostart_checkbox = QCheckBox('开机自动启动')
        self.autostart_checkbox.setStyleSheet('color: #333;')
        self.autostart_checkbox.toggled.connect(self._on_autostart_toggled)
        layout.addWidget(self.autostart_checkbox)

        cleanup_btn = QPushButton('立即清理过期数据')
        cleanup_btn.setStyleSheet('''
            QPushButton {
                background: #F0F6FC;
                border: 1px solid #D0D8E0;
                border-radius: 6px;
                padding: 8px;
                color: #333;
                font-size: 13px;
            }
            QPushButton:hover {
                background: #E0ECF6;
            }
        ''')
        cleanup_btn.clicked.connect(self._manual_cleanup)
        layout.addWidget(cleanup_btn)

        layout.addStretch()

    def _load_settings(self):
        settings = get_settings()
        self.retention_combo.setCurrentText(settings.get('retention_days', '3'))
        self.autostart_checkbox.setChecked(self._check_autostart())
        self._update_image_info()

    def _update_image_info(self):
        total = get_image_total_size()
        mb = total // (1024 * 1024)
        settings = get_settings()
        max_mb = int(settings.get('image_max_mb', 500))
        self.img_label.setText(f'{mb} / {max_mb} MB')
        self.img_progress.setMaximum(max_mb)
        self.img_progress.setValue(min(mb, max_mb))

    def _on_retention_changed(self, value):
        set_setting('retention_days', value)
        self.settings_changed.emit()

    def _on_autostart_toggled(self, checked):
        import sys
        import os
        import winreg
        key = winreg.HKEY_CURRENT_USER
        subkey = r'Software\Microsoft\Windows\CurrentVersion\Run'
        try:
            reg = winreg.OpenKey(key, subkey, 0, winreg.KEY_SET_VALUE)
            if checked:
                exe_path = sys.executable
                script_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'main.py')
                winreg.SetValueEx(reg, 'ClipboardManager', 0, winreg.REG_SZ,
                                  f'"{exe_path}" "{script_path}"')
            else:
                try:
                    winreg.DeleteValue(reg, 'ClipboardManager')
                except FileNotFoundError:
                    pass
            winreg.CloseKey(reg)
        except Exception:
            pass

    def _check_autostart(self):
        import winreg
        try:
            key = winreg.OpenKey(
                winreg.HKEY_CURRENT_USER,
                r'Software\Microsoft\Windows\CurrentVersion\Run',
                0, winreg.KEY_READ
            )
            try:
                winreg.QueryValueEx(key, 'ClipboardManager')
                winreg.CloseKey(key)
                return True
            except FileNotFoundError:
                winreg.CloseKey(key)
                return False
        except Exception:
            return False

    def _manual_cleanup(self):
        settings = get_settings()
        retention = int(settings.get('retention_days', 3))
        max_bytes = int(settings.get('image_max_mb', 500)) * 1024 * 1024
        cleanup_by_age(retention)
        cleanup_by_size(max_bytes)
        self._update_image_info()
        self.settings_changed.emit()
