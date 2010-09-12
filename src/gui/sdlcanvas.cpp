/*
 * sdlcanvas.cpp
 *
 *  Created on: 12.9.2010
 *      Author: bobo
 */

#include <SDL.h>
#include <SDL_Pango.h>
#include <SDL_gfxPrimitives.h>
#include <glibmm/ustring.h>

#include "utils.h"

#include "gui/sdlcanvas.h"

using namespace Glib;

static SDL_Surface *make_surface (int width, int height) {
    return SDL_CreateRGBSurface (SDL_SWSURFACE, width, height, 32, 0xff,
            0xff00, 0xff0000, 0xff000000);
}

SDLCanvas::SDLCanvas () :
    Canvas (), surface (NULL), pango_context (SDLPango_CreateContext ()),
            font_color (*MATRIX_TRANSPARENT_BACK_BLACK_LETTER) {
}

SDLCanvas::~SDLCanvas () {
    if (surface != NULL) {
        SDL_FreeSurface (surface);
        surface = NULL;
    }
    SDLPango_FreeContext (pango_context);
}

void SDLCanvas::set_width (int value) {
    if (width != value) {
        if (surface != NULL) {
            SDL_FreeSurface (surface);
            surface = NULL;
        }

        width = value;

        if (width > 0 && height > 0) {
            surface = make_surface (width, height);
        }
    }
}

void SDLCanvas::set_height (int value) {
    if (height != value) {
        if (surface != NULL) {
            SDL_FreeSurface (surface);
            surface = NULL;
        }

        height = value;

        if (width > 0 && height > 0) {
            surface = make_surface (width, height);
        }
    }
}

void SDLCanvas::set_font (const ustring& value) {
    SDLPango_FreeContext (pango_context);
    font.name = value;
    ustring desc = value + " " + to_string<int> (font.size) + "px";
    pango_context = SDLPango_CreateContext_GivenFontDesc (desc.c_str ());
    SDLPango_SetDefaultColor (pango_context, &font_color);
}

void SDLCanvas::set_font_color (Uint32 value) {
    font.color = value;
    font_color.m[0][1] = 0xff & (value >> 24);
    font_color.m[1][1] = 0xff & (value >> 16);
    font_color.m[2][1] = 0xff & (value >> 8);
    font_color.m[3][1] = 0xff;
    font_color.m[0][0] = 0xff & (value >> 24);
    font_color.m[1][0] = 0xff & (value >> 16);
    font_color.m[2][0] = 0xff & (value >> 8);
    font_color.m[3][0] = 0x00;
    SDLPango_SetDefaultColor (pango_context, &font_color);
}

void SDLCanvas::set_font_size (int value) {
    SDLPango_FreeContext (pango_context);
    font.size = value;
    ustring desc = font.name + " " + to_string<int> (value) + "px";
    pango_context = SDLPango_CreateContext_GivenFontDesc (desc.c_str ());
    SDLPango_SetDefaultColor (pango_context, &font_color);
}

void SDLCanvas::draw_point (int x, int y, Uint32 color) {
    pixelColor (surface, x, y, color);
}

void SDLCanvas::draw_hline (int x, int y, int w, Uint32 color) {
    hlineColor (surface, x, x + w - 1, y - 1, color);
}

void SDLCanvas::draw_vline (int x, int y, int h, Uint32 color) {
    vlineColor (surface, x, y - 1, y + h - 1, color);
}

void SDLCanvas::draw_rectangle (int x, int y, int w, int h, Uint32 color) {
    rectangleColor (surface, x, y, x + w - 1, y + h - 1, color);
}

void SDLCanvas::draw_box (int x, int y, int w, int h, Uint32 color) {
    boxColor (surface, x, y, x + w - 1, y + h - 1, color);
}

void SDLCanvas::draw_line (int x1, int y1, int x2, int y2, Uint32 color) {
    lineColor (surface, x1, y1, x2, y2, color);
}

void SDLCanvas::draw_aaline (int x1, int y1, int x2, int y2, Uint32 color) {
    aalineColor (surface, x1, y1, x2, y2, color);
}

void SDLCanvas::draw_circle (int x, int y, int r, Uint32 color) {
    circleColor (surface, x, y, r, color);
}

void SDLCanvas::draw_arc (int x, int y, int r, int start, int end, Uint32 color) {
    arcColor (surface, x, y, r, start, end, color);
}

void SDLCanvas::draw_filled_circle (int x, int y, int r, Uint32 color) {
    filledCircleColor (surface, x, y, r, color);
}

void SDLCanvas::draw_aacircle (int x, int y, int r, Uint32 color) {
    circleColor (surface, x, y, r, color);
}

void SDLCanvas::draw_trigon (int x1, int y1, int x2, int y2, int x3, int y3,
        Uint32 color) {
    trigonColor (surface, x1, y1, x2, y2, x3, y3, color);
}

