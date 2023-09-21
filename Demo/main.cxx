
#include "yaul.h"
#include "..\ECS\World.hpp"

static inline const auto init = []()
{
    dbgio_init();
    dbgio_dev_default_init(DBGIO_DEV_VDP2_ASYNC);
    dbgio_dev_font_load();

    vdp2_tvmd_display_res_set(VDP2_TVMD_INTERLACE_NONE, VDP2_TVMD_HORZ_NORMAL_A,
        VDP2_TVMD_VERT_224);

    vdp2_scrn_back_color_set(VDP2_VRAM_ADDR(3, 0x01FFFE),
        RGB1555(1, 0, 3, 3));

    vdp2_tvmd_display_set();
    return true;
}();

struct Position
{
    int x, y;
};

struct Velocity
{
    int x, y, z;
};

void main()
{
    using namespace Hyperion::ECS;

    EntityReference entityA = World::CreateEntity<Position, Velocity>();

    EntityReference entityB = entityA;

    World::CreateEntity<Velocity, Position>();
    World::CreateEntity([](Position* p)
    {
        p->x = 1;
        p->y = 2;
    });

    EntityReference entityC = World::CreateEntity([](Velocity* v)
    {
        v->x = 1;
        v->y = 2;
        v->z = 3;
    });

    entityB.Destroy();

    while (1)
    {
        World::ForEachEntity([](Position* p)
        {
            p->x += 2;
            p->y += 2;
            dbgio_printf("Position x:%d y:%d\n", p->x, p->y);
        });

        dbgio_flush();
        vdp2_sync();
        vdp2_sync_wait();
    }
}
