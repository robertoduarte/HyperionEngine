#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"

#include  "..\Yaul\Yaul.hpp"

namespace Hyperion::GL
{
    namespace Color
    {
        struct Bank
        {
            struct Type0
            {
                uint16_t pr : 2;  /* Priority bit(s) */
                uint16_t cc : 3;  /* Color calculation ratio bit(s) */
                uint16_t dc : 11; /* Dot color data bits */
            };

            struct Type1
            {
                uint16_t pr : 3;  /* Priority bit(s) */
                uint16_t cc : 2;  /* Color calculation ratio bit(s) */
                uint16_t dc : 11; /* Dot color data bits */
            };

            struct Type2
            {
                uint16_t sdw : 1; /* Shadow (or sprite window) bit */
                uint16_t pr : 1;  /* Priority bit(s) */
                uint16_t cc : 3;  /* Color calculation ratio bit(s) */
                uint16_t dc : 11; /* Dot color data bits */
            };

            struct Type3
            {
                uint16_t sdw : 1; /* Shadow (or sprite window) bit */
                uint16_t pr : 2;  /* Priority bit(s) */
                uint16_t cc : 2;  /* Color calculation ratio bit(s) */
                uint16_t dc : 11; /* Dot color data bits */
            };

            struct Type4
            {
                uint16_t sdw : 1; /* Shadow (or sprite window) bit */
                uint16_t pr : 2;  /* Priority bit(s) */
                uint16_t cc : 3;  /* Color calculation ratio bit(s) */
                uint16_t dc : 10; /* Dot color data bits */
            };

            struct Type5
            {
                uint16_t sdw : 1; /* Shadow (or sprite window) bit */
                uint16_t pr : 3;  /* Priority bit(s) */
                uint16_t cc : 1;  /* Color calculation ratio bit(s) */
                uint16_t dc : 11; /* Dot color data bits */
            };

            struct Type6
            {
                uint16_t sdw : 1; /* Shadow (or sprite window) bit */
                uint16_t pr : 3;  /* Priority bit(s) */
                uint16_t cc : 2;  /* Color calculation ratio bit(s) */
                uint16_t dc : 10; /* Dot color data bits */
            };

            struct Type7
            {
                uint16_t sdw : 1; /* Shadow (or sprite window) bit */
                uint16_t pr : 3;  /* Priority bit(s) */
                uint16_t cc : 3;  /* Color calculation ratio bit(s) */
                uint16_t dc : 9;  /* Dot color data bits */
            };

            struct Type8
            {
                uint16_t pr : 1; /* Priority bit(s) */
                uint16_t dc : 7; /* Dot color data bits */
                uint16_t : 8; /* Padding*/
            };

            struct Type9
            {
                uint16_t pr : 1; /* Priority bit(s) */
                uint16_t cc : 1; /* Color calculation ratio bit(s) */
                uint16_t dc : 6; /* Dot color data bits */
                uint16_t : 8; /* Padding*/
            };

            struct TypeA
            {
                uint16_t pr : 2; /* Priority bit(s) */
                uint16_t dc : 6; /* Dot color data bits */
                uint16_t : 8; /* Padding*/
            };

            struct TypeB
            {
                uint16_t cc : 2; /* Color calculation ratio bit(s) */
                uint16_t dc : 6; /* Dot color data bits */
                uint16_t : 8; /* Padding*/
            };

            struct TypeC
            {
                uint16_t sp : 1; /* Priority, CRAM addr. shared bit(s) */
                uint16_t dc : 7; /* Dot color data bits */
                uint16_t : 8; /* Padding*/
            };

            struct TypeD
            {
                uint16_t sp : 1; /* Priority, CRAM addr. shared bit(s) */
                uint16_t sc : 1; /* Color calculation ratio, CRAM addr. shared bit(s) */
                uint16_t dc : 6; /* Dot color data bits */
                uint16_t : 8; /* Padding*/
            };

            struct TypeE
            {
                uint16_t sp : 2; /* Priority, CRAM addr. shared bit(s) */
                uint16_t dc : 6; /* Dot color data bits */
                uint16_t : 8; /* Padding*/
            };

