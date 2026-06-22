#include "Manage.h"

typedef struct {
    void (*Init)(void);
    void (*Loop)(void);
    void (*Exit)(void);
} Manage_t;


static Manage_t Manage_List[]={
    // {Task_1_Init(), Task_1_Loop(), Task_1_Exit()}
};

typedef struct {
    uint8_t CurMode;
    uint8_t NextMode;
}Mode_t;

static Mode_t Mode = {0, 0};

#define MANAGE_LIST_SIZE (sizeof(Manage_List) / sizeof(Manage_t))

void Manage_Loop(void)
{
    if(Mode.CurMode == Mode.NextMode)
    {
        Manage_List[Mode.CurMode].Loop();
    }
    else
    {
        Manage_List[Mode.CurMode].Exit();
        Manage_List[Mode.NextMode].Init();
        Mode.CurMode = Mode.NextMode;
    }
}
