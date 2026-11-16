#include <stdint.h>
#include <stddef.h>
#include "ui_assets.h"

/* Freestanding helpers */
void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    while (n--) *d++ = *s++;
    return dst;
}

void *memset(void *dst, int c, size_t n) {
    uint8_t *d = (uint8_t *)dst;
    while (n--) *d++ = (uint8_t)c;
    return dst;
}

void __chkstk(void) {}

/* Minimal UEFI 2.x definitions (x86_64 ms_abi) */
typedef uint64_t UINTN;
typedef int64_t  INTN;
typedef uint64_t EFI_STATUS;
typedef void    *EFI_HANDLE;
typedef void    *EFI_EVENT;
typedef uint8_t  BOOLEAN;

#define EFI_ERROR_BIT (1ULL << 63)
#define EFI_SUCCESS   0ULL
#define TRUE          1
#define FALSE         0

typedef struct {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t  Data4[8];
} EFI_GUID;

typedef struct {
    uint64_t Signature;
    uint32_t Revision;
    uint32_t HeaderSize;
    uint32_t CRC32;
    uint32_t Reserved;
} EFI_TABLE_HEADER;

typedef struct {
    uint16_t ScanCode;
    uint16_t UnicodeChar;
} EFI_INPUT_KEY;

typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
    EFI_STATUS (*Reset)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This, BOOLEAN ExtendedVerification);
    EFI_STATUS (*ReadKeyStroke)(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This, EFI_INPUT_KEY *Key);
    EFI_EVENT WaitForKey;
};

typedef struct {
    uint32_t RedMask;
    uint32_t GreenMask;
    uint32_t BlueMask;
    uint32_t ReservedMask;
} EFI_PIXEL_BITMASK;