            struct TypeF
            {
                uint16_t sc : 2; /* Color calculation ratio, CRAM addr. shared bit(s) */
                uint16_t dc : 6; /* Dot color data bits */
                uint16_t : 8; /* Padding*/
            };

            union
            {
                Type0 type0;
                Type1 type1;
                Type2 type2;
                Type3 type3;
                Type4 type4;
                Type5 type5;
                Type6 type6;
                Type7 type7;
                Type8 type8;
                Type9 type9;
                TypeA typeA;
                TypeB typeB;
                TypeC typeC;
                TypeD typeD;
                TypeE typeE;
                TypeF typeF;
            };

            constexpr Bank(const Type0& type) : type0(type) {}
            constexpr Bank(const Type1& type) : type1(type) {}
            constexpr Bank(const Type2& type) : type2(type) {}
            constexpr Bank(const Type3& type) : type3(type) {}
            constexpr Bank(const Type4& type) : type4(type) {}
            constexpr Bank(const Type5& type) : type5(type) {}
            constexpr Bank(const Type6& type) : type6(type) {}
            constexpr Bank(const Type7& type) : type7(type) {}
            constexpr Bank(const Type8& type) : type8(type) {}
            constexpr Bank(const Type9& type) : type9(type) {}
            constexpr Bank(const TypeA& type) : typeA(type) {}
            constexpr Bank(const TypeB& type) : typeB(type) {}
            constexpr Bank(const TypeC& type) : typeC(type) {}
            constexpr Bank(const TypeD& type) : typeD(type) {}
            constexpr Bank(const TypeE& type) : typeE(type) {}
            constexpr Bank(const TypeF& type) : typeF(type) {}
        };

        struct Rgb1555
        {
            uint16_t msb : 1;
            uint16_t blue : 5;
            uint16_t green : 5;
            uint16_t red : 5;

            constexpr Rgb1555() : msb(0), blue(0), green(0), red(0) {}

            constexpr Rgb1555(const uint16_t& red, const uint16_t& green, const uint16_t& blue, const bool& mSB)
                : msb(mSB), blue(blue), green(green), red(red)
            {
            }
        };

        struct Lut
        {
            Bank entries[16];
            Rgb1555 colors[16];
        };

        struct PalleteData
        {
            union
            {
                Bank colorBank;
                Rgb1555 baseColor;
                uint16_t vramIndex;
            };

            constexpr PalleteData(const Bank& colorBank) :colorBank(colorBank) {}
            constexpr PalleteData(const Rgb1555& baseColor) : baseColor(baseColor) {}
            constexpr PalleteData(const uint16_t& vramIndex) : vramIndex(vramIndex) {}
        };
    };

    struct Control
    {
        enum class LinkType : uint16_t
        {
            JumpNext = 0,
            JumpAssign = 1,
            JumpCall = 2,
            JumpReturn = 3,
            SkipNext = 4,
            SkipAssign = 5,
            SkipCall = 6,
            SkipReturn = 7
        };

        enum class ReadDirection : uint16_t
        {
            Normal = 0,
            Horizontal = 1,
            Vertical = 2,
            HorizontalVertical = 3
        };

        enum class CommandType : uint16_t
        {
            Sprite = 0,
            ScaledSprite = 1,
            DistortedSprite = 2,
            Polygon = 4,
            Polyline = 5,
            Line = 6
        };

        uint16_t : 1;
        LinkType linkType : 3;
        uint16_t : 6;
        ReadDirection readDirection : 2;
        CommandType commandType : 4;

        constexpr Control(const LinkType& linkType, const CommandType& commandType) :
            linkType(linkType), readDirection(ReadDirection::Normal), commandType(commandType)
        {
        }

        constexpr Control(const LinkType& linkType, const ReadDirection& readDirection, const CommandType& commandType) :
            linkType(linkType), readDirection(readDirection), commandType(commandType)
        {
        }
    };

    struct DrawMode : public YaulType<DrawMode, vdp1_cmdt_draw_mode_t>
    {
        enum class MsbEnable : uint16_t
        {
            False = 0,
            True = 1
        };

