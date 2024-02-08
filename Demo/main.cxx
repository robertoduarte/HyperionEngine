
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
    int x = 0, y = 0;
};

struct Velocity
{
    int x = 0, y = 0, z = 0;
};

void main()
{
    using namespace Hyperion::ECS;

    EntityReference entityA = World::CreateEntity<Position, Velocity>();
    EntityReference entityB = entityA;

    World::CreateEntity([](Position* p, Velocity* v)
    {
        p->x = 1;
        p->y = 2;

        v->x = 1;
        v->y = 2;
        v->z = 3;
    });

    entityB.Destroy();

    World::CreateEntity<Velocity, Position>();
    World::CreateEntity<Velocity, Position>();

    struct ExampleSystem : World::EntityIterator
    {
        int classVariable = 2;
        void CustomFunction()
        {
            Iterate([this](Position* p)
            {
                if (p->x < 7)
                {
                    classVariable += p->y;
                    dbgio_printf("Yay from System!\n");
                    dbgio_printf("Printing class variable: %d\n", classVariable);
                    StopIteration();
                }
            });
        }
    };

    while (1)
    {
        World::EntityIterator ei;

        ei.Iterate([](Position* p)
        {
            p->x += 2;
            p->y += 2;
        });

        ei.Iterate([](Position* p, Velocity* v)
        {
            p->x += 2;
            p->y += 2;
            v->x = p->y;
            dbgio_printf("Position x:%d y:%d\n", p->x, p->y);
            dbgio_printf("Velocity x:%d y:%d z:%d\n", v->x, v->y, v->z);
        });

        ei.Iterate([&ei](Position* p)
        {
            if (p->x < 9)
            {
                dbgio_printf("Position x:%d y:%d\n", p->x, p->y);
                dbgio_printf("Yay from Lambda!\n");
                ei.StopIteration();
            }
        });

        ExampleSystem ss;
        ss.CustomFunction();

        dbgio_flush();
        vdp2_sync();
        vdp2_sync_wait();
    }
}
