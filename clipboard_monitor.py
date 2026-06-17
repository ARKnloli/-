import hashlib
import os
from datetime import datetime
from PyQt6.QtCore import QThread, pyqtSignal
from PyQt6.QtWidgets import QApplication

from database import add_item, get_last_text, get_last_image_hash, IMAGE_DIR


class ClipboardMonitor(QThread):
    new_item_signal = pyqtSignal(dict)

    def __init__(self):
        super().__init__()
        self._running = True
        self._last_text = None
        self._last_image_hash = None
        self._skip_next = False

    def skip_next_change(self):
        self._skip_next = True

    def run(self):
        self._last_text = get_last_text()
        self._last_image_hash = get_last_image_hash()

        while self._running:
            try:
                self._check_clipboard()
            except Exception:
                pass
            self.msleep(500)

    def _check_clipboard(self):
        clipboard = QApplication.clipboard()

        text = clipboard.text()
        if text and text != self._last_text:
            if not self._skip_next:
                self._last_text = text
                item_id = add_item('text', text)
                self.new_item_signal.emit({
                    'id': item_id,
                    'type': 'text',
                    'content': text,
                    'pinned': 0,
                    'created_at': datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
                })
            else:
                self._last_text = text
                self._skip_next = False

        image = clipboard.image()
        if not image.isNull():
            image_hash = hashlib.sha256(image.constBits().asstring(image.sizeInBytes())).hexdigest()
            if image_hash != self._last_image_hash:
                if not self._skip_next:
                    self._last_image_hash = image_hash
                    filename = f'{datetime.now().strftime("%Y%m%d%H%M%S%f")}.png'
                    filepath = os.path.join(IMAGE_DIR, filename)
                    image.save(filepath, 'PNG')
                    size = os.path.getsize(filepath)
                    item_id = add_item('image', filepath, size)
                    self.new_item_signal.emit({
                        'id': item_id,
                        'type': 'image',
                        'content': filepath,
                        'size': size,
                        'pinned': 0,
                        'created_at': datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
                    })
                else:
                    self._last_image_hash = image_hash
                    self._skip_next = False

    def stop(self):
        self._running = False
        self.wait(1000)
