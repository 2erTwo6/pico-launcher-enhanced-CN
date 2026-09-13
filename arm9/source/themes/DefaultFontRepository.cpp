#include "common.h"
#include "BitmapSong-9pt_nft2.h"
#include "DefaultFontRepository.h"

const nft2_header_t* DefaultFontRepository::GetFont(FontType fontType) const
{
    switch (fontType)
    {
        case FontType::Regular10:
        {
            return (const nft2_header_t*)BitmapSong_9pt_nft2;
        }
        case FontType::Medium7_5:
        {
            return (const nft2_header_t*)BitmapSong_9pt_nft2;
        }
        case FontType::Medium10:
        {
            return (const nft2_header_t*)BitmapSong_9pt_nft2;
        }
        case FontType::Medium11:
        {
            return (const nft2_header_t*)BitmapSong_9pt_nft2;
        }
        default:
        {
            return nullptr;
        }
    }
}
