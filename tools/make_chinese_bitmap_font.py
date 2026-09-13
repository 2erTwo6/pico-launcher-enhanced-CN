#!/usr/bin/env python3
"""Build a Nitro Font 2 (.nft2) dot-matrix font for Pico Launcher.

The source is WenQuanYi Bitmap Song 9pt, a real pixel font.  At 9pt its
Latin glyphs are 8 pixels high and its Han glyphs are 11 pixels high with
an advance width of 12 pixels, which fits the launcher's 16 pixel labels.

Only glyphs needed by the Chinese UI are written, plus:
  * printable ASCII (letters, digits and punctuation),
  * the whole GB2312 character set (simplified Chinese + CJK punctuation),
  * printable BMP glyphs up to U+33FF,
  * fullwidth/halfwidth forms at U+FF00-U+FFFF.

The script needs pcf2bdf (Debian/Ubuntu package pcf2bdf) when the source is
a PCF file, or an already converted BDF file.
"""

import argparse
import os
import re
import struct
import subprocess
import sys
import tempfile

FONT_ASCENT = 12
FONT_DESCENT = 3
CHAR_MAP_MIN = 0x0020
CHAR_MAP_MAX = 0x9FFF
FULLWIDTH_MIN = 0xFF00
FULLWIDTH_MAX = 0xFFFF
DEFAULT_BDF = "/usr/share/fonts/X11/misc/wenquanyi_9pt.pcf"


def gb2312_charset():
    chars = set()
    for b1 in range(0xA1, 0xFF):
        for b2 in range(0xA1, 0xFF):
            try:
                chars.add(ord(bytes((b1, b2)).decode("gb2312")))
            except UnicodeDecodeError:
                pass
    return chars


def source_charset(source_dirs):
    chars = set()
    pattern = re.compile(r'[^\x00-\x7F]')
    for directory in source_dirs:
        if not os.path.isdir(directory):
            continue
        for root, _, files in os.walk(directory):
            for name in files:
                if not name.endswith((".c", ".cpp", ".h")):
                    continue
                with open(os.path.join(root, name), encoding="utf-8", errors="ignore") as f:
                    chars.update(ord(c) for c in pattern.findall(f.read()))
    return chars


def convert_pcf_to_bdf(path):
    if path.endswith(".bdf"):
        return path, None
    tmp = tempfile.NamedTemporaryFile(suffix=".bdf", delete=False)
    tmp.close()
    with open(tmp.name, "w") as out:
        subprocess.run(["pcf2bdf", path], check=True, stdout=out)
    return tmp.name, tmp.name


def read_bdf(path):
    glyphs = {}
    properties = {}
    in_properties = False
    state = 0
    encoding = None
    bbx = None
    dwidth = None
    rows = []

    with open(path, encoding="latin1") as f:
        for raw_line in f:
            line = raw_line.rstrip("\n")
            if line.startswith("STARTPROPERTIES"):
                in_properties = True
                continue
            if line.startswith("ENDPROPERTIES"):
                in_properties = False
                continue
            if in_properties:
                parts = line.split(None, 1)
                if parts:
                    properties[parts[0]] = parts[1] if len(parts) > 1 else ""
                continue

            if line.startswith("STARTCHAR"):
                state = 1
                encoding = None
                bbx = None
                dwidth = None
                rows = []
            elif state == 1:
                if line.startswith("ENCODING"):
                    try:
                        encoding = int(line.split()[1])
                    except (IndexError, ValueError):
                        encoding = None
                    state = 2
                elif line.startswith("ENDCHAR"):
                    state = 0
            elif state == 2:
                if line.startswith("DWIDTH"):
                    dwidth = tuple(int(v) for v in line.split()[1:3])
                elif line.startswith("BBX"):
                    bbx = tuple(int(v) for v in line.split()[1:5])
                elif line.startswith("BITMAP"):
                    state = 3
            elif state == 3:
                if line.startswith("ENDCHAR"):
                    if encoding is not None:
                        glyphs[encoding] = (bbx, dwidth, rows)
                    state = 0
                else:
                    rows.append(line)

    return properties, glyphs


def encode_glyph(bbx, dwidth, rows):
    width, height, x_offset, y_offset = bbx
    if width < 0 or height < 0:
        raise ValueError(f"invalid BBX {bbx}")
    if len(rows) != height:
        raise ValueError(f"bitmap row count mismatch for {bbx}: {len(rows)}")

    bytes_per_bdf_row = (width + 7) // 8
    stride = (width + 1) // 2
    data = bytearray(stride * height)

    for y in range(height):
        row = rows[y].strip()
        raw = bytes.fromhex(row) if row else b""
        if len(raw) < bytes_per_bdf_row:
            raw += b"\0" * (bytes_per_bdf_row - len(raw))

        for x in range(width):
            if not ((raw[x >> 3] >> (7 - (x & 7))) & 1):
                continue
            if x & 1:
                data[y * stride + (x >> 1)] |= 0xF0  # odd pixel: high nibble
            else:
                data[y * stride + (x >> 1)] |= 0x0F  # even pixel: low nibble

    return (
        bytes(data),
        x_offset,                                # spacingLeft
        dwidth[0] - x_offset - width,            # spacingRight
        FONT_ASCENT - (y_offset + height),       # spacingTop
        width,
        height,
    )


