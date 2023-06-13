#include "network.h"
#include "self_config.h"
#include "accelerator.h"

using namespace std;

void NoC(pipe_manager* pipes) {
    // from
    static Info_P7 sg_from_core_p7;
    static Info_P9 sg_from_core_p9;
    // to
    static Info_P8 sg_p8_rd, sg_p8_wr;

    pipes->write(&sg_from_core_p7);
    pipes->write(&sg_from_core_p9);

    Hyperx_Network(
        sg_from_core_p7.Update_V_ID, sg_from_core_p7.Update_V_Value,
        sg_from_core_p7.Update_V_DValid,
        sg_from_core_p7.Iteration_End, sg_from_core_p7.Iteration_End_DValid,
        sg_from_core_p7.Iteration_ID,
        sg_from_core_p9.Dest_Core_Full,

        sg_p8_wr.Hy1_Update_V_ID, sg_p8_wr.Hy1_Update_V_Value, sg_p8_wr.Hy1_Update_V_DValid,
        sg_p8_wr.Hy1_Iteration_End, sg_p8_wr.Hy1_Iteration_End_DValid,
        sg_p8_wr.Hy1_Iteration_ID,
        sg_p8_wr.Stage_Full
    );

    pipes->read(&sg_p8_wr);
}
