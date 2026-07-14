"""Склеивает кадры Output/Frames/frame_*.ppm в анимированный gif и открывает его.

Запуск (из папки MiniDriver, после того как отработал MiniDriver.exe):
    py make_gif.py

Нужен Pillow:  py -m pip install Pillow
"""
import glob
import os
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
FRAMES_DIR = os.path.join(HERE, "Output", "Frames")
OUT_PATH = os.path.join(HERE, "Output", "animation.gif")
DURATION_MS = 500   # длительность одного кадра; меньше = быстрее

files = sorted(glob.glob(os.path.join(FRAMES_DIR, "frame_*.ppm")))
if not files:
    raise SystemExit(f"Нет кадров в {FRAMES_DIR} — сначала запусти MiniDriver.exe")

frames = [Image.open(f).convert("RGB") for f in files]
frames[0].save(
    OUT_PATH,
    save_all=True,
    append_images=frames[1:],
    duration=DURATION_MS,
    loop=0,           # крутить бесконечно
    optimize=True,
    disposal=2,       # чистить фон перед каждым кадром (без смаза)
)
print(f"Готово: {OUT_PATH}  ({len(frames)} кадров)")
os.startfile(OUT_PATH)   # открыть в приложении по умолчанию (Windows)
