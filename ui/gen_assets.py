#!/usr/bin/env python3
"""Generate crisp SVG RGBA icons and anti-aliased TTF font atlases into ui_assets.h."""

import os
import subprocess
from pathlib import Path
from PIL import Image, ImageFont, ImageDraw

UI_DIR = Path(__file__).resolve().parent
OUT_H = UI_DIR / "ui_assets.h"

ICONS_SVG = {
    "arch": """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 80 80">
      <defs>
        <linearGradient id="g" x1="0" y1="0" x2="1" y2="1">
          <stop offset="0%" stop-color="#38BDF8"/>
          <stop offset="100%" stop-color="#0284C7"/>
        </linearGradient>
      </defs>
      <circle cx="40" cy="40" r="34" fill="#0B2233" stroke="#1E4E68" stroke-width="2"/>
      <path d="M40 14 C37 22 35 27 31 35 C34 38 38 41 43 44 C37 42 33 39 29 37 C24 47 20 53 16 62 C24 57 31 55 37 54 C36.7 52.7 36.5 51.3 36.6 49.8 C36.8 46.2 38.6 43.3 40.7 43.5 C42.8 43.7 44.3 46.8 44.1 50.4 C44.0 51.8 43.7 53.0 43.3 54.1 C49.5 55.3 56.2 57.6 64 62 C62.3 58.8 60.8 55.9 59.3 53.2 C57.0 51.4 54.6 49.1 49.8 46.6 C53.1 47.5 55.5 48.5 57.4 49.6 C43 22.8 41.8 19.2 40 14 Z" fill="url(#g)"/>
    </svg>""",
    "alpine": """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 80 80">
      <defs>
        <linearGradient id="g" x1="0" y1="0" x2="1" y2="1">
          <stop offset="0%" stop-color="#2DD4BF"/>
          <stop offset="100%" stop-color="#0F766E"/>
        </linearGradient>
      </defs>
      <circle cx="40" cy="40" r="34" fill="#092428" stroke="#1B565C" stroke-width="2"/>
      <polygon points="40,16 62,56 18,56" fill="none" stroke="url(#g)" stroke-width="3.5" stroke-linejoin="round"/>
      <polygon points="29,34 44,56 18,56" fill="#36D1DC" opacity="0.85"/>
      <polygon points="40,22 57,56 35,56" fill="#0EA5E9" opacity="0.75"/>
      <polygon points="40,22 46,34 40,31 35,35" fill="#F8FAFC"/>
    </svg>""",
    "debian": """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 80 80">
      <defs>
        <linearGradient id="g" x1="0" y1="0" x2="1" y2="1">
          <stop offset="0%" stop-color="#FB7185"/>
          <stop offset="100%" stop-color="#BE123C"/>
        </linearGradient>
      </defs>
      <circle cx="40" cy="40" r="34" fill="#2A0E19" stroke="#6B1D38" stroke-width="2"/>
      <path d="M45 58 C31 58 21 47 22 33 C23 20 35 14 47 17 C57 20 62 30 58 39 C54 47 44 49 38 45 C33 41 33 34 38 31 C42 29 47 31 46 36" fill="none" stroke="url(#g)" stroke-width="5" stroke-linecap="round"/>
      <circle cx="42" cy="36" r="3" fill="#FDA4AF"/>
    </svg>""",
    "fedora": """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 80 80">
      <defs>
        <linearGradient id="g" x1="0" y1="0" x2="1" y2="1">
          <stop offset="0%" stop-color="#60A5FA"/>
          <stop offset="100%" stop-color="#1D4ED8"/>
        </linearGradient>
      </defs>
      <path d="M40 7 C58.2 7 73 21.8 73 40 C73 58.2 58.2 73 40 73 L22 73 C13.7 73 7 66.3 7 58 L7 40 C7 21.8 21.8 7 40 7 Z" fill="#0F2347" stroke="#295194" stroke-width="2"/>
      <path d="M47 23 C41 23 37 27 37 33 L37 41 L30 41 C24 41 20 45 20 51 C20 56 24 60 30 60 C35 60 39 57 41 52 L41 41 L50 41" fill="none" stroke="url(#g)" stroke-width="5.5" stroke-linecap="round" stroke-linejoin="round"/>
      <circle cx="52" cy="27" r="4" fill="#93C5FD"/>
    </svg>""",
    "windows": """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 80 80">
      <defs>
        <linearGradient id="g" x1="0" y1="0" x2="1" y2="1">
          <stop offset="0%" stop-color="#38BDF8"/>
          <stop offset="100%" stop-color="#0284C7"/>
        </linearGradient>
      </defs>
      <rect x="7" y="7" width="66" height="66" rx="16" fill="#0B2239" stroke="#1E5278" stroke-width="2"/>
      <rect x="20" y="20" width="18" height="18" rx="3" fill="#38BDF8"/>
      <rect x="42" y="20" width="18" height="18" rx="3" fill="#0EA5E9"/>
      <rect x="20" y="42" width="18" height="18" rx="3" fill="#0EA5E9"/>
      <rect x="42" y="42" width="18" height="18" rx="3" fill="#0284C7"/>
    </svg>""",
    "memtest": """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 80 80">
      <circle cx="40" cy="40" r="34" fill="#0A2720" stroke="#195C4A" stroke-width="2"/>
      <rect x="16" y="27" width="48" height="24" rx="4" fill="#10B981" opacity="0.22" stroke="#34D399" stroke-width="2.5"/>
      <rect x="22" y="33" width="9" height="12" rx="1.5" fill="#34D399"/>
      <rect x="35.5" y="33" width="9" height="12" rx="1.5" fill="#34D399"/>
      <rect x="49" y="33" width="9" height="12" rx="1.5" fill="#34D399"/>
      <path d="M22 54 L26 54 M30 54 L34 54 M38 54 L42 54 M46 54 L50 54 M54 54 L58 54" stroke="#FBBF24" stroke-width="3" stroke-linecap="round"/>
    </svg>""",
    "netboot": """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 80 80">
      <circle cx="40" cy="40" r="34" fill="#191638" stroke="#473A8C" stroke-width="2"/>
      <circle cx="40" cy="40" r="20" fill="none" stroke="#A78BFA" stroke-width="2.5"/>
      <ellipse cx="40" cy="40" rx="9" ry="20" fill="none" stroke="#36D1DC" stroke-width="2.2"/>
      <line x1="20" y1="40" x2="60" y2="40" stroke="#A78BFA" stroke-width="2.2"/>
      <path d="M24 29 Q40 34 56 29 M24 51 Q40 46 56 51" fill="none" stroke="#36D1DC" stroke-width="2"/>
    </svg>""",
    "info": """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 80 80">
      <circle cx="40" cy="40" r="34" fill="#291E0B" stroke="#6E4E16" stroke-width="2"/>
      <rect x="21" y="21" width="38" height="38" rx="7" fill="none" stroke="#FBBF24" stroke-width="2.5"/>
      <polyline points="26,42 33,42 37,31 43,49 47,38 54,38" fill="none" stroke="#36D1DC" stroke-width="3" stroke-linecap="round" stroke-linejoin="round"/>
    </svg>""",
    "local": """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 80 80">
      <circle cx="40" cy="40" r="34" fill="#0B2230" stroke="#1E526E" stroke-width="2"/>
      <rect x="19" y="23" width="42" height="34" rx="6" fill="#0F2D3E" stroke="#38BDF8" stroke-width="2.5"/>
      <line x1="25" y1="45" x2="55" y2="45" stroke="#38BDF8" stroke-width="2" opacity="0.6"/>
      <circle cx="50" cy="51" r="2.8" fill="#36D1DC"/>
      <rect x="26" y="30" width="18" height="7" rx="2" fill="#38BDF8" opacity="0.8"/>
    </svg>""",
    "shell": """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 80 80">
      <circle cx="40" cy="40" r="34" fill="#131B24" stroke="#33485C" stroke-width="2"/>
      <rect x="18" y="23" width="44" height="34" rx="5" fill="#0B1117" stroke="#94A3B8" stroke-width="2.2"/>
      <polyline points="26,35 34,40 26,45" fill="none" stroke="#36D1DC" stroke-width="3" stroke-linecap="round" stroke-linejoin="round"/>
      <line x1="38" y1="45" x2="51" y2="45" stroke="#E2E8F0" stroke-width="3" stroke-linecap="round"/>
    </svg>""",
    "brand": """<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 28 28">
      <polygon points="14,2 25,14 14,26 3,14" fill="none" stroke="#36D1DC" stroke-width="2.6"/>
      <polygon points="14,8 19,14 14,20 9,14" fill="#36D1DC"/>
    </svg>""",
}


