#include "common.h"
#include "gui/GraphicsContext.h"
#include "gui/PaletteManager.h"
#include "gui/font/nitroFont2.h"
#include "core/math/RgbMixer.h"
#include "gui/OamBuilder.h"
#include "largeFolderIcon.h"
#include "gui/palette/GradientPalette.h"
#include "themes/IFontRepository.h"
#include "MaterialFileIcon.h"

MaterialFileIcon::MaterialFileIcon(const TCHAR* name, const MaterialColorScheme* materialColorScheme,
    const IFontRepository* fontRepository)
    : _materialColorScheme(materialColorScheme), _fontRepository(fontRepository)
{
    // Decode up to three UTF-8 characters, so Chinese folder and file names
    // show their first Han characters in the Material grid icon instead of the
    // raw bytes.
    const unsigned char* ptr = (const unsigned char*)name;
    int i = 0;
    while (i < 3 && *ptr)
    {
        u16 c;
        if ((*ptr & 0x80) == 0)
        {
            c = *ptr++;
        }
        else if ((*ptr & 0xE0) == 0xC0 && ptr[1] != 0)
        {
            c = ((*ptr & 0x1F) << 6) | (ptr[1] & 0x3F);
            ptr += 2;
        }
        else if ((*ptr & 0xF0) == 0xE0 && ptr[1] != 0 && ptr[2] != 0)
        {
            c = ((*ptr & 0x0F) << 12) | ((ptr[1] & 0x3F) << 6) | (ptr[2] & 0x3F);
            ptr += 3;
        }
        else
        {
            c = '?';
            ptr++;
        }
        _displayName[i++] = c;
    }
    _displayName[i] = 0;
}

void MaterialFileIcon::UploadGraphics()
{
    if (_vramAddress != nullptr)
    {
        dma_ntrCopy32(3, GetIconTiles(), _vramAddress, 32 * 32 / 2);

        auto font = _fontRepository->GetFont(FontType::Medium11);
        u8 tileBuffer[32 * 16 / 2];
        memset(tileBuffer, 0, sizeof(tileBuffer));
        u32 textWidth, textHeight;
        nft2_measureString(font, _displayName, textWidth, textHeight);
        nft2_string_render_params_t renderParams;
        renderParams.x = ((int)32 - (int)textWidth) / 2;
        renderParams.y = 0;
        renderParams.width = 32;
        renderParams.height = 16;
        renderParams.a5i3 = false;
        nft2_renderString(font, _displayName, tileBuffer, 32, &renderParams);
        memcpy((u8*)_vramAddress + largeFolderIconTilesLen, tileBuffer, sizeof(tileBuffer));
    }
}

void MaterialFileIcon::Draw(GraphicsContext& graphicsContext, const Rgb<8, 8, 8>& backgroundColor)
{
    auto iconColor = GetIconColor();
    auto nameColor = GetTextColor();

    auto oams = graphicsContext.GetOamManager().AllocOams(2);

    u32 iconPaletteRow = graphicsContext.GetPaletteManager().AllocRow(
        GradientPalette(backgroundColor, iconColor), _position.y, _position.y + 32);
    OamBuilder::OamWithSize<32, 32>(_position.x, _position.y, _vramOffset >> 7)
        .WithPalette16(iconPaletteRow)
        .WithPriority(graphicsContext.GetPriority())
        .Build(oams[1]);

    u32 namePaletteRow = graphicsContext.GetPaletteManager().AllocRow(
        GradientPalette(iconColor, nameColor), _position.y, _position.y + 32);
    OamBuilder::OamWithSize<32, 16>(_position.x, _position.y + GetTextYOffset(), (_vramOffset + largeFolderIconTilesLen) >> 7)
        .WithPalette16(namePaletteRow)
        .WithPriority(graphicsContext.GetPriority())
        .Build(oams[0]);
}