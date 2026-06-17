from PyQt6.QtWidgets import QFrame, QHBoxLayout, QVBoxLayout, QLabel, QPushButton
from PyQt6.QtGui import QPixmap, QCursor
from PyQt6.QtCore import Qt, pyqtSignal

STYLE = """
QFrame#card {
    background: #FFFFFF;
    border-radius: 8px;
    border: 1px solid #EBEBEB;
    padding: 10px;
}
QFrame#card:hover {
    border-color: #4A90D9;
}
"""


def format_time(created_at):
    from datetime import datetime
    try:
        dt = datetime.strptime(created_at, '%Y-%m-%d %H:%M:%S')
        diff = datetime.now() - dt
        if diff.days > 0:
            return f'{diff.days}天前'
        if diff.seconds >= 3600:
            return f'{diff.seconds // 3600}小时前'
        if diff.seconds >= 60:
            return f'{diff.seconds // 60}分钟前'
        return '刚刚'
    except Exception:
        return created_at


class CardWidget(QFrame):
    clicked = pyqtSignal(dict)
    pin_toggled = pyqtSignal(int, bool)
    delete_requested = pyqtSignal(int)

    def __init__(self, item):
        super().__init__()
        self.item = item
        self.setObjectName('card')
        self.setStyleSheet(STYLE)
        self.setCursor(QCursor(Qt.CursorShape.PointingHandCursor))
        self._build_ui()

    def _build_ui(self):
        main_layout = QVBoxLayout(self)
        main_layout.setContentsMargins(12, 10, 12, 10)
        main_layout.setSpacing(6)

        top_row = QHBoxLayout()
        top_row.setSpacing(8)

        if self.item['pinned']:
            pin_btn = QPushButton('📌')
            pin_btn.setToolTip('取消置顶')
        else:
            pin_btn = QPushButton('📍')
            pin_btn.setToolTip('置顶')

        pin_btn.setFixedSize(28, 28)
        pin_btn.setStyleSheet('border: none; font-size: 14px;')
        pin_btn.clicked.connect(lambda e: self._toggle_pin())
        top_row.addWidget(pin_btn)

        if self.item['type'] == 'text':
            content = self.item['content']
            display = content[:40].replace('\n', ' ') + ('...' if len(content) > 40 else '')
            content_label = QLabel(display)
            content_label.setStyleSheet('color: #333; font-size: 13px;')
            content_label.setWordWrap(True)
            top_row.addWidget(content_label, 1)
        else:
            thumbnail = QLabel()
            pixmap = QPixmap(self.item['content'])
            if not pixmap.isNull():
                scaled = pixmap.scaledToHeight(60, Qt.TransformationMode.SmoothTransformation)
                thumbnail.setPixmap(scaled)
            thumbnail.setFixedHeight(60)
            thumbnail.setStyleSheet('border-radius: 4px;')
            top_row.addWidget(thumbnail, 1)

        delete_btn = QPushButton('🗑')
        delete_btn.setFixedSize(28, 28)
        delete_btn.setStyleSheet('border: none; font-size: 14px;')
        delete_btn.setToolTip('删除')
        delete_btn.clicked.connect(lambda e: self.delete_requested.emit(self.item['id']))
        top_row.addWidget(delete_btn)

        main_layout.addLayout(top_row)

        bottom_row = QHBoxLayout()
        time_label = QLabel(format_time(self.item['created_at']))
        time_label.setStyleSheet('color: #999; font-size: 11px;')
        bottom_row.addStretch()
        bottom_row.addWidget(time_label)
        main_layout.addLayout(bottom_row)

    def _toggle_pin(self):
        new_state = not self.item['pinned']
        self.pin_toggled.emit(self.item['id'], new_state)

    def mousePressEvent(self, event):
        self.clicked.emit(self.item)
        super().mousePressEvent(event)