def render_svg_bgra(name: str, svg: str, size: int) -> list[int]:
    svg_p = UI_DIR / f"_tmp_{name}.svg"
    png_p = UI_DIR / f"_tmp_{name}.png"
    svg_p.write_text(svg)
    subprocess.run(
        ["rsvg-convert", "-w", str(size), "-h", str(size), str(svg_p), "-o", str(png_p)],
        check=True,
    )
    img = Image.open(png_p).convert("RGBA")
    out = []
    for r, g, b, a in img.getdata():
        # Store as ARGB32 (UEFI GOP EfiBltVideo is B,G,R,Reserved in little-endian uint32 -> 0xAARRGGBB)
        out.append((a << 24) | (r << 16) | (g << 8) | b)
    svg_p.unlink(missing_ok=True)
    png_p.unlink(missing_ok=True)
    return out


def build_font(ttf_path: str, size: int, prefix: str):
    font = ImageFont.truetype(ttf_path, size)
    ascent, descent = font.getmetrics()
    line_h = ascent + descent
    glyph_meta = []
    bitmap = bytearray()
    for ch in range(32, 127):
        s = chr(ch)
        bbox = font.getbbox(s)
        adv = int(round(font.getlength(s)))
        if bbox is None or s == " ":
            glyph_meta.append((len(bitmap), 0, 0, 0, 0, max(adv, size // 3)))
            continue
        x0, y0, x1, y1 = bbox
        w = max(0, x1 - x0)
        h = max(0, y1 - y0)
        if w == 0 or h == 0:
            glyph_meta.append((len(bitmap), 0, 0, 0, 0, max(adv, 4)))
            continue
        canvas = Image.new("L", (w + 4, h + 4), 0)
        d = ImageDraw.Draw(canvas)
        d.text((-x0 + 2, -y0 + 2), s, fill=255, font=font)
        cropped = canvas.crop((2, 2, 2 + w, 2 + h))
        offset = len(bitmap)
        bitmap.extend(cropped.tobytes())
        glyph_meta.append((offset, w, h, x0, y0, adv))
    return line_h, glyph_meta, bitmap


def main():
    lines = [
        "#ifndef UI_ASSETS_H",
        "#define UI_ASSETS_H",
        "#include <stdint.h>",
        "",
        "typedef struct {",
        "    uint32_t offset;",
        "    uint8_t w, h;",
        "    int8_t xoff, yoff;",
        "    uint8_t adv;",
        "} Glyph;",
        "",
    ]

    for name, svg in ICONS_SVG.items():
        sz = 28 if name == "brand" else 80
        px = render_svg_bgra(name, svg, sz)
        lines.append(f"static const uint32_t icon_{name}[{sz * sz}] = {{")
        for i in range(0, len(px), 8):
            chunk = ", ".join(f"0x{v:08x}u" for v in px[i : i + 8])
            lines.append(f"    {chunk},")
        lines.append("};")
        lines.append("")

    fonts = [
        ("font_hdr", "/usr/share/fonts/dejavu/DejaVuSans-Bold.ttf", 19),
        ("font_title", "/usr/share/fonts/dejavu/DejaVuSans-Bold.ttf", 18),
        ("font_sub", "/usr/share/fonts/dejavu/DejaVuSans.ttf", 13),
        ("font_badge", "/usr/share/fonts/hack/Hack-Bold.ttf", 13),
    ]
    for cname, path, px in fonts:
        lh, meta, bm = build_font(path, px, cname)
        lines.append(f"#define {cname.upper()}_H {lh}")
        lines.append(f"static const Glyph {cname}_glyphs[95] = {{")
        for off, w, h, xo, yo, adv in meta:
            lines.append(f"    {{{off}u, {w}u, {h}u, {xo}, {yo}, {adv}u}},")
        lines.append("};")
        lines.append(f"static const uint8_t {cname}_bitmap[{len(bm)}] = {{")
        for i in range(0, len(bm), 16):
            chunk = ", ".join(f"0x{b:02x}" for b in bm[i : i + 16])
            lines.append(f"    {chunk},")
        lines.append("};")
        lines.append("")

    lines.append("#endif /* UI_ASSETS_H */")
    OUT_H.write_text("\n".join(lines))
    print(f"Generated {OUT_H} ({OUT_H.stat().st_size} bytes)")


if __name__ == "__main__":
    main()
