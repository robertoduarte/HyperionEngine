
#include "..\..\Math\Vec3.hpp"
#include "..\..\Math\Trigonometry.hpp"
#include "..\..\Math\Mat43.hpp"
#include "gamemath.h"

#include "BakuGraphics.hpp"
#include "MikaGraphics.hpp"
#include "TailsGraphics.hpp"
#include "MeshC.hpp"
#include "MeshM.hpp"
#include "MeshTorus.hpp"

static inline size_t LoadTexture(texture_t* textures, uint32_t slot, const picture_t* picture, vdp1_vram_t texture_base)
{
    texture_t* const texture = &textures[slot];

    texture->size = TEXTURE_SIZE(picture->width, picture->height);
    texture->vram_index = TEXTURE_VRAM_INDEX(texture_base);

    scu_dma_transfer(0, (void*)texture_base, picture->data, picture->data_size);
    scu_dma_transfer_wait(0);

    return picture->data_size;
}

static inline void LoadPalette(uint16_t bank_256, uint16_t bank_16, const palette_t* palette)
{
    scu_dma_transfer(0, (void*)VDP2_CRAM_MODE_0_OFFSET(bank_256, bank_16, 0), palette->data, palette->data_size);
    scu_dma_transfer_wait(0);
}

struct Camera : public YaulType<Camera, camera_t>
{
    Vec3 target;
    Vec3 up;
    Vec3 position;

    constexpr Camera() : target(), up(0.0, 1.0, 0.0), position() {}
};

struct GouraudTable : public YaulType<GouraudTable, vdp1_gouraud_table_t>
{
    Color::Rgb1555 vertexA;
    Color::Rgb1555 vertexB;
    Color::Rgb1555 vertexC;
    Color::Rgb1555 vertexD;

    GouraudTable() {}

    GouraudTable(const Color::Rgb1555& commonColor) :
        vertexA(commonColor),
        vertexB(commonColor),
        vertexC(commonColor),
        vertexD(commonColor)
    {
    }

    GouraudTable(
        const Color::Rgb1555& colorA,
        const Color::Rgb1555& colorB,
        const Color::Rgb1555& colorC,
        const Color::Rgb1555& colorD
    ) :
        vertexA(colorA),
        vertexB(colorB),
        vertexC(colorC),
        vertexD(colorD)
    {
    }
};

static inline GouraudTable poolShadingTables[CONFIG_MIC3D_CMDT_COUNT];
static inline GouraudTable poolShadingTables2[512];

static inline vdp1_vram_t texture_base;

static inline texture_t textures[8];

static inline workarea_mic3d_depth_values_t pool_depth_values;
static inline workarea_mic3d_z_values_t pool_z_values;
static inline workarea_mic3d_screen_points_t pool_screen_points;
static inline workarea_mic3d_sort_singles_t pool_sort_singles;
static inline workarea_mic3d_cmdts_t pool_cmdts;
static inline workarea_mic3d_render_matrices_t pool_render_matrices;
static inline workarea_mic3d_light_matrices_t pool_light_matrices;
static inline workarea_mic3d_colors_t pool_colors;
static inline workarea_mic3d_work_t pool_work;

static inline workarea_mic3d_t workarea = {
 &pool_depth_values, &pool_z_values, &pool_screen_points,
 &pool_sort_singles, &pool_cmdts, &pool_render_matrices,
 &pool_light_matrices, &pool_colors, &pool_work
};

static inline sort_list_t sortList[512];

static inline const auto init = []()
{
    dbgio_init();
    dbgio_dev_default_init(DBGIO_DEV_VDP2_ASYNC);
    dbgio_dev_font_load();

    vdp2_tvmd_display_res_set(VDP2_TVMD_INTERLACE_NONE, VDP2_TVMD_HORZ_NORMAL_A, VDP2_TVMD_VERT_224);

    vdp2_scrn_back_color_set(VDP2_VRAM_ADDR(3, 0x01FFFE), RGB1555(1, 0, 3, 15));
    vdp1_sync_interval_set(-1);
    vdp1_env_default_set();
    vdp2_sprite_priority_set(0, 6);
    vdp2_tvmd_display_set();
    vdp2_sync();
    vdp2_sync_wait();

    return true;
}();

int main(void)
{
    vdp1_vram_partitions_t vdp1_vram_partitions;
    vdp1_vram_partitions_get(&vdp1_vram_partitions);
    mic3d_init(&workarea);
    render_sort_depth_set(sortList, 512);
    light_gst_set(poolShadingTables->YaulPtr(), CONFIG_MIC3D_CMDT_COUNT, (vdp1_vram_t)(vdp1_vram_partitions.gouraud_base + 512));

    // tlist_set(textures, 8);
    // texture_base = (vdp1_vram_t)vdp1_vram_partitions.texture_base;
    // texture_base += LoadTexture(textures, 0, &picture_mika, texture_base);
    // texture_base += LoadTexture(textures, 1, &picture_tails, texture_base);
    // texture_base += LoadTexture(textures, 2, &picture_baku, texture_base);

    // LoadPalette(0, 0, &palette_baku);

    Camera camera;
    camera_lookat(camera.YaulConstPtr());

    /* Set up a command table for insertion */
    vdp1_cmdt_t cmdt_polygon;
    vdp1_cmdt_polygon_set(&cmdt_polygon);
    vdp1_cmdt_draw_mode_t polygon_draw_mode;
    polygon_draw_mode.raw = 0x0000;
    vdp1_cmdt_draw_mode_set(&cmdt_polygon, polygon_draw_mode);

    Fxp theta;

    while (true)
    {
        dbgio_printf("[H[2J");

        render_start();

        /* Rotate a 2D quad */
        constexpr Vec3 points[] = { {-45.0, -45.0, 0.0}, {45.0, -45.0, 0.0}, {45.0, 45.0, 0.0}, {-45.0, 45.0, 0.0} };

        Mat33 rotatedMatrix = Mat33::Identity();
        rotatedMatrix.RotateZ(theta).RotateX(theta).RotateY(theta);
        rotatedMatrix.PrintDebug();

        for (uint32_t i = 0; i < 4; i++)
        {
            Vec3 rotation = rotatedMatrix * points[i];
            cmdt_polygon.cmd_vertices[i].x = rotation.x.Value() >> 16;
            cmdt_polygon.cmd_vertices[i].y = rotation.y.Value() >> 16;
        }

        cmdt_polygon.cmd_colr = 0x8010;

        render_cmdt_insert(&cmdt_polygon, (fix16_t)Fxp(-1.0).Value());

        theta += 0.0005;

        render_end();

        vdp1_sync_render();
        vdp1_sync();
        vdp1_sync_wait();

        dbgio_flush();
    }
}
