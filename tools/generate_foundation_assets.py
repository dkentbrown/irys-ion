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


INK = (7, 10, 14, 255)
GRAPHITE = (24, 29, 35, 255)
GRAPHITE_LIT = (48, 58, 67, 255)
JOINT = (76, 87, 96, 255)
PALE = (176, 188, 195, 255)
PALE_LIT = (226, 235, 238, 255)
HAIR = (207, 217, 221, 255)
HAIR_LIT = (247, 250, 250, 255)
CYAN = (28, 219, 244, 255)
CYAN_WHITE = (184, 249, 255, 255)
ORANGE = (255, 147, 31, 255)
ORANGE_HOT = (255, 207, 102, 255)


def segment(c, a, b, thickness, base, light):
    c.line(a[0], a[1], b[0], b[1], INK, thickness + 2)
    c.line(a[0], a[1], b[0], b[1], base, thickness)
    c.line(a[0], a[1] - 1, b[0], b[1] - 1, light, max(1, thickness - 2))


def draw_irys(frame, pose):
    c = Canvas(48, 48)
    lean = pose.get("lean", 0)
    bob = pose.get("bob", 0)
    hip = (23 + lean * 0.25, 29 + bob)
    chest = (23 + lean, 20 + bob)
    head = (24 + lean * 1.2, 11 + bob)
    stride = pose.get("stride", 0)
    arm = pose.get("arm", -stride)
    crouch = pose.get("crouch", 0)
    hip = (hip[0], hip[1] + crouch)
    chest = (chest[0], chest[1] + crouch * 0.35)
    head = (head[0], head[1] + crouch * 0.2)

    back_knee = (20 - stride * 2.2, 36 + crouch * 0.45)
    back_foot = (18 - stride * 4.0, 44)
    front_knee = (27 + stride * 2.3, 36 + crouch * 0.45)
    front_foot = (29 + stride * 4.0, 44)
    if pose.get("air"):
        phase = pose.get("air_phase", 0)
        if phase < 0:  # Rising: long upward line through the body and trailing feet.
            back_knee, back_foot = (20 - stride, 34), (18 - stride * 2, 40)
            front_knee, front_foot = (27 + stride, 33), (30 + stride * 2, 39)
        elif phase == 0:  # Apex: legs separate slightly without becoming a ground crouch.
            back_knee, back_foot = (19, 34), (18, 39)
            front_knee, front_foot = (28, 34), (31, 39)
        else:  # Falling: both feet reach toward the next surface.
            back_knee, back_foot = (21 - stride, 35), (20 - stride, 42)
            front_knee, front_foot = (27 + stride, 35), (29 + stride, 42)
    if pose.get("brake"):
        back_foot, front_foot = (16, 44), (31, 44)
        back_knee, front_knee = (20, 35), (27, 36)

    # Rear limbs establish depth with pale synthetic plates over graphite joints.
    segment(c, hip, back_knee, 4, GRAPHITE, GRAPHITE_LIT)
    segment(c, back_knee, back_foot, 3, JOINT, PALE)
    c.polygon([(back_foot[0] - 2, back_foot[1] - 1), (back_foot[0] + 5, back_foot[1] - 1),
               (back_foot[0] + 4, back_foot[1] + 1), (back_foot[0] - 2, back_foot[1] + 1)], INK)
    c.put(back_knee[0], back_knee[1], CYAN)

    shoulder = (chest[0] - 1, chest[1] - 1)
    rear_elbow = (20 - arm * 2.5 + lean, 26 + bob)
    rear_hand = (18 - arm * 4.0 + lean, 30 + bob)
    segment(c, shoulder, rear_elbow, 3, GRAPHITE, GRAPHITE_LIT)
    segment(c, rear_elbow, rear_hand, 2, JOINT, PALE)

    # Adult synthetic torso: narrow waist, armored rib plates, exposed engineered seams.
    c.polygon([(chest[0] - 5, chest[1] - 4), (chest[0] + 5, chest[1] - 4),
               (hip[0] + 4, hip[1]), (hip[0] - 4, hip[1])], INK)
    c.polygon([(chest[0] - 4, chest[1] - 3), (chest[0] + 4, chest[1] - 3),
               (hip[0] + 3, hip[1] - 1), (hip[0] - 3, hip[1] - 1)], GRAPHITE)
    c.polygon([(chest[0] - 4, chest[1] - 3), (chest[0], chest[1] - 1),
               (chest[0] + 4, chest[1] - 3), (chest[0] + 2, chest[1] + 1),
               (chest[0], chest[1] + 2), (chest[0] - 2, chest[1] + 1)], PALE)
    c.line(chest[0], chest[1], hip[0], hip[1] - 3, GRAPHITE_LIT, 1)
    c.rect(hip[0] - 4, hip[1] - 2, 8, 3, JOINT)
    c.line(hip[0] - 2, hip[1] - 1, hip[0] + 2, hip[1] - 1, CYAN, 1)
    c.rect(chest[0] - 1, chest[1] - 2, 3, 2, CYAN)
    c.put(chest[0] + 1, chest[1] - 2, CYAN_WHITE)

    segment(c, hip, front_knee, 4, GRAPHITE, GRAPHITE_LIT)
    segment(c, front_knee, front_foot, 3, PALE, PALE_LIT)
    c.polygon([(front_foot[0] - 2, front_foot[1] - 1), (front_foot[0] + 6, front_foot[1] - 1),
               (front_foot[0] + 5, front_foot[1] + 1), (front_foot[0] - 2, front_foot[1] + 1)], INK)
    c.line(front_knee[0] - 1, front_knee[1], front_foot[0], front_foot[1] - 2, CYAN, 1)

    front_elbow = (28 + arm * 2.0 + lean, 25 + bob)
    front_hand = (31 + arm * 3.0 + lean, 29 + bob)
    segment(c, (chest[0] + 2, chest[1] - 1), front_elbow, 3, GRAPHITE, PALE)
    segment(c, front_elbow, front_hand, 2, JOINT, PALE_LIT)
    c.put(front_elbow[0], front_elbow[1], CYAN)

    # Pale face plate and bright white bob remain stable across every movement state.
    c.ellipse(head[0], head[1], 4.5, 5.5, INK)
    c.polygon([(head[0] - 3, head[1] - 4), (head[0] + 3, head[1] - 3),
               (head[0] + 4, head[1] + 1), (head[0] + 2, head[1] + 4),
               (head[0] - 2, head[1] + 4), (head[0] - 4, head[1])], PALE)
    c.polygon([(head[0] - 5, head[1] - 4), (head[0] + 3, head[1] - 5),
               (head[0] + 4, head[1] - 2), (head[0] - 1, head[1] - 2),
               (head[0] - 3, head[1] + 5), (head[0] - 6, head[1] + 2)], HAIR)
    c.line(head[0] - 4, head[1] - 3, head[0] + 2, head[1] - 4, HAIR_LIT, 1)
    c.put(head[0] + 3, head[1], CYAN_WHITE)
    c.put(head[0] + 4, head[1], CYAN)
    c.line(head[0] + 1, head[1] + 2, head[0] - 1, head[1] + 4, GRAPHITE_LIT, 1)
    c.rect(head[0] - 5, head[1], 2, 3, JOINT)
    c.put(head[0] - 5, head[1] + 1, CYAN)

    # Ion is always visibly gripped: one constant blade, dark shell, electric-blue channel.
    sword_angle = pose.get("sword", -0.35)
    sx, sy = front_hand
    length = 17
    ex = sx + math.cos(sword_angle) * length
    ey = sy + math.sin(sword_angle) * length
    nx, ny = -math.sin(sword_angle), math.cos(sword_angle)
    c.line(sx - math.cos(sword_angle) * 2, sy - math.sin(sword_angle) * 2, ex, ey, INK, 5)
    c.line(sx, sy, ex, ey, JOINT, 3)
    c.line(sx + nx, sy + ny, ex - math.cos(sword_angle) * 2 + nx, ey - math.sin(sword_angle) * 2 + ny, CYAN, 1)
    c.put(ex, ey, CYAN_WHITE)
    c.line(sx - nx * 2, sy - ny * 2, sx + nx, sy + ny, INK, 2)
    c.line(sx - math.cos(sword_angle) * 3, sy - math.sin(sword_angle) * 3,
           sx - math.cos(sword_angle) * 5 + nx * 2, sy - math.sin(sword_angle) * 5 + ny * 2, CYAN, 2)
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
                        dict(air=True, air_phase=-1, stride=0.15, lean=1, sword=-1.05)], 14, False)
    add("rising", [dict(air=True, air_phase=-1, stride=0.25, lean=1, sword=-1.15),
                   dict(air=True, air_phase=-1, stride=-0.2, lean=1, sword=-0.95)], 9, True)
    add("apex", [dict(air=True, air_phase=0, stride=0.4, sword=-0.25)], 1, True)
    add("falling", [dict(air=True, air_phase=1, stride=-0.35, lean=-1, sword=0.15),
                    dict(air=True, air_phase=1, stride=0.25, lean=-1, sword=0.35)], 8, True)
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
        "description": "White-haired synthetic Irys holding Ion; authored at 2x pixel-cluster scale."
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
    c.line(x + 4, y - 1, x + w - 5, y - 1, (104, 57, 20, 255), 1)
    c.polygon([(x, y + 2), (x + 3, y), (x + w - 4, y), (x + w, y + 3),
               (x + w - 3, y + h), (x + 3, y + h)], (21, 26, 31, 255))
    c.line(x + 3, y, x + w - 4, y, ORANGE_HOT, 1)
    c.line(x + 5, y + 1, x + w - 6, y + 1, ORANGE, 1)
    c.line(x + 5, y + 3, x + w - 6, y + 3, (68, 77, 84, 255), 1)
    for px in range(int(x + 10), int(x + w - 7), 16):
        c.line(px, y + 4, px - 4, y + h - 1, (42, 49, 55, 255), 1)
        c.put(px + 3, y + 4, (91, 101, 108, 255))


