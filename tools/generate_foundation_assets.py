#!/usr/bin/env python3
"""Deterministically author the player-foundation pixel assets using only Python stdlib."""

from __future__ import annotations

import json
import math
import struct
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "assets" / "foundation"


class Canvas:
    def __init__(self, width: int, height: int, color=(0, 0, 0, 0)):
        self.width = width
        self.height = height
        self.pixels = [list(color) for _ in range(width * height)]

    def put(self, x, y, color):
        x, y = int(round(x)), int(round(y))
        if 0 <= x < self.width and 0 <= y < self.height:
            self.pixels[y * self.width + x] = list(color)

    def get(self, x, y):
        if 0 <= x < self.width and 0 <= y < self.height:
            return tuple(self.pixels[y * self.width + x])
        return (0, 0, 0, 0)

    def rect(self, x, y, w, h, color):
        for py in range(max(0, int(y)), min(self.height, int(y + h))):
            for px in range(max(0, int(x)), min(self.width, int(x + w))):
                self.put(px, py, color)

    def line(self, x0, y0, x1, y1, color, width=1):
        dx, dy = x1 - x0, y1 - y0
        steps = max(1, int(max(abs(dx), abs(dy))))
        for i in range(steps + 1):
            t = i / steps
            x, y = x0 + dx * t, y0 + dy * t
            radius = max(0, width // 2)
            for oy in range(-radius, radius + 1):
                for ox in range(-radius, radius + 1):
                    self.put(x + ox, y + oy, color)

    def ellipse(self, cx, cy, rx, ry, color):
        for y in range(int(cy - ry), int(cy + ry) + 1):
            for x in range(int(cx - rx), int(cx + rx) + 1):
                if ((x - cx) / max(rx, 0.01)) ** 2 + ((y - cy) / max(ry, 0.01)) ** 2 <= 1:
                    self.put(x, y, color)

    def polygon(self, points, color):
        min_y = max(0, int(math.floor(min(p[1] for p in points))))
        max_y = min(self.height - 1, int(math.ceil(max(p[1] for p in points))))
        for y in range(min_y, max_y + 1):
            scan_y = y + 0.5
            crossings = []
            for i, a in enumerate(points):
                b = points[(i + 1) % len(points)]
                if (a[1] <= scan_y < b[1]) or (b[1] <= scan_y < a[1]):
                    crossings.append(a[0] + (scan_y - a[1]) * (b[0] - a[0]) / (b[1] - a[1]))
            crossings.sort()
            for i in range(0, len(crossings) - 1, 2):
                for x in range(int(math.ceil(crossings[i])), int(math.floor(crossings[i + 1])) + 1):
                    self.put(x, y, color)

    def blit(self, source, x, y):
        for sy in range(source.height):
            for sx in range(source.width):
                color = source.get(sx, sy)
                if color[3]:
                    self.put(x + sx, y + sy, color)

    def upscale(self, factor):
        out = Canvas(self.width * factor, self.height * factor)
        for y in range(self.height):
            for x in range(self.width):
                color = self.get(x, y)
                out.rect(x * factor, y * factor, factor, factor, color)
        return out

    def save_png(self, path):
        path.parent.mkdir(parents=True, exist_ok=True)
        raw = bytearray()
        for y in range(self.height):
            raw.append(0)
            for x in range(self.width):
                raw.extend(self.get(x, y))

        def chunk(kind, data):
            payload = kind + data
            return struct.pack(">I", len(data)) + payload + struct.pack(">I", zlib.crc32(payload) & 0xFFFFFFFF)

        png = b"\x89PNG\r\n\x1a\n"
        png += chunk(b"IHDR", struct.pack(">IIBBBBB", self.width, self.height, 8, 6, 0, 0, 0))
        png += chunk(b"IDAT", zlib.compress(bytes(raw), 9))
        png += chunk(b"IEND", b"")
        path.write_bytes(png)


INK = (10, 12, 18, 255)
SUIT = (30, 33, 42, 255)
SUIT_LIT = (59, 64, 77, 255)
SKIN = (181, 172, 166, 255)
SKIN_LIGHT = (224, 210, 198, 255)
PLATE = (82, 88, 99, 255)
CYAN = (35, 226, 239, 255)
CYAN_WHITE = (202, 255, 250, 255)
CORAL = (245, 82, 116, 255)
HAIR = (95, 34, 83, 255)
HAIR_LIT = (222, 61, 151, 255)


def segment(c, a, b, thickness, base, light):
    c.line(a[0], a[1], b[0], b[1], INK, thickness + 2)
    c.line(a[0], a[1], b[0], b[1], base, thickness)
    c.line(a[0], a[1] - 1, b[0], b[1] - 1, light, max(1, thickness - 2))


def draw_irys(frame, pose):
    c = Canvas(48, 48)
    lean = pose.get("lean", 0)
    bob = pose.get("bob", 0)
    hip = (24 + lean * 0.25, 29 + bob)
    chest = (24 + lean, 20 + bob)
    head = (24 + lean * 1.25, 12 + bob)
    stride = pose.get("stride", 0)
    arm = pose.get("arm", -stride)
    crouch = pose.get("crouch", 0)
    hip = (hip[0], hip[1] + crouch)
    chest = (chest[0], chest[1] + crouch * 0.35)
    head = (head[0], head[1] + crouch * 0.2)

    back_knee = (21 - stride * 2.2, 36 + crouch * 0.5)
    back_foot = (18 - stride * 4.0, 44)
    front_knee = (27 + stride * 2.4, 36 + crouch * 0.5)
    front_foot = (30 + stride * 4.0, 44)
    if pose.get("air"):
        lift = pose.get("lift", 0)
        back_knee = (20 - stride, 34 + lift)
        back_foot = (19 - stride * 2, 39 + lift)
        front_knee = (28 + stride, 34 - lift)
        front_foot = (30 + stride * 2, 39 - lift)
    if pose.get("brake"):
        back_foot, front_foot = (16, 44), (31, 44)
        back_knee, front_knee = (20, 35), (27, 36)

    # Rear limbs establish silhouette depth.
    segment(c, hip, back_knee, 4, SUIT, SUIT_LIT)
    segment(c, back_knee, back_foot, 3, PLATE, SUIT_LIT)
    c.polygon([(back_foot[0] - 2, back_foot[1] - 1), (back_foot[0] + 5, back_foot[1] - 1),
               (back_foot[0] + 4, back_foot[1] + 1), (back_foot[0] - 2, back_foot[1] + 1)], INK)
    c.line(hip[0] - 2, hip[1] - 1, chest[0] - 4, chest[1] + 1, SUIT_LIT, 4)

    shoulder = (chest[0] - 1, chest[1] - 1)
    rear_elbow = (20 - arm * 2.5 + lean, 26 + bob)
    rear_hand = (18 - arm * 4.0 + lean, 30 + bob)
    segment(c, shoulder, rear_elbow, 3, SUIT, PLATE)
    segment(c, rear_elbow, rear_hand, 2, SKIN, SKIN_LIGHT)

    # Torso is an engineered athletic plate assembly with a signature collar.
    c.polygon([(chest[0] - 5, chest[1] - 4), (chest[0] + 5, chest[1] - 4),
               (hip[0] + 4, hip[1]), (hip[0] - 4, hip[1])], INK)
    c.polygon([(chest[0] - 4, chest[1] - 3), (chest[0] + 4, chest[1] - 3),
               (hip[0] + 3, hip[1] - 1), (hip[0] - 3, hip[1] - 1)], SUIT)
    c.polygon([(chest[0] - 4, chest[1] - 3), (chest[0], chest[1] - 1),
               (chest[0] + 4, chest[1] - 3), (chest[0] + 2, chest[1] + 1),
               (chest[0], chest[1] + 3), (chest[0] - 2, chest[1] + 1)], PLATE)
    c.line(chest[0], chest[1], hip[0], hip[1] - 2, CORAL, 1)
    c.rect(hip[0] - 4, hip[1] - 2, 8, 3, PLATE)
    c.put(hip[0] + 3, hip[1] - 1, CYAN)

    segment(c, hip, front_knee, 4, SUIT, SUIT_LIT)
    segment(c, front_knee, front_foot, 3, PLATE, SKIN_LIGHT)
    c.polygon([(front_foot[0] - 2, front_foot[1] - 1), (front_foot[0] + 6, front_foot[1] - 1),
               (front_foot[0] + 5, front_foot[1] + 1), (front_foot[0] - 2, front_foot[1] + 1)], INK)
    c.line(front_knee[0] - 1, front_knee[1], front_foot[0], front_foot[1] - 2, CYAN, 1)

    front_elbow = (28 + arm * 2.0 + lean, 25 + bob)
    front_hand = (31 + arm * 3.0 + lean, 29 + bob)
    segment(c, (chest[0] + 2, chest[1] - 1), front_elbow, 3, SUIT, PLATE)
    segment(c, front_elbow, front_hand, 2, SKIN, SKIN_LIGHT)

    # Adult realistic head, artificial eye, temple port, and asymmetrical synthetic hair.
    c.ellipse(head[0], head[1], 4.5, 5.5, INK)
    c.polygon([(head[0] - 3, head[1] - 4), (head[0] + 3, head[1] - 3),
               (head[0] + 4, head[1] + 1), (head[0] + 2, head[1] + 4),
               (head[0] - 2, head[1] + 4), (head[0] - 4, head[1])], SKIN)
    c.polygon([(head[0] - 5, head[1] - 4), (head[0] + 3, head[1] - 5),
               (head[0] + 4, head[1] - 2), (head[0] - 1, head[1] - 2),
               (head[0] - 3, head[1] + 5), (head[0] - 6, head[1] + 2)], HAIR)
    c.line(head[0] - 4, head[1] - 3, head[0] + 2, head[1] - 4, HAIR_LIT, 1)
    c.put(head[0] + 3, head[1], CYAN_WHITE)
    c.put(head[0] + 4, head[1], CYAN)
    c.line(head[0] + 1, head[1] + 2, head[0] - 1, head[1] + 4, SUIT_LIT, 1)
    c.rect(head[0] - 5, head[1], 2, 3, PLATE)
    c.put(head[0] - 5, head[1] + 1, CORAL)

    # Ion: narrow single edge, offset spine, luminous channel, asymmetrical hooked pommel.
    sword_angle = pose.get("sword", -0.35)
    sx, sy = front_hand
    length = 15
    ex = sx + math.cos(sword_angle) * length
    ey = sy + math.sin(sword_angle) * length
    nx, ny = -math.sin(sword_angle), math.cos(sword_angle)
    c.line(sx - math.cos(sword_angle) * 2, sy - math.sin(sword_angle) * 2, ex, ey, INK, 5)
    c.line(sx, sy, ex, ey, (105, 116, 130, 255), 3)
    c.line(sx + nx, sy + ny, ex - math.cos(sword_angle) * 2 + nx, ey - math.sin(sword_angle) * 2 + ny, CYAN, 1)
    c.put(ex, ey, CYAN_WHITE)
    c.line(sx - nx * 2, sy - ny * 2, sx + nx, sy + ny, INK, 2)
    c.line(sx - math.cos(sword_angle) * 3, sy - math.sin(sword_angle) * 3,
           sx - math.cos(sword_angle) * 5 + nx * 2, sy - math.sin(sword_angle) * 5 + ny * 2, CORAL, 2)
    return c


def animation_frames():
    frames = []
    states = {}

    def add(name, poses, fps, loop):
        states[name] = {"first": len(frames), "count": len(poses), "fps": fps, "loop": loop}
        frames.extend(poses)

    add("idle", [dict(bob=b, sword=-0.28 + b * 0.03) for b in (0, -0.35, 0, 0.3)], 6, True)
    add("acceleration", [dict(lean=2, stride=0.4, arm=-0.5, sword=-0.55),
                         dict(lean=3, stride=0.8, arm=-0.8, sword=-0.7)], 12, False)
    add("run", [dict(lean=2, stride=math.sin(i * math.pi / 3), arm=-math.sin(i * math.pi / 3),
                          bob=-abs(math.sin(i * math.pi / 3)) * 0.7, sword=-0.55)
                for i in range(6)], 14, True)
    add("braking", [dict(lean=-2, brake=True, crouch=1, sword=0.05),
                    dict(lean=-1, brake=True, crouch=2, sword=-0.1)], 10, False)
    add("turning", [dict(lean=-2, brake=True, crouch=2, sword=0.7),
                    dict(lean=1, stride=-0.35, crouch=1, sword=-1.0)], 14, False)
    add("jump_launch", [dict(crouch=3, brake=True, sword=-0.9),
                        dict(air=True, lift=1, lean=1, sword=-1.05)], 14, False)
    add("rising", [dict(air=True, lift=-1, stride=0.25, lean=1, sword=-1.15),
                   dict(air=True, lift=0, stride=-0.2, lean=1, sword=-0.95)], 9, True)
    add("apex", [dict(air=True, lift=2, stride=0.45, crouch=1, sword=-0.25)], 1, True)
    add("falling", [dict(air=True, lift=1, stride=-0.4, lean=-1, sword=0.15),
                    dict(air=True, lift=2, stride=0.25, lean=-1, sword=0.35)], 8, True)
    add("landing", [dict(crouch=2, brake=True, sword=0.25), dict(crouch=1, sword=-0.15)], 13, False)
    add("hard_landing", [dict(crouch=5, brake=True, lean=-2, sword=0.65),
                         dict(crouch=3, brake=True, lean=-1, sword=0.25)], 9, False)
    return frames, states


def generate_irys():
    poses, states = animation_frames()
    atlas = Canvas(48 * 9, 48 * 3)
    for index, pose in enumerate(poses):
        atlas.blit(draw_irys(index, pose), (index % 9) * 48, (index // 9) * 48)
    atlas.upscale(2).save_png(OUT / "irys_standard.png")
    metadata = {
        "texture": "irys_standard.png",
        "frame_size": [96, 96],
        "columns": 9,
        "frame_count": len(poses),
        "draw_origin": [48, 86],
        "states": states,
        "description": "Standard-anatomy temporary Irys; authored at 2x pixel-cluster scale."
    }
    (OUT / "irys_standard.json").write_text(json.dumps(metadata, indent=2) + "\n")


def chamfered(c, x, y, w, h, cut, fill, edge):
    points = [(x + cut, y), (x + w - cut, y), (x + w, y + cut),
              (x + w, y + h - cut), (x + w - cut, y + h),
              (x + cut, y + h), (x, y + h - cut), (x, y + cut)]
    c.polygon(points, edge)
    inner = [(px + (1 if px < x + w / 2 else -1), py + (1 if py < y + h / 2 else -1)) for px, py in points]
    c.polygon(inner, fill)


def pipe(c, points, outer, inner, width=5):
    for a, b in zip(points, points[1:]):
        c.line(*a, *b, outer, width + 2)
        c.line(*a, *b, inner, width)
    for x, y in points[1:-1]:
        c.ellipse(x, y, width // 2 + 1, width // 2 + 1, outer)
        c.ellipse(x, y, width // 2, width // 2, inner)


def draw_platform(c, x, y, w, h=8):
    # Art coordinates are half of collision coordinates.
    c.polygon([(x, y + 2), (x + 3, y), (x + w - 4, y), (x + w, y + 3),
               (x + w - 3, y + h), (x + 3, y + h)], (25, 29, 34, 255))
    c.line(x + 3, y, x + w - 4, y, (241, 179, 76, 255), 1)
    c.line(x + 5, y + 2, x + w - 6, y + 2, (83, 91, 91, 255), 1)
    for px in range(int(x + 9), int(x + w - 6), 14):
        c.line(px, y + 3, px - 4, y + h - 1, (53, 59, 62, 255), 1)
        c.put(px + 2, y + 3, (222, 82, 71, 255))


def generate_room():
    w, h = 960, 360
    bg = Canvas(w, h, (8, 11, 14, 255))
    # Subtle vertical value shift: industrial green-black into warm floor shadow.
    for y in range(h):
        t = y / h
        color = (8 + int(8 * t), 11 + int(7 * t), 14 + int(5 * t), 255)
        bg.rect(0, y, w, 1, color)

    # Vast distant ribbing and maintenance lights establish scale without noise.
    for x in range(-30, w + 100, 128):
        bg.polygon([(x, 0), (x + 34, 0), (x + 58, 320), (x + 35, 320)], (16, 22, 24, 255))
        bg.line(x + 34, 0, x + 58, 320, (31, 39, 39, 255), 2)
        for y in range(28, 300, 46):
            bg.rect(x + 38 + y // 26, y, 2, 5, (42, 87, 80, 255))

    # Deep machinery silhouettes and lit inspection bays.
    for x, y, scale in [(62, 75, 1.0), (250, 44, 1.25), (520, 70, 0.9), (780, 36, 1.3)]:
        chamfered(bg, x, y, 76 * scale, 138 * scale, 8, (19, 25, 28, 255), (39, 47, 49, 255))
        bg.ellipse(x + 38 * scale, y + 52 * scale, 22 * scale, 22 * scale, (8, 13, 16, 255))
        bg.ellipse(x + 38 * scale, y + 52 * scale, 15 * scale, 15 * scale, (36, 48, 49, 255))
        for spoke in range(8):
            a = spoke * math.pi / 4
            bg.line(x + 38 * scale, y + 52 * scale,
                    x + 38 * scale + math.cos(a) * 19 * scale,
                    y + 52 * scale + math.sin(a) * 19 * scale,
                    (65, 68, 61, 255), 2)
        bg.rect(x + 10 * scale, y + 98 * scale, 56 * scale, 3, (81, 42, 50, 255))
        bg.rect(x + 14 * scale, y + 99 * scale, 19 * scale, 1, (242, 71, 129, 255))

    pipe(bg, [(0, 35), (120, 35), (120, 115), (210, 115)], (15, 17, 19, 255), (69, 52, 48, 255), 7)
    pipe(bg, [(346, 0), (346, 86), (430, 86), (430, 145)], (12, 16, 18, 255), (48, 83, 77, 255), 6)
    pipe(bg, [(720, 0), (720, 78), (670, 78), (670, 132)], (14, 15, 18, 255), (83, 43, 64, 255), 5)

    # Midground authored structural towers, conduits, service bridges, and beacons.
    for x in (16, 180, 360, 552, 748, 908):
        bg.polygon([(x, 18), (x + 16, 12), (x + 24, 23), (x + 24, 320),
                    (x + 2, 320)], (22, 27, 29, 255))
        bg.line(x + 3, 30, x + 3, 310, (63, 55, 50, 255), 2)
        for y in range(45, 310, 34):
            bg.rect(x + 6, y, 13, 3, (48, 54, 53, 255))
            bg.put(x + 18, y + 1, (237, 84, 76, 255))

    # Machinery clusters keep every section authored and visually distinct.
    chamfered(bg, 94, 228, 74, 92, 8, (34, 38, 41, 255), (73, 65, 59, 255))
    bg.ellipse(131, 260, 25, 25, (12, 17, 19, 255))
    bg.ellipse(131, 260, 15, 15, (58, 70, 69, 255))
    bg.rect(117, 286, 28, 3, (230, 68, 126, 255))
    pipe(bg, [(460, 320), (460, 236), (505, 236), (505, 190)], (11, 15, 18, 255), (41, 104, 99, 255), 6)
    chamfered(bg, 674, 202, 68, 118, 7, (35, 31, 39, 255), (79, 59, 72, 255))
    for y in range(216, 300, 14):
        bg.rect(685, y, 45, 4, (46, 50, 52, 255))
        bg.rect(686, y + 1, (y // 7) % 18 + 8, 1, (36, 211, 214, 255))
    bg.polygon([(824, 214), (877, 214), (893, 242), (885, 320), (812, 320), (808, 246)],
               (39, 38, 38, 255))
    bg.line(819, 235, 882, 235, (239, 178, 75, 255), 2)
    for x in range(824, 883, 10):
        bg.line(x, 246, x - 7, 312, (64, 66, 62, 255), 2)

    # Collision-aligned playable layer: detailed manufactured surfaces, never flat boxes.
    bg.rect(0, 320, w, 40, (21, 24, 26, 255))
    bg.rect(0, 320, w, 2, (232, 174, 78, 255))
    bg.rect(0, 323, w, 4, (59, 67, 65, 255))
    for x in range(0, w, 24):
        bg.polygon([(x, 328), (x + 18, 328), (x + 23, 334), (x + 18, 359), (x + 2, 359)],
                   (29 + (x // 24) % 2 * 4, 32, 33, 255))
        bg.line(x + 2, 332, x + 18, 332, (65, 61, 54, 255), 1)
        bg.put(x + 4, 324, (197, 71, 66, 255))
    for x, y, width in [(150, 284, 86), (260, 255, 88), (374, 227, 71),
                        (470, 257, 95), (592, 219, 90), (714, 177, 82), (822, 215, 78)]:
        draw_platform(bg, x, y, width)
    # Right solid test plinth, aligned to x=1780 y=552 at world scale.
    chamfered(bg, 890, 276, 54, 44, 5, (36, 40, 40, 255), (85, 72, 60, 255))
    bg.line(892, 276, 942, 276, (235, 179, 78, 255), 1)
    for y in range(284, 315, 7):
        bg.line(897, y, 937, y, (54, 60, 58, 255), 1)

    # Saturated wayfinding lights are restrained and keep collision lines legible.
    for x, color in [(42, CORAL), (292, CYAN), (479, CORAL), (711, CYAN), (902, CORAL)]:
        bg.rect(x, 305, 12, 3, (13, 15, 17, 255))
        bg.rect(x + 2, 305, 8, 1, color)
    # A curated high-detail background may replace this deterministic fallback.
    # Never overwrite that committed art during routine atlas regeneration.
    background_path = OUT / "foundation_background.png"
    if not background_path.exists():
        bg.upscale(2).save_png(background_path)

    fg = Canvas(w, h)
    # Sparse overhead framing and near-camera chains; center remains readable.
    fg.polygon([(0, 0), (86, 0), (73, 16), (15, 22)], (15, 17, 19, 255))
    fg.polygon([(870, 0), (960, 0), (960, 46), (930, 36)], (17, 18, 20, 255))
    for x, length in [(36, 62), (925, 96)]:
        for y in range(8, length, 8):
            fg.ellipse(x + (y // 8) % 2, y, 2, 4, (39, 40, 39, 255))
            fg.put(x, y, (102, 78, 62, 255))
    fg.polygon([(0, 300), (13, 292), (20, 360), (0, 360)], (12, 14, 16, 235))
    fg.polygon([(948, 274), (960, 268), (960, 360), (943, 360)], (12, 14, 16, 235))
    # Collision-aligned near layer preserves exact gameplay affordances over the rich backdrop.
    for x, y, width in [(150, 284, 86), (260, 255, 88), (374, 227, 71),
                        (470, 257, 95), (592, 219, 90), (714, 177, 82), (822, 215, 78)]:
        draw_platform(fg, x, y, width)
    fg.rect(0, 320, 960, 2, (22, 25, 27, 255))
    fg.rect(0, 320, 960, 1, (240, 181, 79, 255))
    chamfered(fg, 890, 276, 54, 44, 5, (36, 40, 40, 255), (85, 72, 60, 255))
    fg.line(892, 276, 942, 276, (235, 179, 78, 255), 1)
    fg.upscale(2).save_png(OUT / "foundation_foreground.png")


def generate_tiles():
    c = Canvas(128, 128, (13, 16, 18, 255))
    # Four authored material swatches: floor, platform, wall, luminous machine face.
    for ox, oy, accent in [(0, 0, (236, 176, 75, 255)), (64, 0, CYAN),
                           (0, 64, CORAL), (64, 64, (167, 244, 132, 255))]:
        chamfered(c, ox + 3, oy + 3, 58, 58, 6, (35, 39, 40, 255), (76, 70, 62, 255))
        c.line(ox + 10, oy + 10, ox + 54, oy + 10, accent, 2)
        c.line(ox + 10, oy + 16, ox + 46, oy + 16, (69, 77, 75, 255), 1)
        for y in range(oy + 24, oy + 54, 7):
            c.line(ox + 12, y, ox + 51, y, (51, 56, 56, 255), 1)
            c.put(ox + 53, y, accent)
    c.upscale(2).save_png(OUT / "industrial_tiles.png")
    metadata = {
        "texture": "industrial_tiles.png",
        "tile_size": [128, 128],
        "tiles": {"floor": [0, 0], "platform": [1, 0], "wall": [0, 1], "machine": [1, 1]},
        "pixel_cluster_scale": 2
    }
    (OUT / "industrial_tiles.json").write_text(json.dumps(metadata, indent=2) + "\n")


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    generate_irys()
    generate_room()
    generate_tiles()
    print(f"Generated foundation assets in {OUT}")


if __name__ == "__main__":
    main()