typedef enum {
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct {
    uint32_t Version;
    uint32_t HorizontalResolution;
    uint32_t VerticalResolution;
    EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
    EFI_PIXEL_BITMASK PixelInformation;
    uint32_t PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
    uint32_t MaxMode;
    uint32_t Mode;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
    UINTN    SizeOfInfo;
    uint64_t FrameBufferBase;
    UINTN    FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef enum {
    EfiBltVideoFill,
    EfiBltVideoToBltBuffer,
    EfiBltBufferToVideo,
    EfiBltVideoToVideo,
    EfiGraphicsOutputBltOperationMax
} EFI_GRAPHICS_OUTPUT_BLT_OPERATION;

typedef struct {
    uint8_t Blue;
    uint8_t Green;
    uint8_t Red;
    uint8_t Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL;

typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL EFI_GRAPHICS_OUTPUT_PROTOCOL;
struct _EFI_GRAPHICS_OUTPUT_PROTOCOL {
    EFI_STATUS (*QueryMode)(EFI_GRAPHICS_OUTPUT_PROTOCOL *This, uint32_t ModeNumber,
                            UINTN *SizeOfInfo, EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **Info);
    EFI_STATUS (*SetMode)(EFI_GRAPHICS_OUTPUT_PROTOCOL *This, uint32_t ModeNumber);
    EFI_STATUS (*Blt)(EFI_GRAPHICS_OUTPUT_PROTOCOL *This, EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer,
                      EFI_GRAPHICS_OUTPUT_BLT_OPERATION BltOperation,
                      UINTN SourceX, UINTN SourceY, UINTN DestinationX, UINTN DestinationY,
                      UINTN Width, UINTN Height, UINTN Delta);
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode;
};

typedef struct {
    int32_t RelativeMovementX;
    int32_t RelativeMovementY;
    int32_t RelativeMovementZ;
    BOOLEAN LeftButton;
    BOOLEAN RightButton;
} EFI_SIMPLE_POINTER_STATE;

typedef struct {
    uint64_t ResolutionX;
    uint64_t ResolutionY;
    uint64_t ResolutionZ;
    BOOLEAN  LeftButton;
    BOOLEAN  RightButton;
} EFI_SIMPLE_POINTER_MODE;

typedef struct _EFI_SIMPLE_POINTER_PROTOCOL EFI_SIMPLE_POINTER_PROTOCOL;
struct _EFI_SIMPLE_POINTER_PROTOCOL {
    EFI_STATUS (*Reset)(EFI_SIMPLE_POINTER_PROTOCOL *This, BOOLEAN ExtendedVerification);
    EFI_STATUS (*GetState)(EFI_SIMPLE_POINTER_PROTOCOL *This, EFI_SIMPLE_POINTER_STATE *State);
    EFI_EVENT WaitForInput;
    EFI_SIMPLE_POINTER_MODE *Mode;
};

typedef enum {
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData
} EFI_MEMORY_TYPE;

typedef struct {
    EFI_TABLE_HEADER Hdr;
    void *RaiseTPL;
    void *RestoreTPL;
    void *AllocatePages;
    void *FreePages;
    void *GetMemoryMap;
    EFI_STATUS (*AllocatePool)(EFI_MEMORY_TYPE PoolType, UINTN Size, void **Buffer);
    EFI_STATUS (*FreePool)(void *Buffer);
    void *CreateEvent;
    void *SetTimer;
    void *WaitForEvent;
    void *SignalEvent;
    void *CloseEvent;
    EFI_STATUS (*CheckEvent)(EFI_EVENT Event);
    void *InstallProtocolInterface;
    void *ReinstallProtocolInterface;
    void *UninstallProtocolInterface;
    void *HandleProtocol;
    void *Reserved;
    void *RegisterProtocolNotify;
    void *LocateHandle;
    void *LocateDevicePath;
    void *InstallConfigurationTable;
    void *LoadImage;
    void *StartImage;
    void *Exit;
    void *UnloadImage;
    void *ExitBootServices;
    void *GetNextMonotonicCount;
    EFI_STATUS (*Stall)(UINTN Microseconds);
    EFI_STATUS (*SetWatchdogTimer)(UINTN Timeout, uint64_t WatchdogCode, UINTN DataSize, uint16_t *WatchdogData);
    void *ConnectController;
    void *DisconnectController;
    void *OpenProtocol;
    void *CloseProtocol;
    void *OpenProtocolInformation;
    void *ProtocolsPerHandle;
    void *LocateHandleBuffer;
    EFI_STATUS (*LocateProtocol)(const EFI_GUID *Protocol, void *Registration, void **Interface);
} EFI_BOOT_SERVICES;

typedef struct {
    EFI_TABLE_HEADER Hdr;
    uint16_t *FirmwareVendor;
    uint32_t FirmwareRevision;
    EFI_HANDLE ConsoleInHandle;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;
    EFI_HANDLE ConsoleOutHandle;
    void *ConOut;
    EFI_HANDLE StandardErrorHandle;
    void *StdErr;
    void *RuntimeServices;
    EFI_BOOT_SERVICES *BootServices;
} EFI_SYSTEM_TABLE;

static const EFI_GUID gGopGuid = {
    0x9042a9de, 0x23dc, 0x4a38, {0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a}
};
static const EFI_GUID gPointerGuid = {
    0x31878c87, 0x0b75, 0x11d5, {0x9a, 0x4f, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d}
};

/* Global drawing context */
static uint32_t gW = 1920, gH = 1080;
static uint32_t *gFB = NULL;
static uint32_t *gBG = NULL;

static inline uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

static inline uint32_t blend(uint32_t bg, uint32_t fg, uint8_t a) {
    if (a == 0) return bg;
    if (a == 255) return fg & 0x00ffffffu;
    uint32_t ia = 255u - a;
    uint32_t r = ((((fg >> 16) & 0xffu) * a) + (((bg >> 16) & 0xffu) * ia)) / 255u;
    uint32_t g = ((((fg >> 8) & 0xffu) * a) + (((bg >> 8) & 0xffu) * ia)) / 255u;
    uint32_t b = (((fg & 0xffu) * a) + ((bg & 0xffu) * ia)) / 255u;
    return (r << 16) | (g << 8) | b;
}

static void put_px(int x, int y, uint32_t col, uint8_t a) {
    if ((uint32_t)x >= gW || (uint32_t)y >= gH) return;
    gFB[y * gW + x] = blend(gFB[y * gW + x], col, a);
}

static void fill_rect(int x, int y, int w, int h, uint32_t col, uint8_t a) {
    int x0 = x < 0 ? 0 : x;
    int y0 = y < 0 ? 0 : y;
    int x1 = (x + w > (int)gW) ? (int)gW : x + w;
    int y1 = (y + h > (int)gH) ? (int)gH : y + h;
    for (int yy = y0; yy < y1; yy++) {
        uint32_t *row = &gFB[yy * gW];
        if (a == 255) {
            for (int xx = x0; xx < x1; xx++) row[xx] = col;
        } else {
            for (int xx = x0; xx < x1; xx++) row[xx] = blend(row[xx], col, a);
        }
    }
}

static uint32_t isqrt_u32(uint32_t x) {
    uint32_t r = 0, b = 1u << 30;
    while (b > x) b >>= 2;
    while (b != 0) {
        if (x >= r + b) { x -= r + b; r = (r >> 1) + b; }
        else r >>= 1;
        b >>= 2;
    }
    return r;
}

/* Rounded rectangle fill + optional 1px/2px border with anti-aliased corners */
static void draw_rrect(int x, int y, int w, int h, int rad,
                       uint32_t fill, uint8_t fill_a,
                       uint32_t border, uint8_t border_a, int bw) {
    int x0 = x < 0 ? 0 : x;
    int y0 = y < 0 ? 0 : y;
    int x1 = (x + w > (int)gW) ? (int)gW : x + w;
    int y1 = (y + h > (int)gH) ? (int)gH : y + h;
    for (int yy = y0; yy < y1; yy++) {
        int dy_top = yy - y;
        int dy_bot = (y + h - 1) - yy;
        int cy = (dy_top < rad) ? (rad - dy_top) : (dy_bot < rad ? (rad - dy_bot) : 0);
        uint32_t *row = &gFB[yy * gW];
        for (int xx = x0; xx < x1; xx++) {
            int dx_l = xx - x;
            int dx_r = (x + w - 1) - xx;
            int cx = (dx_l < rad) ? (rad - dx_l) : (dx_r < rad ? (rad - dx_r) : 0);
            int d_edge;
            if (cx > 0 && cy > 0) {
                /* Signed-ish distance from rounded corner */
                uint32_t dist = isqrt_u32((uint32_t)(cx * cx + cy * cy));
                if ((int)dist > rad) continue;
                d_edge = rad - (int)dist;
            } else {
                int m = dx_l;
                if (dx_r < m) m = dx_r;
                if (dy_top < m) m = dy_top;
                if (dy_bot < m) m = dy_bot;
                d_edge = m;
            }
            if (bw > 0 && d_edge < bw && border_a > 0) {
                row[xx] = blend(row[xx], border, border_a);
            } else if (fill_a > 0) {
                row[xx] = blend(row[xx], fill, fill_a);
            }
        }
    }
}

typedef struct {
    int line_h;
    const Glyph *glyphs;
    const uint8_t *bitmap;
} FontRef;

static const FontRef F_HDR   = { FONT_HDR_H, font_hdr_glyphs, font_hdr_bitmap };
static const FontRef F_TITLE = { FONT_TITLE_H, font_title_glyphs, font_title_bitmap };
static const FontRef F_SUB   = { FONT_SUB_H, font_sub_glyphs, font_sub_bitmap };
static const FontRef F_BADGE = { FONT_BADGE_H, font_badge_glyphs, font_badge_bitmap };

static int text_width(const FontRef *f, const char *s) {
    int w = 0;
    while (*s) {
        uint8_t c = (uint8_t)*s++;
        if (c >= 32 && c < 127) w += f->glyphs[c - 32].adv;
    }
    return w;
}

static void draw_text(const FontRef *f, int x, int y, uint32_t col, const char *s) {
    (void)f;
    int cx = x;
    while (*s) {
        uint8_t c = (uint8_t)*s++;
        if (c < 32 || c >= 127) continue;
        const Glyph *g = &f->glyphs[c - 32];
        const uint8_t *bm = &f->bitmap[g->offset];
        for (int row = 0; row < g->h; row++) {
            int py = y + g->yoff + row;
            if ((uint32_t)py >= gH) continue;
            uint32_t *dst = &gFB[py * gW];
            for (int colx = 0; colx < g->w; colx++) {
                int px = cx + g->xoff + colx;
                if ((uint32_t)px >= gW) continue;
                uint8_t a = bm[row * g->w + colx];
                if (a) dst[px] = blend(dst[px], col, a);
            }
        }
        cx += g->adv;
    }
}

static void draw_icon(int x, int y, int sz, const uint32_t *argb) {
    for (int row = 0; row < sz; row++) {
        int py = y + row;
        if ((uint32_t)py >= gH) continue;
        uint32_t *dst = &gFB[py * gW];
        const uint32_t *src = &argb[row * sz];
        for (int col = 0; col < sz; col++) {
            int px = x + col;
            if ((uint32_t)px >= gW) continue;
            uint32_t p = src[col];
            uint8_t a = (uint8_t)(p >> 24);
            if (a) dst[px] = blend(dst[px], p, a);
        }
    }
}

/* Smooth sine approximation in [-1000, +1000] for x_deg in [0..360] */
static int isin_deg(int deg) {
    deg %= 360;
    if (deg < 0) deg += 360;
    int sign = 1;
    if (deg >= 180) { deg -= 180; sign = -1; }
    /* Bhaskara I approximation: sin(d) ~= 4*d*(180-d) / (40500 - d*(180-d)) */
    int p = deg * (180 - deg);
    int num = 4000 * p;
    int den = 40500 - p;
    return sign * (num / den);
}

/* Smooth wave backdrop matching LaynCorp bg-1080p palette */
static void build_backdrop(void) {
    int cx = (int)(gW / 2);
    int cy = (int)(gH / 2);
    int maxr = (int)isqrt_u32((uint32_t)(cx * cx + cy * cy));
    if (maxr < 1) maxr = 1;

    for (uint32_t y = 0; y < gH; y++) {
        uint32_t *dst = &gBG[y * gW];
        int dy = (int)y - cy;
        for (uint32_t x = 0; x < gW; x++) {
            int dx = (int)x - cx;
            int r2 = dx * dx + (dy * 5 / 4) * (dy * 5 / 4);
            int d = (int)isqrt_u32((uint32_t)r2);
            int t = 255 - (d * 255) / maxr;
            if (t < 0) t = 0;

            int base_r = 8 + (t * 7) / 255;
            int base_g = 14 + (t * 22) / 255;
            int base_b = 18 + (t * 28) / 255;

            int s1 = isin_deg((int)((x * 280) / gW) + 20);
            int s2 = isin_deg((int)((x * 220) / gW) + 140);
            int wave_y1 = (int)((gH * 78) / 100) + (s1 * (int)gH) / 9500 - (int)((x * gH) / (gW * 6));
            int wave_y2 = (int)((gH * 88) / 100) + (s2 * (int)gH) / 11000 - (int)((x * gH) / (gW * 7));

            if ((int)y > wave_y1) {
                base_r += 4; base_g += 14; base_b += 18;
            }
            if ((int)y > wave_y2) {
                base_r += 3; base_g += 11; base_b += 14;
            }
            if ((int)y == wave_y1 || (int)y == wave_y1 + 1) {
                base_r += 6; base_g += 20; base_b += 24;
            }

            if (base_r > 255) base_r = 255;
            if (base_g > 255) base_g = 255;
            if (base_b > 255) base_b = 255;
            dst[x] = rgb((uint8_t)base_r, (uint8_t)base_g, (uint8_t)base_b);
        }
    }
}

typedef struct {
    const char *title;
    const char *sub;
    const char *desc;
    const char *keyhint;
    const uint32_t *icon;
    uint32_t accent;
    uint8_t code; /* Returned as EFI_ERROR | code */
} MenuItem;

static const MenuItem ITEMS[10] = {
    { "Arch Linux",   "Rolling Live",   "Arch Linux Live ISO via HTTPS mirror (geo.mirror.pkgbuild.com)", "1", icon_arch,    0x38bdf8u, 1 },
    { "Alpine Linux", "RAM Rootfs",     "Alpine Linux Stable netboot direto na memoria RAM",               "2", icon_alpine,  0x2dd4bfu, 2 },
    { "Debian",       "Stable Netboot", "Instalador oficial Debian GNU/Linux Stable (amd64)",              "3", icon_debian,  0xfb7185u, 3 },
    { "Fedora 44",    "Netinstall",     "Fedora Linux 44 Network Installer via HTTPS/HTTP",                "4", icon_fedora,  0x60a5fau, 4 },
    { "Windows",      "Sem Pendrive",   "Instalacao Windows 10/11 via rede (wimboot / WinPE)",             "5", icon_windows, 0x00a4efu, 5 },
    { "Memtest86+",   "Teste de RAM",   "Memtest86+ UEFI x86_64 diagnostico completo de memoria",          "6", icon_memtest, 0x34d399u, 6 },
    { "netboot.xyz",  "Hub ISO Online", "Suite completa netboot.xyz com dezenas de ISOs e ferramentas",    "7", icon_netboot, 0xa78bfau, 7 },
    { "Telemetria",   "Rede & BIOS",    "Inspecionar IP, MAC, gateway, DNS e plataforma UEFI",             "I", icon_info,    0xfbbf24u, 8 },
    { "Disco Local",  "NVMe / SATA",    "Inicializar sistema instalado no disco local / saida UEFI",       "X", icon_local,   0x38bdf8u, 9 },
    { "Shell iPXE",   "Modo Resgate",   "Abrir linha de comando interativa iPXE para diagnostico",         "S", icon_shell,   0x94a3b8u, 10 },
};

static int gWinX, gWinY, gWinW, gWinH;
static int gCardX[10], gCardY[10], gCardW, gCardH;

static void compute_layout(void) {
    gWinW = (gW >= 1400) ? 1240 : ((int)gW - 80);
    gWinH = (gH >= 860) ? 720 : ((int)gH - 80);
    gWinX = ((int)gW - gWinW) / 2;
    gWinY = ((int)gH - gWinH) / 2;

    int padX = 44;
    int gapX = 20;
    int gapY = 20;
    gCardW = (gWinW - padX * 2 - gapX * 4) / 5;
    gCardH = (gWinH - 220 - gapY) / 2;
    if (gCardH > 218) gCardH = 218;
    int startX = gWinX + (gWinW - (gCardW * 5 + gapX * 4)) / 2;
    int startY = gWinY + 122;

    for (int i = 0; i < 10; i++) {
        int r = i / 5;
        int c = i % 5;
        gCardX[i] = startX + c * (gCardW + gapX);
        gCardY[i] = startY + r * (gCardH + gapY);
    }
}

static void draw_cursor(int mx, int my) {
    static const uint8_t arrow[14][10] = {
        {1,0,0,0,0,0,0,0,0,0},
        {1,1,0,0,0,0,0,0,0,0},
        {1,2,1,0,0,0,0,0,0,0},
        {1,2,2,1,0,0,0,0,0,0},
        {1,2,2,2,1,0,0,0,0,0},
        {1,2,2,2,2,1,0,0,0,0},
        {1,2,2,2,2,2,1,0,0,0},
        {1,2,2,2,2,2,2,1,0,0},
        {1,2,2,2,2,2,2,2,1,0},
        {1,2,2,2,2,1,1,1,1,1},
        {1,2,1,2,2,1,0,0,0,0},
        {1,1,0,1,2,2,1,0,0,0},
        {0,0,0,1,2,2,1,0,0,0},
        {0,0,0,0,1,1,0,0,0,0},
    };
    for (int y = 0; y < 14; y++) {
        for (int x = 0; x < 10; x++) {
            uint8_t v = arrow[y][x];
            if (v == 1) put_px(mx + x, my + y, 0x060b0fu, 255);
            else if (v == 2) put_px(mx + x, my + y, 0x36d1dcu, 255);
        }
    }
}

static void format_timer(char *buf, int sec) {
    /* "Auto-boot: Disco Local em 25s" */
    const char *pre = "Auto-boot em ";
    int i = 0;
    while (pre[i]) { buf[i] = pre[i]; i++; }
    if (sec >= 10) buf[i++] = (char)('0' + (sec / 10));
    buf[i++] = (char)('0' + (sec % 10));
    buf[i++] = 's';
    buf[i] = '\0';
}

static void render_scene(int sel, int timeout_sec, int has_mouse, int mx, int my) {
    memcpy(gFB, gBG, (size_t)gW * gH * 4);

    /* Main glass window */
    draw_rrect(gWinX, gWinY, gWinW, gWinH, 16,
               0x0d171fu, 232, 0x1e3f4cu, 255, 2);

    /* Top cyan glow strip inside header */
    fill_rect(gWinX + 24, gWinY + 1, gWinW - 48, 2, 0x36d1dcu, 210);

    /* Header bar */
    fill_rect(gWinX + 2, gWinY + 2, gWinW - 4, 62, 0x11202bu, 200);
    fill_rect(gWinX + 2, gWinY + 64, gWinW - 4, 1, 0x1d3b47u, 255);

    draw_icon(gWinX + 28, gWinY + 19, 28, icon_brand);
    draw_text(&F_HDR, gWinX + 68, gWinY + 22, 0xe2f1f5u, "LAYNCORP");
    int bw = text_width(&F_HDR, "LAYNCORP");
    draw_text(&F_HDR, gWinX + 68 + bw + 8, gWinY + 22, 0x36d1dcu, "// B75CR501 CLOUD BOOT");

    /* Right pill badge */
    const char *pill = "UEFI x86_64  |  HTTPS";
    int pw = text_width(&F_BADGE, pill) + 28;
    int px = gWinX + gWinW - pw - 26;
    draw_rrect(px, gWinY + 18, pw, 30, 15, 0x0a1c24u, 255, 0x2a6978u, 255, 1);
    draw_text(&F_BADGE, px + 14, gWinY + 25, 0x7ee8f0u, pill);

    /* Subtitle */
    draw_text(&F_SUB, gWinX + 44, gWinY + 84, 0x8fb3c0u,
              "Selecione um sistema operacional ou ferramenta para iniciar pela rede:");

    /* 10 icon cards */
    for (int i = 0; i < 10; i++) {
        int x = gCardX[i];
        int y = gCardY[i];
        int active = (i == sel);
        uint32_t bgc = active ? 0x152b38u : 0x101d26u;
        uint32_t bdc = active ? ITEMS[i].accent : 0x1c3440u;
        int border_w = active ? 2 : 1;

        draw_rrect(x, y, gCardW, gCardH, 14, bgc, 255, bdc, 255, border_w);
        if (active) {
            fill_rect(x + 22, y + 2, gCardW - 44, 3, ITEMS[i].accent, 230);
        }

        /* Shortcut key badge top-left */
        int badge_w = 24;
        draw_rrect(x + 12, y + 12, badge_w, 22, 6,
                   active ? ITEMS[i].accent : 0x162934u, 255, 0, 0, 0);
        int khw = text_width(&F_BADGE, ITEMS[i].keyhint);
        draw_text(&F_BADGE, x + 12 + (badge_w - khw) / 2, y + 15,
                  active ? 0x081319u : 0x7ba1aeu, ITEMS[i].keyhint);

        /* Centered 80x80 icon */
        int ix = x + (gCardW - 80) / 2;
        int iy = y + 34;
        draw_icon(ix, iy, 80, ITEMS[i].icon);

        /* Title & subtitle centered below icon */
        int tw = text_width(&F_TITLE, ITEMS[i].title);
        draw_text(&F_TITLE, x + (gCardW - tw) / 2, y + 126,
                  active ? 0xffffffu : 0xd2e6ecu, ITEMS[i].title);

        int sw = text_width(&F_SUB, ITEMS[i].sub);
        draw_text(&F_SUB, x + (gCardW - sw) / 2, y + 154,
                  active ? ITEMS[i].accent : 0x6f94a3u, ITEMS[i].sub);
    }

    /* Active description strip above footer */
    int descY = gWinY + gWinH - 96;
    draw_rrect(gWinX + 44, descY, gWinW - 88, 34, 8, 0x0a141bu, 220, 0x1b3440u, 255, 1);
    draw_text(&F_BADGE, gWinX + 58, descY + 9, ITEMS[sel].accent, ">");
    draw_text(&F_SUB, gWinX + 76, descY + 9, 0xc3dce4u, ITEMS[sel].desc);

    /* Footer bar */
    fill_rect(gWinX + 2, gWinY + gWinH - 48, gWinW - 4, 1, 0x1a343fu, 255);
    draw_text(&F_SUB, gWinX + 44, gWinY + gWinH - 32, 0x89acb8u,
              "[Setas] Navegar    [Enter] Iniciar    [1-7/I/X/S] Atalho    [R] Reiniciar    [Q] Firmware");

    if (timeout_sec >= 0) {
        char tbuf[32];
        format_timer(tbuf, timeout_sec);
        int ttw = text_width(&F_BADGE, tbuf) + 22;
        int tx = gWinX + gWinW - ttw - 36;
        draw_rrect(tx, gWinY + gWinH - 36, ttw, 24, 12, 0x122933u, 255, 0x2c6d7bu, 255, 1);
        draw_text(&F_BADGE, tx + 11, gWinY + gWinH - 31, 0x7de7f0u, tbuf);
    } else {
        const char *manual = "Modo Manual";
        int mw = text_width(&F_BADGE, manual) + 22;
        int mxp = gWinX + gWinW - mw - 36;
        draw_rrect(mxp, gWinY + gWinH - 36, mw, 24, 12, 0x10222bu, 255, 0x224c58u, 255, 1);
        draw_text(&F_BADGE, mxp + 11, gWinY + gWinH - 31, 0x70a2b0u, manual);
    }

    if (has_mouse) draw_cursor(mx, my);
}

static void blit_full(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop) {
    gop->Blt(gop, (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)gFB, EfiBltBufferToVideo,
             0, 0, 0, 0, gW, gH, gW * 4);
}

static void blit_window(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop) {
    gop->Blt(gop, (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)gFB, EfiBltBufferToVideo,
             gWinX, gWinY, gWinX, gWinY, gWinW, gWinH, gW * 4);
}

static int card_hit(int mx, int my) {
    for (int i = 0; i < 10; i++) {
        if (mx >= gCardX[i] && mx < gCardX[i] + gCardW &&
            my >= gCardY[i] && my < gCardY[i] + gCardH) {
            return i;
        }
    }
    return -1;
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    (void)ImageHandle;
    EFI_BOOT_SERVICES *bs = SystemTable->BootServices;
    bs->SetWatchdogTimer(0, 0, 0, NULL);

    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;
    if (bs->LocateProtocol(&gGopGuid, NULL, (void **)&gop) != EFI_SUCCESS || !gop) {
        return EFI_ERROR_BIT | 10ULL; /* Fallback to shell/classic if no GOP */
    }

    /* Prefer 1920x1080 mode if supported */
    uint32_t curW = gop->Mode->Info->HorizontalResolution;
    uint32_t curH = gop->Mode->Info->VerticalResolution;
    if (curW != 1920 || curH != 1080) {
        uint32_t bestMode = gop->Mode->Mode;
        uint32_t bestArea = curW * curH;
        for (uint32_t m = 0; m < gop->Mode->MaxMode; m++) {
            EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info = NULL;
            UINTN isz = 0;
            if (gop->QueryMode(gop, m, &isz, &info) == EFI_SUCCESS && info) {
                if (info->PixelFormat != PixelBltOnly) {
                    if (info->HorizontalResolution == 1920 && info->VerticalResolution == 1080) {
                        bestMode = m;
                        bs->FreePool(info);
                        break;
                    }
                    uint32_t area = info->HorizontalResolution * info->VerticalResolution;
                    if (area > bestArea) {
                        bestArea = area;
                        bestMode = m;
                    }
                }
                bs->FreePool(info);
            }
        }
        if (bestMode != gop->Mode->Mode) {
            gop->SetMode(gop, bestMode);
        }
    }

    gW = gop->Mode->Info->HorizontalResolution;
    gH = gop->Mode->Info->VerticalResolution;

    size_t buf_bytes = (size_t)gW * gH * 4;
    if (bs->AllocatePool(EfiLoaderData, buf_bytes, (void **)&gFB) != EFI_SUCCESS ||
        bs->AllocatePool(EfiLoaderData, buf_bytes, (void **)&gBG) != EFI_SUCCESS) {
        return EFI_ERROR_BIT | 10ULL;
    }

    build_backdrop();
    compute_layout();

    EFI_SIMPLE_POINTER_PROTOCOL *ptr = NULL;
    int has_mouse = 0;
    if (bs->LocateProtocol(&gPointerGuid, NULL, (void **)&ptr) == EFI_SUCCESS && ptr) {
        ptr->Reset(ptr, FALSE);
        has_mouse = 1;
    }

    /* Drain stale keystrokes */
    EFI_INPUT_KEY key;
    while (SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &key) == EFI_SUCCESS) {}

    int sel = 0;          /* Default highlighted card: Arch Linux */
    int timeout_ms = 25000; /* 25s auto-boot countdown on Disco Local if idle */
    int last_sec = 25;
    int mx = (int)gW / 2, my = (int)gH / 2;
    int prev_btn = 0;

    render_scene(sel, last_sec, has_mouse, mx, my);
    blit_full(gop);

    EFI_STATUS ret_status = EFI_SUCCESS;
    for (;;) {
        int dirty = 0;

        /* Poll keyboard */
        if (SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &key) == EFI_SUCCESS) {
            timeout_ms = -1;
            dirty = 1;

            if (key.ScanCode == 0x01) {        /* Up */
                sel = (sel >= 5) ? (sel - 5) : (sel + 5);
            } else if (key.ScanCode == 0x02) { /* Down */
                sel = (sel < 5) ? (sel + 5) : (sel - 5);
            } else if (key.ScanCode == 0x03) { /* Right */
                sel = (sel + 1) % 10;
            } else if (key.ScanCode == 0x04) { /* Left */
                sel = (sel + 9) % 10;
            } else if (key.ScanCode == 0x17 || key.UnicodeChar == 'q' || key.UnicodeChar == 'Q') {
                ret_status = EFI_SUCCESS; /* Exit to Firmware */
                break;
            } else if (key.UnicodeChar == '\r' || key.UnicodeChar == '\n' || key.UnicodeChar == ' ') {
                ret_status = EFI_ERROR_BIT | (uint64_t)ITEMS[sel].code;
                break;
            } else if (key.UnicodeChar >= '1' && key.UnicodeChar <= '7') {
                sel = (int)(key.UnicodeChar - '1');
                render_scene(sel, -1, has_mouse, mx, my);
                blit_window(gop);
                bs->Stall(120000);
                ret_status = EFI_ERROR_BIT | (uint64_t)ITEMS[sel].code;
                break;
            } else if (key.UnicodeChar == 'i' || key.UnicodeChar == 'I') {
                ret_status = EFI_ERROR_BIT | 8ULL;
                break;
            } else if (key.UnicodeChar == 'x' || key.UnicodeChar == 'X') {
                ret_status = EFI_ERROR_BIT | 9ULL;
                break;
            } else if (key.UnicodeChar == 's' || key.UnicodeChar == 'S') {
                ret_status = EFI_ERROR_BIT | 10ULL;
                break;
            } else if (key.UnicodeChar == 'r' || key.UnicodeChar == 'R') {
                ret_status = EFI_ERROR_BIT | 11ULL; /* Reboot */
                break;
            }
        }

        /* Poll mouse if available */
        if (has_mouse && ptr) {
            EFI_SIMPLE_POINTER_STATE st;
            if (ptr->GetState(ptr, &st) == EFI_SUCCESS) {
                int stepX = st.RelativeMovementX ? (st.RelativeMovementX / 128) : 0;
                int stepY = st.RelativeMovementY ? (st.RelativeMovementY / 128) : 0;
                if (st.RelativeMovementX && stepX == 0) stepX = st.RelativeMovementX > 0 ? 1 : -1;
                if (st.RelativeMovementY && stepY == 0) stepY = st.RelativeMovementY > 0 ? 1 : -1;
                if (stepX != 0 || stepY != 0) {
                    mx += stepX;
                    my += stepY;
                    if (mx < 0) mx = 0;
                    if (my < 0) my = 0;
                    if (mx >= (int)gW) mx = (int)gW - 1;
                    if (my >= (int)gH) my = (int)gH - 1;
                    timeout_ms = -1;
                    int hit = card_hit(mx, my);
                    if (hit >= 0) sel = hit;
                    dirty = 1;
                }
                if (st.LeftButton && !prev_btn) {
                    int hit = card_hit(mx, my);
                    if (hit >= 0) {
                        sel = hit;
                        ret_status = EFI_ERROR_BIT | (uint64_t)ITEMS[sel].code;
                        break;
                    }
                }
                prev_btn = st.LeftButton ? 1 : 0;
            }
        }

        if (timeout_ms > 0) {
            timeout_ms -= 40;
            int sec = (timeout_ms + 999) / 1000;
            if (sec != last_sec) {
                last_sec = sec;
                dirty = 1;
            }
            if (timeout_ms <= 0) {
                ret_status = EFI_ERROR_BIT | 9ULL; /* Timeout -> Disco Local */
                break;
            }
        }

        if (dirty) {
            render_scene(sel, timeout_ms > 0 ? last_sec : -1, has_mouse, mx, my);
            blit_window(gop);
        }
        bs->Stall(40000); /* 40ms frame tick */
    }

    bs->FreePool(gFB);
    bs->FreePool(gBG);
    return ret_status;
}