def platform_surfaces():
    return [(32, 227, 64), (150, 284, 86), (260, 255, 88), (272, 169, 172),
            (374, 227, 71), (403, 255, 165), (448, 129, 294), (470, 257, 95),
            (592, 219, 90), (714, 177, 82), (769, 206, 127), (822, 215, 78)]


def generate_room():
    w, h = 960, 360
    bg = Canvas(w, h, (19, 24, 29, 255))
    # Quiet cool-grey value falloff keeps the authored chamber dimensional but restrained.
    for y in range(h):
        t = y / h
        color = (20 - int(4 * t), 25 - int(3 * t), 30 - int(2 * t), 255)
        bg.rect(0, y, w, 1, color)

    # Repeating wall plates share one measured pixel density across the full chamber.
    panel_w, panel_h = 80, 48
    for row, y in enumerate(range(-8, 320, panel_h)):
        offset = -40 if row % 2 else 0
        for x in range(offset, w, panel_w):
            shade = 27 + ((x // panel_w + row) & 1) * 2
            bg.rect(x + 1, y + 1, panel_w - 2, panel_h - 2,
                    (shade, shade + 5, shade + 9, 255))
            bg.line(x + 1, y, x + panel_w - 2, y, (48, 57, 64, 255), 1)
            bg.line(x, y + 1, x, y + panel_h - 2, (13, 18, 22, 255), 1)
            bg.put(x + 6, y + 6, (71, 80, 86, 255))
            bg.put(x + panel_w - 7, y + panel_h - 7, (13, 17, 21, 255))

    # Low-contrast braces and inset service panels give depth without competing detail.
    for x in (112, 352, 592, 832):
        bg.polygon([(x, 0), (x + 13, 0), (x + 70, 320), (x + 57, 320)], (18, 23, 28, 255))
        bg.line(x + 13, 0, x + 70, 320, (42, 50, 57, 255), 2)
    for x, y, width, height in [(44, 55, 92, 56), (242, 86, 108, 72),
                                (488, 44, 104, 64), (724, 75, 112, 70)]:
        chamfered(bg, x, y, width, height, 5, (21, 27, 32, 255), (52, 61, 68, 255))
        for vent_y in range(y + 13, y + height - 10, 8):
            bg.line(x + 14, vent_y, x + width - 14, vent_y, (42, 51, 58, 255), 2)
        bg.rect(x + width - 12, y + 7, 3, 3, (81, 91, 97, 255))

    # Restrained conduit runs stay in the wall plane and use no identity-color lighting.
    pipe(bg, [(0, 27), (192, 27), (192, 52)], (12, 16, 20, 255), (52, 61, 68, 255), 4)
    pipe(bg, [(610, 0), (610, 38), (686, 38)], (12, 16, 20, 255), (48, 57, 64, 255), 4)
    pipe(bg, [(960, 106), (898, 106), (898, 152)], (12, 16, 20, 255), (54, 62, 69, 255), 4)

    # Side collision walls and sparse structural uprights frame the playable volume.
    bg.rect(0, 0, 16, 320, (16, 21, 26, 255))
    bg.rect(944, 0, 16, 320, (16, 21, 26, 255))
    for x in (16, 224, 704, 936):
        bg.rect(x, 0, 8, 320, (22, 28, 33, 255))
        bg.line(x + 7, 0, x + 7, 320, (49, 58, 65, 255), 1)

    # Collision-aligned traversal layer: dark metal bodies and unambiguous orange edges.
    bg.rect(0, 320, w, 40, (18, 22, 27, 255))
    bg.line(0, 319, w, 319, (105, 57, 20, 255), 1)
    bg.line(0, 320, w, 320, ORANGE_HOT, 1)
    bg.line(0, 321, w, 321, ORANGE, 1)
    bg.rect(0, 323, w, 4, (56, 65, 72, 255))
    for x in range(0, w, 32):
        bg.rect(x + 1, 328, 30, 31, (23 + (x // 32 & 1) * 3, 29, 34, 255))
        bg.line(x + 4, 333, x + 27, 333, (49, 57, 63, 255), 1)
        bg.put(x + 5, 326, (96, 105, 110, 255))
    for x, y, width in platform_surfaces():
        draw_platform(bg, x, y, width)
        if width > 84:
            bg.line(x + 12, y + 8, x + 8, min(319, y + 23), (37, 44, 50, 255), 3)
            bg.line(x + width - 13, y + 8, x + width - 9, min(319, y + 23), (37, 44, 50, 255), 3)
    # Right solid test plinth, aligned to x=1780 y=552 at world scale.
    chamfered(bg, 890, 276, 54, 44, 5, (25, 31, 36, 255), (68, 78, 85, 255))
    bg.line(892, 276, 942, 276, ORANGE_HOT, 1)
    bg.line(894, 278, 940, 278, ORANGE, 1)
    for y in range(284, 315, 7):
        bg.line(897, y, 937, y, (48, 57, 63, 255), 1)
    bg.upscale(2).save_png(OUT / "foundation_background.png")

    fg = Canvas(w, h)
    # Minimal foreground framing and crisp near edges; the center stays unobstructed.
    fg.polygon([(0, 0), (72, 0), (62, 10), (12, 15)], (13, 17, 21, 255))
    fg.polygon([(888, 0), (960, 0), (960, 31), (930, 24)], (13, 17, 21, 255))
    fg.polygon([(0, 292), (10, 286), (17, 360), (0, 360)], (10, 14, 18, 245))
    fg.polygon([(951, 282), (960, 278), (960, 360), (946, 360)], (10, 14, 18, 245))
    for x, y, width in platform_surfaces():
        draw_platform(fg, x, y, width)
    fg.rect(0, 319, 960, 3, (104, 57, 20, 180))
    fg.rect(0, 320, 960, 1, ORANGE_HOT)
    chamfered(fg, 890, 276, 54, 44, 5, (25, 31, 36, 255), (68, 78, 85, 255))
    fg.line(892, 276, 942, 276, ORANGE_HOT, 1)
    fg.upscale(2).save_png(OUT / "foundation_foreground.png")


def generate_tiles():
    c = Canvas(128, 128, (15, 19, 23, 255))
    # Four coherent chamber materials: floor, orange-edged platform, wall, vent panel.
    for ox, oy in [(0, 0), (64, 0), (0, 64), (64, 64)]:
        c.rect(ox + 2, oy + 2, 60, 60, (26, 32, 37, 255))
        c.line(ox + 2, oy + 2, ox + 61, oy + 2, (61, 70, 77, 255), 1)
        c.line(ox + 2, oy + 2, ox + 2, oy + 61, (11, 15, 19, 255), 1)
        c.put(ox + 7, oy + 7, (88, 97, 102, 255))
        c.put(ox + 57, oy + 57, (10, 14, 18, 255))
    c.line(8, 10, 56, 10, ORANGE_HOT, 2)
    c.line(8, 14, 56, 14, ORANGE, 1)
    for x in range(10, 57, 12):
        c.line(x, 24, x - 5, 54, (47, 55, 61, 255), 2)
    c.line(72, 10, 120, 10, ORANGE_HOT, 2)
    c.rect(72, 16, 48, 32, (21, 26, 31, 255))
    for y in range(22, 47, 7):
        c.line(78, y, 114, y, (52, 61, 68, 255), 1)
    for y in range(76, 118, 10):
        c.line(10, y, 54, y, (48, 57, 64, 255), 1)
    chamfered(c, 72, 72, 48, 48, 4, (20, 26, 31, 255), (56, 66, 73, 255))
    for y in range(82, 113, 6):
        c.line(80, y, 112, y, (44, 53, 60, 255), 2)
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