def build_nft2(glyphs, wanted, output):
    # Assign glyph indices in code point order.  U+0020 is first and becomes
    # glyph 0, the blank fallback used by nft2_findGlyphIdxForCharacter().
    ordered = sorted(cp for cp in wanted if cp in glyphs)
    cp_to_index = {}
    unique = {}
    glyph_infos = []
    glyph_data = bytearray()

    for cp in ordered:
        encoded = encode_glyph(*glyphs[cp])
        key = encoded
        index = unique.get(key)
        if index is None:
            index = len(glyph_infos)
            if index > 0xFFFF:
                raise ValueError("too many glyphs for NFTR2")
            offset = len(glyph_data)
            if offset > 0xFFFFFF:
                raise ValueError("glyph data exceeds 24-bit NFTR2 offset")
            unique[key] = index
            glyph_infos.append((offset, encoded[4], encoded[1], encoded[2], encoded[5], encoded[3]))
            glyph_data += encoded[0]
        cp_to_index[cp] = index

    if cp_to_index.get(0x20) != 0:
        raise ValueError("U+0020 must be present and become glyph 0")

    def make_map(low, high):
        data = bytearray(2 * (high - low + 1))
        for cp in range(low, high + 1):
            struct.pack_into("<H", data, 2 * (cp - low), cp_to_index.get(cp, 0))
        return data

    char_map = bytearray()
    for low, high in ((CHAR_MAP_MIN, CHAR_MAP_MAX), (FULLWIDTH_MIN, FULLWIDTH_MAX)):
        count = high - low + 1
        if count > 0xFFFF:
            raise ValueError("character range too large")
        char_map += struct.pack("<HH", count, low)
        char_map += make_map(low, high)
    char_map += b"\0\0\0\0"  # count == 0 terminates the list

    glyph_table_offset = 20
    char_map_offset = glyph_table_offset + 8 * len(glyph_infos)
    glyph_data_offset = char_map_offset + len(char_map)

    header = struct.pack(
        "<IIIIBBH",
        0x3254464E,  # 'NFT2' little-endian
        glyph_table_offset,
        char_map_offset,
        glyph_data_offset,
        FONT_ASCENT,
        FONT_DESCENT,
        len(glyph_infos),
    )

    glyph_table = bytearray()
    for offset, width, spacing_left, spacing_right, height, spacing_top in glyph_infos:
        raw_offset_width = (offset & 0xFFFFFF) | ((width & 0xFF) << 24)
        glyph_table += struct.pack(
            "<IbbBb", raw_offset_width, spacing_left, spacing_right, height, spacing_top
        )

    with open(output, "wb") as f:
        f.write(header)
        f.write(glyph_table)
        f.write(char_map)
        f.write(glyph_data)

    return len(glyph_infos), len(glyph_data), len(char_map)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--bdf", default=DEFAULT_BDF, help="WenQuanYi 9pt PCF or BDF file")
    parser.add_argument("--out", default="arm9/data/BitmapSong-9pt.nft2", help="output .nft2 file")
    parser.add_argument("--source-dir", action="append", default=["arm9/source"],
                        help="also include every non-ASCII character in this source tree")
    args = parser.parse_args()

    bdf_path, temporary = convert_pcf_to_bdf(args.bdf)
    try:
        properties, glyphs = read_bdf(bdf_path)
    finally:
        if temporary:
            os.unlink(temporary)

    wanted = set(range(0x20, 0x7F))
    wanted |= gb2312_charset()
    wanted |= source_charset(args.source_dir)
    # Include the printable symbol and kana/CJK-punctuation ranges available
    # in the source font, plus fullwidth forms, without pulling in all 20k CJK
    # ideographs (GB2312 is included explicitly above).
    for cp in glyphs:
        if CHAR_MAP_MIN <= cp <= 0x33FF or FULLWIDTH_MIN <= cp <= FULLWIDTH_MAX:
            wanted.add(cp)

    out_dir = os.path.dirname(args.out)
    if out_dir:
        os.makedirs(out_dir, exist_ok=True)

    glyph_count, glyph_bytes, char_map_bytes = build_nft2(glyphs, wanted, args.out)
    total = os.path.getsize(args.out)
    print(f"source:  {args.bdf}")
    print(f"output:  {args.out}")
    print(f"glyphs:  {glyph_count}")
    print(f"data:    {glyph_bytes} bytes")
    print(f"charmap: {char_map_bytes} bytes")
    print(f"total:   {total} bytes")


if __name__ == "__main__":
    main()
