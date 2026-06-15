import sys
import os
import traceback
from datetime import datetime
from PyQt6.QtWidgets import QApplication
from PyQt6.QtCore import QTimer

from database import init_db, close_db, get_settings, cleanup_by_age, cleanup_by_size
from tray_manager import TrayManager
from clipboard_monitor import ClipboardMonitor
from ui.main_window import MainWindow

LOG_DIR = os.path.dirname(os.path.abspath(__file__))


def log_error(msg):
    try:
        with open(os.path.join(LOG_DIR, 'error.log'), 'a', encoding='utf-8') as f:
            f.write(f'[{datetime.now().strftime("%Y-%m-%d %H:%M:%S")}] {msg}\n')
    except Exception:
        pass


def main():
    try:
        app = QApplication(sys.argv)
        app.setApplicationName('历史粘贴板')
        app.setQuitOnLastWindowClosed(False)

        init_db()

        window = MainWindow()

        def show_window():
            window.show()
            window.raise_()
            window.activateWindow()

        def on_new_item(_item):
            try:
                search = window.search_input.text()
                window.refresh_list(search if search else None)
            except Exception:
                pass

        def quit_app():
            monitor.stop()
            close_db()
            app.quit()

        tray = TrayManager(show_window, quit_app)

        monitor = ClipboardMonitor()
        monitor.new_item_signal.connect(on_new_item)
        monitor.start()

        window._before_copy = monitor.skip_next_change

        def run_cleanup():
            try:
                settings = get_settings()
                retention = int(settings.get('retention_days', 3))
                max_bytes = int(settings.get('image_max_mb', 500)) * 1024 * 1024
                cleanup_by_age(retention)
                cleanup_by_size(max_bytes)
            except Exception:
                pass

        run_cleanup()
        cleanup_timer = QTimer()
        cleanup_timer.timeout.connect(run_cleanup)
        cleanup_timer.start(30 * 60 * 1000)

        QTimer.singleShot(100, lambda: show_window())

        sys.exit(app.exec())

    except Exception:
        log_error(traceback.format_exc())
        raise


if __name__ == '__main__':
    main()
