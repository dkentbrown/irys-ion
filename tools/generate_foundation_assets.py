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


def edged_polygon(c, points, fill, edge=INK):
    c.polygon(points, edge)
    cx = sum(point[0] for point in points) / len(points)
    cy = sum(point[1] for point in points) / len(points)
    c.polygon([(x + (1 if x < cx else -1), y + (1 if y < cy else -1)) for x, y in points], fill)


def generate_rig_pieces():
    rig_out = OUT / "irys_rig"
    pieces = {}

    def save(name, canvas, pivot):
        canvas.upscale(2).save_png(rig_out / f"{name}.png")
        pieces[name] = {"texture": f"{name}.png", "pivot": [pivot[0] * 2, pivot[1] * 2]}

    save("root", Canvas(1, 1), (0, 0))

    c = Canvas(10, 7)
    edged_polygon(c, [(1, 2), (3, 0), (7, 0), (9, 2), (8, 6), (2, 6)], GRAPHITE)
    c.line(2, 3, 8, 3, PALE, 1)
    c.line(4, 4, 6, 4, CYAN, 1)
    save("pelvis", c, (5, 3))

    c = Canvas(9, 10)
    edged_polygon(c, [(2, 0), (7, 0), (8, 4), (6, 9), (3, 9), (1, 4)], GRAPHITE)
    c.polygon([(3, 1), (6, 1), (6, 6), (5, 8), (3, 6)], GRAPHITE_LIT)
    c.line(3, 5, 6, 5, CYAN, 1)
    save("torso_lower", c, (4.5, 8))

    c = Canvas(13, 12)
    edged_polygon(c, [(2, 1), (5, 0), (8, 0), (11, 2), (12, 7), (9, 11), (4, 11), (1, 7)], GRAPHITE)
    c.polygon([(2, 3), (5, 1), (6, 5), (4, 8), (2, 7)], PALE)
    c.polygon([(11, 3), (8, 1), (7, 5), (9, 8), (11, 7)], PALE_LIT)
    c.line(5, 6, 8, 6, CYAN, 1)
    save("torso_upper", c, (6.5, 10))

    c = Canvas(5, 6)
    edged_polygon(c, [(1, 0), (4, 0), (4, 5), (1, 5)], JOINT)
    c.put(2, 2, CYAN)
    save("neck", c, (2.5, 5))

    c = Canvas(13, 13)
    edged_polygon(c, [(3, 1), (8, 0), (11, 3), (12, 7), (9, 11), (5, 12), (2, 9), (1, 4)], PALE)
    c.polygon([(7, 2), (10, 3), (10, 9), (8, 10), (7, 7)], PALE_LIT)
    c.put(10, 6, CYAN_WHITE)
    c.put(11, 6, CYAN)
    c.line(7, 9, 9, 10, GRAPHITE, 1)
    save("head", c, (6, 11))

    c = Canvas(15, 15)
    edged_polygon(c, [(2, 2), (6, 0), (12, 2), (14, 6), (12, 12), (8, 14), (3, 12), (0, 7)], HAIR)
    c.polygon([(2, 3), (6, 1), (4, 12), (2, 11)], HAIR_LIT)
    c.polygon([(10, 3), (13, 5), (11, 12), (9, 13)], PALE)
    save("hair_rear", c, (7, 11))

    c = Canvas(14, 13)
    c.polygon([(1, 3), (5, 0), (11, 1), (13, 4), (9, 3), (8, 9), (6, 5), (4, 11), (2, 9)], HAIR_LIT)
    c.line(2, 3, 10, 1, PALE, 1)
    c.polygon([(10, 2), (13, 4), (11, 8), (9, 5)], HAIR)
    save("hair_front", c, (6, 11))

    def limb(name, width, height, rear, accent):
        c = Canvas(width, height)
        edged_polygon(c, [(2, 1), (width - 3, 0), (width - 1, 3),
                          (width - 2, height - 2), (width - 4, height - 1),
                          (1, height - 3)], GRAPHITE if rear else JOINT)
        c.polygon([(3, 2), (width - 3, 2), (width - 3, height - 4),
                   (width - 4, height - 2), (3, height - 4)], GRAPHITE_LIT if rear else PALE)
        if accent:
            c.line(2, height // 2, width - 3, height // 2 + 1, CYAN, 1)
        save(name, c, (width / 2, 2))

    for name, width, height, rear, accent in [
        ("upper_arm_rear", 7, 12, True, False), ("forearm_rear", 6, 11, True, True),
        ("upper_arm_front", 7, 12, False, False), ("forearm_front", 6, 11, False, True),
        ("thigh_rear", 8, 14, True, False), ("shin_rear", 7, 13, True, True),
        ("thigh_front", 8, 14, False, False), ("shin_front", 7, 13, False, True)]:
        limb(name, width, height, rear, accent)

    for name, rear in (("hand_rear", True), ("hand_front", False)):
        c = Canvas(6, 7)
        edged_polygon(c, [(1, 1), (4, 0), (5, 3), (4, 6), (2, 6), (0, 3)], JOINT if rear else PALE)
        c.put(3, 3, CYAN)
        save(name, c, (3, 2))

    for name, rear in (("foot_rear", True), ("foot_front", False)):
        c = Canvas(10, 6)
        edged_polygon(c, [(1, 0), (5, 0), (6, 2), (9, 3), (9, 5), (1, 5), (0, 3)], GRAPHITE if rear else PALE)
        c.line(2, 4, 8, 4, GRAPHITE_LIT, 1)
        c.put(3, 2, CYAN)
        save(name, c, (3, 2))

    for name, rear in (("shoulder_rear", True), ("shoulder_front", False)):
        c = Canvas(8, 6)
        edged_polygon(c, [(0, 3), (2, 0), (6, 0), (7, 3), (5, 5), (1, 5)], GRAPHITE if rear else PALE)
        c.put(5, 2, CYAN)
        save(name, c, (4, 3))

    for name, rear in (("knee_rear", True), ("knee_front", False)):
        c = Canvas(7, 7)
        edged_polygon(c, [(2, 0), (5, 1), (6, 4), (4, 6), (1, 5), (0, 2)], GRAPHITE_LIT if rear else PALE)
        c.put(3, 3, CYAN)
        save(name, c, (3.5, 3.5))

    for name, rear in (("ankle_rear", True), ("ankle_front", False)):
        c = Canvas(7, 5)
        edged_polygon(c, [(1, 0), (6, 0), (6, 4), (1, 4)], GRAPHITE if rear else PALE)
        c.line(2, 2, 5, 2, CYAN, 1)
        save(name, c, (3, 2))

    c = Canvas(10, 7)
    edged_polygon(c, [(0, 2), (3, 2), (4, 0), (6, 0), (7, 2), (9, 2), (9, 5), (6, 5), (5, 7), (3, 5), (0, 5)], GRAPHITE)
    c.line(4, 1, 4, 6, CYAN, 1)
    save("ion_grip", c, (3, 3.5))

    c = Canvas(26, 8)
    edged_polygon(c, [(0, 2), (21, 1), (25, 3), (22, 6), (0, 6)], JOINT)
    c.polygon([(2, 3), (21, 2), (23, 3), (21, 5), (2, 5)], GRAPHITE_LIT)
    save("ion_blade", c, (2, 4))

    c = Canvas(26, 8)
    c.line(2, 4, 21, 3, CYAN, 1)
    c.line(4, 3, 19, 3, CYAN_WHITE, 1)
    c.put(22, 3, CYAN_WHITE)
    save("ion_channel", c, (2, 4))
    return pieces


def generate_rig_metadata(pieces):
    rig_out = OUT / "irys_rig"
    parts = []

    def part(name, parent, attachment, order, scale=(1, 1)):
        parts.append({"name": name, "texture": pieces[name]["texture"], "parent": parent,
                      "pivot": pieces[name]["pivot"], "attachment": list(attachment),
                      "render_order": order, "offset": [0, 0], "scale": list(scale)})

    for args in [
        ("root", "", (0, 0), -100), ("pelvis", "root", (0, -40), 30),
        ("torso_lower", "pelvis", (0, -7), 31), ("torso_upper", "torso_lower", (0, -10), 32),
        ("neck", "torso_upper", (0, -10), 35), ("head", "neck", (0, -3), 41, (.8, .8)),
        ("hair_rear", "head", (0, 0), 1), ("hair_front", "head", (0, 0), 43),
        ("upper_arm_rear", "torso_upper", (-5, -7), 20), ("shoulder_rear", "upper_arm_rear", (0, 0), 23),
        ("forearm_rear", "upper_arm_rear", (0, 16), 21), ("hand_rear", "forearm_rear", (0, 15), 22),
        ("upper_arm_front", "torso_upper", (5, -7), 60), ("shoulder_front", "upper_arm_front", (0, 0), 61),
        ("forearm_front", "upper_arm_front", (0, 16), 62), ("hand_front", "forearm_front", (0, 15), 67),
        ("thigh_rear", "pelvis", (-3, 2), 10), ("shin_rear", "thigh_rear", (0, 19), 11),
        ("knee_rear", "shin_rear", (0, 0), 13), ("foot_rear", "shin_rear", (0, 18), 12),
        ("ankle_rear", "foot_rear", (0, 0), 14), ("thigh_front", "pelvis", (3, 2), 50),
        ("shin_front", "thigh_front", (0, 19), 51), ("knee_front", "shin_front", (0, 0), 53),
        ("foot_front", "shin_front", (0, 18), 52), ("ankle_front", "foot_front", (0, 0), 54),
        ("ion_grip", "hand_front", (0, 6), 66), ("ion_blade", "ion_grip", (7, 0), 64),
        ("ion_channel", "ion_blade", (0, 0), 65)]:
        part(*args)
    (rig_out / "rig.json").write_text(json.dumps({"version": 1, "parts": parts}, indent=2) + "\n")


def generate_rig_animations():
    rig_out = OUT / "irys_rig"
    track_names = ["root", "pelvis", "torso_lower", "torso_upper", "head",
                   "upper_arm_front", "forearm_front", "upper_arm_rear", "forearm_rear",
                   "thigh_front", "shin_front", "foot_front", "thigh_rear", "shin_rear",
                   "foot_rear", "ion_grip"]
    standing = {"root": (0, 0, 0), "pelvis": -2, "torso_lower": 1, "torso_upper": 1, "head": 0,
                "upper_arm_front": 8, "forearm_front": 18, "upper_arm_rear": -5, "forearm_rear": 14,
                "thigh_front": -7, "shin_front": 16, "foot_front": -6,
                "thigh_rear": 7, "shin_rear": 14, "foot_rear": -5, "ion_grip": 28}
    clips = []

    def authored_pose(values):
        result = dict(standing)
        result.update(values)
        return result

    def keyframe(time, value):
        translation, rotation = ([value[0], value[1]], value[2]) if isinstance(value, tuple) else ([0, 0], value)
        return {"time": time, "translation": translation, "rotation": rotation, "scale": [1, 1]}

    def clip(name, duration, loop, authored):
        poses = [(time, authored_pose(values)) for time, values in authored]
        # Ion values author its desired world-space orientation. Bake them into the
        # hand-relative local track so runtime remains pure forward kinematics.
        def track_value(part, values):
            if part == "ion_grip":
                return values[part] - values["upper_arm_front"] - values["forearm_front"]
            return values[part]
        clips.append({"name": name, "duration": duration, "loop": loop,
                      "tracks": [{"part": part,
                                  "keyframes": [keyframe(time, track_value(part, values)) for time, values in poses]}
                                 for part in track_names]})

    clip("idle", 1.6, True, [
        (0, {"pelvis": (-.7, 0, -2), "torso_lower": 2, "torso_upper": 2, "head": -2,
             "thigh_front": -8, "shin_front": 18, "ion_grip": 28}),
        (.8, {"root": (0, -1, 0), "pelvis": (.7, 0, 1), "torso_lower": -2, "torso_upper": -1,
              "head": 2, "upper_arm_front": 11, "forearm_front": 15, "upper_arm_rear": -7,
              "forearm_rear": 17, "thigh_front": -5, "shin_front": 14,
              "thigh_rear": 8, "shin_rear": 16, "ion_grip": 31}),
        (1.6, {"pelvis": (-.7, 0, -2), "torso_lower": 2, "torso_upper": 2, "head": -2,
               "thigh_front": -8, "shin_front": 18, "ion_grip": 28})])

    clip("acceleration", .36, False, [
        (0, {"root": (0, 1, 0), "torso_upper": 5, "head": -3, "thigh_front": -14,
             "shin_front": 25, "thigh_rear": 12, "shin_rear": 12, "ion_grip": 24}),
        (.18, {"root": (0, -1, 0), "pelvis": (1, 0, 5), "torso_lower": 4, "torso_upper": 12,
               "head": -8, "upper_arm_front": 26, "forearm_front": 30, "upper_arm_rear": -25,
               "forearm_rear": 8, "thigh_front": -31, "shin_front": 43, "foot_front": 8,
               "thigh_rear": 24, "shin_rear": 8, "foot_rear": -12, "ion_grip": 18}),
        (.36, {"root": (0, -2, 0), "pelvis": (2, 0, 7), "torso_lower": 5, "torso_upper": 15,
               "head": -10, "upper_arm_front": 34, "forearm_front": 36, "upper_arm_rear": -32,
               "forearm_rear": 4, "thigh_front": -37, "shin_front": 52, "foot_front": 10,
               "thigh_rear": 28, "shin_rear": 5, "foot_rear": -15, "ion_grip": 15})])

    clip("run", .64, True, [
        (0, {"pelvis": 4, "torso_lower": 3, "torso_upper": 10, "head": -7,
             "upper_arm_front": 32, "forearm_front": 35, "upper_arm_rear": -34, "forearm_rear": 5,
             "thigh_front": -34, "shin_front": 24, "foot_front": 12,
             "thigh_rear": 28, "shin_rear": 42, "foot_rear": -18, "ion_grip": 17}),
        (.16, {"root": (0, -3, 0), "pelvis": (1, 0, -3), "torso_lower": 5, "torso_upper": 11,
               "head": -6, "upper_arm_front": 2, "forearm_front": 30, "upper_arm_rear": -5,
               "forearm_rear": 40, "thigh_front": -8, "shin_front": 38, "foot_front": -4,
               "thigh_rear": 5, "shin_rear": 72, "foot_rear": -28, "ion_grip": 20}),
        (.32, {"pelvis": 4, "torso_lower": 3, "torso_upper": 10, "head": -7,
               "upper_arm_front": -32, "forearm_front": 7, "upper_arm_rear": 30, "forearm_rear": 34,
               "thigh_front": 27, "shin_front": 43, "foot_front": -18,
               "thigh_rear": -35, "shin_rear": 25, "foot_rear": 13, "ion_grip": 16}),
        (.48, {"root": (0, -3, 0), "pelvis": (-1, 0, -3), "torso_lower": 5, "torso_upper": 11,
               "head": -6, "upper_arm_front": 0, "forearm_front": 42, "upper_arm_rear": 4,
               "forearm_rear": 27, "thigh_front": 4, "shin_front": 72, "foot_front": -28,
               "thigh_rear": -9, "shin_rear": 39, "foot_rear": -3, "ion_grip": 20}),
        (.64, {"pelvis": 4, "torso_lower": 3, "torso_upper": 10, "head": -7,
               "upper_arm_front": 32, "forearm_front": 35, "upper_arm_rear": -34, "forearm_rear": 5,
               "thigh_front": -34, "shin_front": 24, "foot_front": 12,
               "thigh_rear": 28, "shin_rear": 42, "foot_rear": -18, "ion_grip": 17})])

    clip("brake", .4, False, [
        (0, {"root": (0, 1, 0), "pelvis": -5, "torso_lower": -4, "torso_upper": -8, "head": 6,
             "upper_arm_front": -16, "forearm_front": 20, "upper_arm_rear": 20, "forearm_rear": 30,
             "thigh_front": -27, "shin_front": 38, "foot_front": 18,
             "thigh_rear": 20, "shin_rear": 28, "foot_rear": -15, "ion_grip": 8}),
        (.2, {"root": (-2, 4, 0), "pelvis": (-2, 0, -8), "torso_lower": -5, "torso_upper": -13,
              "head": 9, "upper_arm_front": -22, "forearm_front": 28, "upper_arm_rear": 25,
              "forearm_rear": 34, "thigh_front": -34, "shin_front": 52, "foot_front": 24,
              "thigh_rear": 26, "shin_rear": 42, "foot_rear": -20, "ion_grip": 4}),
        (.4, {"root": (0, 2, 0), "pelvis": -3, "torso_lower": -2, "torso_upper": -5,
              "head": 4, "thigh_front": -18, "shin_front": 32, "foot_front": 12,
              "thigh_rear": 14, "shin_rear": 24, "ion_grip": 15})])

    clip("turn", .48, False, [
        (0, {"root": (-2, 3, 0), "pelvis": (-2, 0, -10), "torso_lower": -5, "torso_upper": -14,
             "head": 10, "upper_arm_front": -28, "forearm_front": 32, "upper_arm_rear": 25,
             "forearm_rear": 35, "thigh_front": -30, "shin_front": 51, "foot_front": 20,
             "thigh_rear": 22, "shin_rear": 38, "foot_rear": -16, "ion_grip": 2}),
        (.24, {"root": (0, 5, 0), "pelvis": (0, 0, 12), "torso_lower": -8, "torso_upper": 1,
               "head": -3, "upper_arm_front": 8, "forearm_front": 58, "upper_arm_rear": -7,
               "forearm_rear": 50, "thigh_front": -8, "shin_front": 62, "foot_front": -12,
               "thigh_rear": 8, "shin_rear": 58, "foot_rear": -8, "ion_grip": 42}),
        (.48, {"root": (1, 0, 0), "pelvis": (2, 0, 6), "torso_lower": 4, "torso_upper": 14,
               "head": -9, "upper_arm_front": 30, "forearm_front": 36, "upper_arm_rear": -28,
               "forearm_rear": 8, "thigh_front": -32, "shin_front": 44, "foot_front": 8,
               "thigh_rear": 24, "shin_rear": 10, "foot_rear": -14, "ion_grip": 16})])

    clip("jump_launch", .3, False, [
        (0, {"root": (0, 7, 0), "pelvis": -4, "torso_lower": -3, "torso_upper": -6, "head": 5,
             "upper_arm_front": -18, "forearm_front": 40, "upper_arm_rear": 18, "forearm_rear": 42,
             "thigh_front": -28, "shin_front": 66, "foot_front": 19,
             "thigh_rear": 25, "shin_rear": 61, "foot_rear": -18, "ion_grip": 12}),
        (.15, {"root": (0, 4, 0), "pelvis": 5, "torso_lower": 5, "torso_upper": 10, "head": -8,
               "upper_arm_front": 24, "forearm_front": 34, "upper_arm_rear": -23, "forearm_rear": 20,
               "thigh_front": -12, "shin_front": 28, "foot_front": 6,
               "thigh_rear": 11, "shin_rear": 24, "foot_rear": -7, "ion_grip": 15}),
        (.3, {"root": (0, -3, 0), "pelvis": 7, "torso_lower": 6, "torso_upper": 13, "head": -9,
              "upper_arm_front": 36, "forearm_front": 28, "upper_arm_rear": -32, "forearm_rear": 9,
              "thigh_front": 4, "shin_front": 8, "foot_front": -8,
              "thigh_rear": 15, "shin_rear": 10, "foot_rear": -10, "ion_grip": 12})])

    clip("rising", .5, False, [
        (0, {"root": (0, -2, 0), "pelvis": 6, "torso_lower": 5, "torso_upper": 12, "head": -8,
             "upper_arm_front": 34, "forearm_front": 24, "upper_arm_rear": -30, "forearm_rear": 10,
             "thigh_front": 7, "shin_front": 12, "foot_front": -10,
             "thigh_rear": 21, "shin_rear": 18, "foot_rear": -12, "ion_grip": 10}),
        (.25, {"root": (0, -1, 0), "pelvis": 4, "torso_lower": 4, "torso_upper": 9, "head": -6,
               "upper_arm_front": 24, "forearm_front": 34, "upper_arm_rear": -20, "forearm_rear": 18,
               "thigh_front": 12, "shin_front": 18, "foot_front": -8,
               "thigh_rear": 27, "shin_rear": 25, "foot_rear": -15, "ion_grip": 14}),
        (.5, {"pelvis": 2, "torso_lower": 3, "torso_upper": 7, "head": -5,
              "upper_arm_front": 18, "forearm_front": 38, "upper_arm_rear": -14, "forearm_rear": 24,
              "thigh_front": 16, "shin_front": 24, "foot_front": -6,
              "thigh_rear": 22, "shin_rear": 30, "foot_rear": -12, "ion_grip": 18})])

    clip("apex", .4, False, [
        (0, {"pelvis": 0, "torso_lower": 2, "torso_upper": 3, "head": -2,
             "upper_arm_front": 8, "forearm_front": 44, "upper_arm_rear": -5, "forearm_rear": 35,
             "thigh_front": -20, "shin_front": 52, "foot_front": -14,
             "thigh_rear": 18, "shin_rear": 47, "foot_rear": -10, "ion_grip": 24}),
        (.2, {"root": (0, 1, 0), "pelvis": (1, 0, -2), "torso_upper": 1, "head": 0,
              "upper_arm_front": 2, "forearm_front": 48, "upper_arm_rear": 2, "forearm_rear": 40,
              "thigh_front": -27, "shin_front": 61, "foot_front": -18,
              "thigh_rear": 24, "shin_rear": 56, "foot_rear": -13, "ion_grip": 28}),
        (.4, {"pelvis": 0, "torso_lower": 2, "torso_upper": 3, "head": -2,
              "upper_arm_front": 8, "forearm_front": 44, "upper_arm_rear": -5, "forearm_rear": 35,
              "thigh_front": -20, "shin_front": 52, "foot_front": -14,
              "thigh_rear": 18, "shin_rear": 47, "foot_rear": -10, "ion_grip": 24})])

    clip("falling", .5, False, [
        (0, {"pelvis": -2, "torso_lower": -1, "torso_upper": -4, "head": 4,
             "upper_arm_front": -8, "forearm_front": 40, "upper_arm_rear": 12, "forearm_rear": 43,
             "thigh_front": -15, "shin_front": 38, "foot_front": 3,
             "thigh_rear": 16, "shin_rear": 34, "foot_rear": -4, "ion_grip": 34}),
        (.25, {"root": (0, 1, 0), "pelvis": -4, "torso_lower": -2, "torso_upper": -7, "head": 6,
               "upper_arm_front": -14, "forearm_front": 35, "upper_arm_rear": 18, "forearm_rear": 39,
               "thigh_front": -20, "shin_front": 46, "foot_front": 8,
               "thigh_rear": 20, "shin_rear": 43, "foot_rear": -7, "ion_grip": 38}),
        (.5, {"root": (0, 2, 0), "pelvis": -5, "torso_lower": -3, "torso_upper": -8, "head": 7,
              "upper_arm_front": -18, "forearm_front": 32, "upper_arm_rear": 22, "forearm_rear": 36,
              "thigh_front": -24, "shin_front": 52, "foot_front": 12,
              "thigh_rear": 23, "shin_rear": 49, "foot_rear": -10, "ion_grip": 40})])

    clip("soft_landing", .32, False, [
        (0, {"root": (0, 6, 0), "pelvis": -5, "torso_lower": -2, "torso_upper": -5, "head": 5,
             "upper_arm_front": -15, "forearm_front": 38, "upper_arm_rear": 18, "forearm_rear": 41,
             "thigh_front": -27, "shin_front": 61, "foot_front": 19,
             "thigh_rear": 24, "shin_rear": 55, "foot_rear": -17, "ion_grip": 35}),
        (.16, {"root": (0, 8, 0), "pelvis": -7, "torso_lower": -3, "torso_upper": -8, "head": 7,
               "upper_arm_front": -20, "forearm_front": 42, "upper_arm_rear": 23, "forearm_rear": 45,
               "thigh_front": -34, "shin_front": 72, "foot_front": 24,
               "thigh_rear": 31, "shin_rear": 68, "foot_rear": -22, "ion_grip": 39}),
        (.32, {"root": (0, 1, 0), "pelvis": -2, "torso_lower": 1, "torso_upper": 1, "head": -1,
               "thigh_front": -10, "shin_front": 24, "foot_front": -3,
               "thigh_rear": 10, "shin_rear": 22, "foot_rear": -5, "ion_grip": 28})])

    clip("hard_landing", .46, False, [
        (0, {"root": (0, 11, 0), "pelvis": -10, "torso_lower": -7, "torso_upper": -15, "head": 12,
             "upper_arm_front": -32, "forearm_front": 52, "upper_arm_rear": 35, "forearm_rear": 55,
             "thigh_front": -42, "shin_front": 84, "foot_front": 30,
             "thigh_rear": 39, "shin_rear": 78, "foot_rear": -28, "ion_grip": 48}),
        (.23, {"root": (-1, 14, 0), "pelvis": (-2, 0, -13), "torso_lower": -9, "torso_upper": -19,
               "head": 15, "upper_arm_front": -38, "forearm_front": 58, "upper_arm_rear": 42,
               "forearm_rear": 60, "thigh_front": -49, "shin_front": 94, "foot_front": 35,
               "thigh_rear": 45, "shin_rear": 90, "foot_rear": -33, "ion_grip": 54}),
        (.46, {"root": (0, 3, 0), "pelvis": -4, "torso_lower": -2, "torso_upper": -4, "head": 4,
               "upper_arm_front": -10, "forearm_front": 30, "upper_arm_rear": 12, "forearm_rear": 33,
               "thigh_front": -18, "shin_front": 39, "foot_front": 10,
               "thigh_rear": 17, "shin_rear": 36, "foot_rear": -9, "ion_grip": 34})])

    (rig_out / "animations.json").write_text(json.dumps({"version": 1, "clips": clips}, indent=2) + "\n")


def generate_irys_rig():
    pieces = generate_rig_pieces()
    generate_rig_metadata(pieces)
    generate_rig_animations()


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
    generate_irys_rig()
    generate_room()
    generate_tiles()
    print(f"Generated foundation assets in {OUT}")


if __name__ == "__main__":
    main()