        enum class HssEnable : uint16_t
        {
            False = 0,
            True = 1
        };

        enum class PreClippingDisable : uint16_t
        {
            False = 0,
            True = 1
        };

        enum class UserClippingMode : uint16_t
        {
            Mode0 = 0,
            Mode1 = 1,
            Mode2 = 2,
            Mode3 = 3
        };

        enum class MeshEnable : uint16_t
        {
            False = 0,
            True = 1
        };

        enum class EndCodeDisable : uint16_t
        {
            False = 0,
            True = 1
        };

        enum class TransPixelDisable : uint16_t
        {
            False = 0,
            True = 1
        };

        enum class CombinationMode : uint16_t
        {
            Replace = 0,
            Shadow = 1,
            HalfLuminance = 2,
            HalfTransparent = 3,
            Gouraud = 4
        };

        enum class ColorMode : uint16_t
        {
            CB16 = 0,
            CLUT16 = 1,
            CB64 = 2,
            CB128 = 3,
            CB256 = 4,
            RGB32768 = 5
        };

        MsbEnable msbEnable : 1;
        uint16_t : 2;
        HssEnable hssEnable : 1;
        PreClippingDisable preClippingDisable : 1;
        UserClippingMode userClippingMode : 2;
        MeshEnable meshEnable : 1;
        EndCodeDisable endCodeDisable : 1;
        TransPixelDisable transPixelDisable : 1;
        CombinationMode combinationMode : 3;
        ColorMode colorMode : 3;

        constexpr DrawMode() :
            msbEnable(MsbEnable::False),
            hssEnable(HssEnable::False),
            preClippingDisable(PreClippingDisable::False),
            userClippingMode(UserClippingMode::Mode0),
            meshEnable(MeshEnable::False),
            endCodeDisable(EndCodeDisable::False),
            transPixelDisable(TransPixelDisable::False),
            combinationMode(CombinationMode::Replace),
            colorMode(ColorMode::CB16)
        {
        }

        constexpr DrawMode(const EndCodeDisable& endCodeDisable, const TransPixelDisable& transPixelDisable) :
            msbEnable(MsbEnable::False),
            hssEnable(HssEnable::False),
            preClippingDisable(PreClippingDisable::False),
            userClippingMode(UserClippingMode::Mode0),
            meshEnable(MeshEnable::False),
            endCodeDisable(endCodeDisable),
            transPixelDisable(transPixelDisable),
            combinationMode(CombinationMode::Gouraud),
            colorMode(ColorMode::CB16)
        {
        }

        constexpr DrawMode(const EndCodeDisable& endCodeDisable, const TransPixelDisable& transPixelDisable, const ColorMode& colorMode) :
            msbEnable(MsbEnable::False),
            hssEnable(HssEnable::False),
            preClippingDisable(PreClippingDisable::False),
            userClippingMode(UserClippingMode::Mode0),
            meshEnable(MeshEnable::False),
            endCodeDisable(endCodeDisable),
            transPixelDisable(transPixelDisable),
            combinationMode(CombinationMode::Gouraud),
            colorMode(colorMode)
        {
        }
    };

    struct Attribute
    {
        Control control;
        DrawMode drawMode;
        Color::PalleteData palleteData;
        uint16_t textureSlot;
        uint16_t shadingSlot;

        constexpr Attribute
        (
            const Control& control,
            const DrawMode& drawMode,
            const Color::PalleteData& palleteData,
            const uint16_t& textureSlot,
            const uint16_t& shadingSlot
        ) :
            control(control),
            drawMode(drawMode),
            palleteData(palleteData),
            textureSlot(textureSlot),
            shadingSlot(shadingSlot)
        {
        }

        constexpr Attribute(const Control& control, const DrawMode& drawMode, const Color::PalleteData& palleteData) :
            control(control), drawMode(drawMode), palleteData(palleteData), textureSlot(0), shadingSlot(0)
        {
        }

        constexpr Attribute(const Control& control, const Color::PalleteData& palleteData) :
            control(control), drawMode(), palleteData(palleteData), textureSlot(0), shadingSlot(0)
        {
        }
    };
}

#pragma GCC diagnostic pop