void SDLCanvas::draw_filled_trigon (int x1, int y1, int x2, int y2, int x3,
        int y3, Uint32 color) {
    filledTrigonColor (surface, x1, y1, x2, y2, x3, y3, color);
}

void SDLCanvas::draw_aatrigon (int x1, int y1, int x2, int y2, int x3, int y3,
        Uint32 color) {
    aatrigonColor (surface, x1, y1, x2, y2, x3, y3, color);
}

void SDLCanvas::draw_image (int x, int y, Canvas* image) {
    SDL_Rect dest_area;
    dest_area.x = x;
    dest_area.y = y;

    SDLCanvas *sdlimage = dynamic_cast<SDLCanvas*> (image);
    if (sdlimage != NULL) {
        SDL_BlitSurface (sdlimage->surface, NULL, surface, &dest_area);
    }
}

void SDLCanvas::draw_image (int x, int y, Canvas* image, int src_x, int src_y,
        int src_w, int src_h) {
    SDL_Rect src_area;
    src_area.x = src_x;
    src_area.y = src_y;
    src_area.w = src_w;
    src_area.h = src_h;

    SDL_Rect dest_area;
    dest_area.x = x;
    dest_area.y = y;

    SDLCanvas *sdlimage = dynamic_cast<SDLCanvas*> (image);
    if (sdlimage != NULL) {
        SDL_BlitSurface (sdlimage->surface, &src_area, surface, &dest_area);
    }
}

void SDLCanvas::draw_text (int x, int y, int w, int h, HorizontalAling ha,
        VerticalAling va, const ustring& text) {
    SDL_Rect src;
    SDL_Rect dest;

    if (text == "")
        return;

    SDLPango_SetMarkup (pango_context, text.c_str (), -1);
    SDL_Surface* face = SDLPango_CreateSurfaceDraw (pango_context);

    src.w = (face->w < w) ? face->w : w;
    src.h = (face->h < h) ? face->h : h;

    switch (ha) {
    case HA_left:
        src.x = 0;
        dest.x = x;
        break;
    case HA_center:
        src.x = (face->w - src.w) / 2;
        dest.x = x + (w - src.w) / 2;
        break;
    case HA_right:
        src.x = face->w - src.w;
        dest.x = x + w - src.w;
        break;
    }

    switch (va) {
    case VA_top:
        src.y = 0;
        dest.y = y;
        break;
    case VA_center:
        src.y = (face->h - src.h) / 2;
        dest.y = y + (h - src.h) / 2;
        break;
    case VA_bottom:
        src.y = face->h - src.h;
        dest.y = y + h - src.h;
        break;
    }

    SDL_BlitSurface (face, &src, surface, &dest);

    SDL_FreeSurface (face);
}

void SDLCanvas::draw_text (int x, int y, int w, int h, int x_shift,
        VerticalAling va, const ustring& text) {
    SDL_Rect src;
    SDL_Rect dest;

    if (text == "")
        return;

    SDLPango_SetMarkup (pango_context, text.c_str (), -1);
    SDL_Surface* face = SDLPango_CreateSurfaceDraw (pango_context);

    src.w = (face->w - x_shift < w) ? face->w - x_shift : w;
    src.h = (face->h < h) ? face->h : h;

    if (x_shift >= 0) {
        src.x = x_shift;
        dest.x = x;
    } else {
        src.x = 0;
        dest.x = -x_shift;
    }

    switch (va) {
    case VA_top:
        src.y = 0;
        dest.y = y;
        break;
    case VA_center:
        src.y = (face->h - src.h) / 2;
        dest.y = y + (h - src.h) / 2;
        break;
    case VA_bottom:
        src.y = face->h - src.h;
        dest.y = y + h - src.h;
        break;
    }

    SDL_BlitSurface (face, &src, surface, &dest);

    SDL_FreeSurface (face);
}

void SDLCanvas::draw_wrapped_text (int x, int y, int w, int h,
        const ustring& text) {
    SDL_Rect src;
    SDL_Rect dest;

    if (text == "")
        return;

    SDLPango_SetMarkup (pango_context, text.c_str (), -1);
    SDLPango_SetMinimumSize (pango_context, w, h);
    SDL_Surface* face = SDLPango_CreateSurfaceDraw (pango_context);

    src.w = (face->w < w) ? face->w : w;
    src.h = (face->h < h) ? face->h : h;

    src.x = 0;
    dest.x = x;

    src.y = 0;
    dest.y = y;

    SDL_BlitSurface (face, &src, surface, &dest);

    SDL_FreeSurface (face);
}

int SDLCanvas::get_text_width (const ustring& text) {
    SDLPango_SetMarkup (pango_context, text.c_str (), -1);
    return SDLPango_GetLayoutWidth (pango_context);
}

