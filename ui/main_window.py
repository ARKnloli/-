from PyQt6.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QLineEdit, QScrollArea,
    QLabel, QPushButton, QApplication
)
from PyQt6.QtCore import Qt, pyqtSignal
from PyQt6.QtGui import QIcon

from database import get_items, pin_item, delete_item, get_image_total_size, get_settings
from ui.card_widget import CardWidget


class MainWindow(QWidget):
    item_copied = pyqtSignal(dict)

    def __init__(self):
        super().__init__()
        self.setWindowTitle('历史粘贴板')
        self.setFixedSize(420, 620)
        self._before_copy = None
        self._build_ui()
        self.refresh_list()

    def _build_ui(self):
        self.setStyleSheet('background: #F0F6FC;')
        layout = QVBoxLayout(self)
        layout.setContentsMargins(12, 10, 12, 6)
        layout.setSpacing(8)

        toolbar = QHBoxLayout()
        toolbar.setSpacing(8)

        self.search_input = QLineEdit()
        self.search_input.setPlaceholderText('搜索历史记录...')
        self.search_input.setStyleSheet('''
            QLineEdit {
                padding: 8px 12px;
                border: 1px solid #D0D8E0;
                border-radius: 6px;
                background: #FFFFFF;
                font-size: 13px;
                color: #333;
            }
            QLineEdit:focus {
                border-color: #4A90D9;
            }
        ''')
        self.search_input.textChanged.connect(self._on_search)
        toolbar.addWidget(self.search_input, 1)

        settings_btn = QPushButton('⚙')
        settings_btn.setFixedSize(36, 36)
        settings_btn.setStyleSheet('''
            QPushButton {
                border: none; font-size: 18px; background: transparent;
            }
            QPushButton:hover {
                background: rgba(74,144,217,0.1);
                border-radius: 6px;
            }
        ''')
        settings_btn.setToolTip('设置')
        settings_btn.clicked.connect(self._open_settings)
        toolbar.addWidget(settings_btn)

        layout.addLayout(toolbar)

        self.scroll_area = QScrollArea()
        self.scroll_area.setWidgetResizable(True)
        self.scroll_area.setStyleSheet('''
            QScrollArea {
                border: none;
                background: transparent;
            }
            QScrollBar:vertical {
                width: 6px;
                background: transparent;
            }
            QScrollBar::handle:vertical {
                background: #C0C8D0;
                border-radius: 3px;
            }
        ''')

        self.card_container = QWidget()
        self.card_container.setStyleSheet('background: transparent;')
        self.card_layout = QVBoxLayout(self.card_container)
        self.card_layout.setContentsMargins(0, 0, 0, 0)
        self.card_layout.setSpacing(8)
        self.card_layout.addStretch()
        self.scroll_area.setWidget(self.card_container)

        layout.addWidget(self.scroll_area, 1)

        self.status_bar = QLabel()
        self.status_bar.setStyleSheet('color: #999; font-size: 11px; padding: 4px 0;')
        layout.addWidget(self.status_bar)

    def refresh_list(self, search=None):
        for i in reversed(range(self.card_layout.count())):
            widget = self.card_layout.itemAt(i).widget()
            if widget:
                widget.deleteLater()
            else:
                self.card_layout.removeItem(self.card_layout.itemAt(i))

        items = get_items(search)

        if not items:
            empty = QLabel('📋\n还没有复制记录\n\n试试复制一段文字或图片吧')
            empty.setAlignment(Qt.AlignmentFlag.AlignCenter)
            empty.setStyleSheet('color: #999; font-size: 14px; padding: 60px 0;')
            self.card_layout.addStretch()
            self.card_layout.addWidget(empty)
            self.card_layout.addStretch()
        else:
            for item in items:
                card = CardWidget(item)
                card.clicked.connect(self._copy_item)
                card.pin_toggled.connect(self._toggle_pin)
                card.delete_requested.connect(self._delete_item)
                self.card_layout.addWidget(card)
            self.card_layout.addStretch()

        total = len(items)
        img_size = get_image_total_size()
        settings = get_settings()
        max_mb = int(settings.get('image_max_mb', 500))
        self.status_bar.setText(f'  共 {total} 条记录    图片: {img_size // (1024*1024)}MB / {max_mb}MB')

    def _on_search(self, text):
        self.refresh_list(text if text else None)

    def _copy_item(self, item):
        if self._before_copy:
            self._before_copy()
        clipboard = QApplication.clipboard()
        if item['type'] == 'text':
            clipboard.setText(item['content'])
        else:
            from PyQt6.QtGui import QPixmap
            pixmap = QPixmap(item['content'])
            if not pixmap.isNull():
                clipboard.setPixmap(pixmap)
        self.item_copied.emit(item)

    def _toggle_pin(self, item_id, pinned):
        pin_item(item_id, pinned)
        self.refresh_list(self.search_input.text() or None)

    def _delete_item(self, item_id):
        delete_item(item_id)
        self.refresh_list(self.search_input.text() or None)

    def _open_settings(self):
        from ui.settings_panel import SettingsPanel
        panel = SettingsPanel(self)
        panel.settings_changed.connect(lambda: self.refresh_list(self.search_input.text() or None))
        panel.show()
