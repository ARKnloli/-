import os
from PyQt6.QtWidgets import QSystemTrayIcon, QMenu
from PyQt6.QtGui import QIcon, QPixmap, QPainter, QColor
from PyQt6.QtCore import Qt

ICON_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'assets', 'icon.png')


def _ensure_icon():
    os.makedirs(os.path.dirname(ICON_PATH), exist_ok=True)
    if not os.path.exists(ICON_PATH):
        pixmap = QPixmap(32, 32)
        pixmap.fill(Qt.GlobalColor.transparent)
        painter = QPainter(pixmap)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)
        painter.setBrush(QColor('#4A90D9'))
        painter.setPen(Qt.PenStyle.NoPen)
        painter.drawRoundedRect(4, 2, 24, 28, 5, 5)
        painter.setBrush(QColor('#FFFFFF'))
        painter.drawRect(8, 9, 16, 2)
        painter.drawRect(8, 14, 12, 2)
        painter.drawRect(8, 19, 14, 2)
        painter.end()
        pixmap.save(ICON_PATH, 'PNG')
    return QIcon(ICON_PATH)


class TrayManager:
    def __init__(self, show_callback, quit_callback):
        self.icon = QSystemTrayIcon()
        self.icon.setIcon(_ensure_icon())
        self.icon.setToolTip('历史粘贴板')

        menu = QMenu()
        show_action = menu.addAction('打开主界面')
        show_action.triggered.connect(show_callback)
        menu.addSeparator()
        quit_action = menu.addAction('退出')
        quit_action.triggered.connect(quit_callback)

        self.icon.setContextMenu(menu)
        self.icon.activated.connect(self._on_activated)
        self.icon.show()

    def _on_activated(self, reason):
        if reason == QSystemTrayIcon.ActivationReason.Trigger:
            self.icon.contextMenu().actions()[0].trigger()
