import sqlite3
import os
import threading
from datetime import datetime, timedelta

DB_DIR = os.path.dirname(os.path.abspath(__file__))
DB_PATH = os.path.join(DB_DIR, 'clipboard.db')
IMAGE_DIR = os.path.join(DB_DIR, 'images')

_local = threading.local()


def get_conn():
    if not hasattr(_local, 'conn') or _local.conn is None:
        _local.conn = sqlite3.connect(DB_PATH)
        _local.conn.row_factory = sqlite3.Row
        _local.conn.execute('PRAGMA journal_mode=WAL')
    return _local.conn


def init_db():
    os.makedirs(IMAGE_DIR, exist_ok=True)
    conn = get_conn()
    conn.execute('''
        CREATE TABLE IF NOT EXISTS items (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            type TEXT NOT NULL CHECK(type IN ('text','image')),
            content TEXT,
            size INTEGER DEFAULT 0,
            pinned INTEGER DEFAULT 0 CHECK(pinned IN (0,1)),
            created_at TEXT NOT NULL DEFAULT (datetime('now','localtime'))
        )
    ''')
    conn.execute('''
        CREATE TABLE IF NOT EXISTS settings (
            key TEXT PRIMARY KEY,
            value TEXT NOT NULL
        )
    ''')
    conn.execute('''
        INSERT OR IGNORE INTO settings (key, value) VALUES ('retention_days', '3')
    ''')
    conn.execute('''
        INSERT OR IGNORE INTO settings (key, value) VALUES ('image_max_mb', '500')
    ''')
    conn.commit()


def add_item(item_type, content, size=0):
    conn = get_conn()
    now = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    cursor = conn.execute(
        'INSERT INTO items (type, content, size, created_at) VALUES (?, ?, ?, ?)',
        (item_type, content, size, now)
    )
    conn.commit()
    return cursor.lastrowid


def get_items(search=None, limit=1000):
    conn = get_conn()
    if search:
        rows = conn.execute('''
            SELECT * FROM items
            WHERE type = 'text' AND content LIKE ?
            ORDER BY pinned DESC, created_at DESC
            LIMIT ?
        ''', (f'%{search}%', limit)).fetchall()
    else:
        rows = conn.execute('''
            SELECT * FROM items
            ORDER BY pinned DESC, created_at DESC
            LIMIT ?
        ''', (limit,)).fetchall()
    return [dict(row) for row in rows]


def pin_item(item_id, pinned):
    conn = get_conn()
    conn.execute('UPDATE items SET pinned = ? WHERE id = ?', (1 if pinned else 0, item_id))
    conn.commit()


def delete_item(item_id):
    conn = get_conn()
    row = conn.execute('SELECT type, content FROM items WHERE id = ?', (item_id,)).fetchone()
    if row and row['type'] == 'image':
        img_path = row['content']
        if img_path and os.path.exists(img_path):
            os.remove(img_path)
    conn.execute('DELETE FROM items WHERE id = ?', (item_id,))
    conn.commit()


def get_settings():
    conn = get_conn()
    rows = conn.execute('SELECT key, value FROM settings').fetchall()
    return {row['key']: row['value'] for row in rows}


def set_setting(key, value):
    conn = get_conn()
    conn.execute('INSERT OR REPLACE INTO settings (key, value) VALUES (?, ?)', (key, str(value)))
    conn.commit()


def get_last_text():
    conn = get_conn()
    row = conn.execute(
        "SELECT content FROM items WHERE type='text' ORDER BY created_at DESC LIMIT 1"
    ).fetchone()
    return row['content'] if row else None


def get_last_image_hash():
    conn = get_conn()
    row = conn.execute(
        "SELECT content FROM items WHERE type='image' ORDER BY created_at DESC LIMIT 1"
    ).fetchone()
    return row['content'] if row else None


def cleanup_by_age(retention_days):
    cutoff = (datetime.now() - timedelta(days=retention_days)).strftime('%Y-%m-%d %H:%M:%S')
    conn = get_conn()
    rows = conn.execute(
        "SELECT id, content FROM items WHERE type='image' AND pinned=0 AND created_at < ?",
        (cutoff,)
    ).fetchall()
    for row in rows:
        if row['content'] and os.path.exists(row['content']):
            os.remove(row['content'])
    conn.execute(
        "DELETE FROM items WHERE pinned=0 AND created_at < ?",
        (cutoff,)
    )
    conn.commit()


def cleanup_by_size(max_bytes):
    conn = get_conn()
    total = conn.execute("SELECT COALESCE(SUM(size), 0) FROM items WHERE type='image'").fetchone()[0]
    if total <= max_bytes:
        return

    rows = conn.execute(
        "SELECT id, content FROM items WHERE type='image' ORDER BY pinned ASC, created_at ASC"
    ).fetchall()

    removed = 0
    for row in rows:
        if total - removed <= max_bytes:
            break
        size = conn.execute("SELECT size FROM items WHERE id=?", (row['id'],)).fetchone()[0]
        if row['content'] and os.path.exists(row['content']):
            os.remove(row['content'])
        conn.execute("DELETE FROM items WHERE id=?", (row['id'],))
        removed += size

    conn.commit()


def get_image_total_size():
    conn = get_conn()
    return conn.execute("SELECT COALESCE(SUM(size), 0) FROM items WHERE type='image'").fetchone()[0]


def close_db():
    if hasattr(_local, 'conn') and _local.conn:
        _local.conn.close()
        _local.conn = None
