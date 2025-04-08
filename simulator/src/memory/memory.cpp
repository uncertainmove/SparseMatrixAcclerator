#include "memory.h"
#include "self_config.h"
#include "accelerator.h"
#include "util.h"

extern int clk;
extern int rst_rd;

void Memory(pipe_manager* pipes) {
    // from
    static Info_P2 sg_from_core_p2;
    static Info_P5 sg_from_core_p5;
    static Info_P10 sg_from_core_p10;
    // to
    static Info_MEM sg_mem_rd, sg_mem_wr;

    pipes->write(&sg_from_core_p2);
    pipes->write(&sg_from_core_p5);
    pipes->write(&sg_from_core_p10);
    pipes->write(&sg_mem_rd);

    /*
    if (clk == 464) {
        printf("sg_p10_rd: %d %d %d %d %d %d\n", sg_from_core_p10.Delta1_Bram_Rd_Addr[0],
                sg_from_core_p10.Delta2_Bram_Rd_Addr[0], sg_from_core_p10.PR_Uram_Rd_Addr[0],
                sg_from_core_p10.Delta1_Bram_Rd_Valid[0], sg_from_core_p10.Delta2_Bram_Rd_Valid[0],
                sg_from_core_p10.PR_Uram_Rd_Valid[0]);
    }
    */

    Offset_Uram(
        sg_from_core_p2.RD_Active_V_Offset_Addr, sg_from_core_p2.Active_V_DValid,

        sg_mem_wr.Active_V_LOffset, sg_mem_wr.Active_V_ROffset, sg_mem_wr.Active_V_DValid);

    Delta1_Bram(
        sg_from_core_p10.Delta1_Bram_Rd_Addr, sg_from_core_p10.Delta1_Bram_Rd_Valid,
        sg_from_core_p10.Delta1_Bram_Wr_Addr, sg_from_core_p10.Delta1_Bram_Wr_Value,
        sg_from_core_p10.Delta1_Bram_Wr_Valid,

        sg_mem_wr.Delta1_Bram_Data, sg_mem_wr.Delta1_Bram_Data_Valid);

    Delta2_Bram(
        sg_from_core_p10.Delta2_Bram_Rd_Addr, sg_from_core_p10.Delta2_Bram_Rd_Valid,
        sg_from_core_p10.Delta2_Bram_Wr_Addr, sg_from_core_p10.Delta2_Bram_Wr_Value,
        sg_from_core_p10.Delta2_Bram_Wr_Valid,

        sg_mem_wr.Delta2_Bram_Data, sg_mem_wr.Delta2_Bram_Data_Valid);

    PR_Uram(
        sg_from_core_p10.PR_Uram_Rd_Addr, sg_from_core_p10.PR_Uram_Rd_Valid,
        sg_from_core_p10.PR_Uram_Wr_Addr, sg_from_core_p10.PR_Uram_Wr_Value,
        sg_from_core_p10.PR_Uram_Wr_Valid,

        sg_mem_wr.PR_Uram_Data, sg_mem_wr.PR_Uram_Data_Valid);
    
    // asynchronous FIFO
    HBM_Interface(
        sg_from_core_p5.Rd_HBM_Edge_Addr, sg_from_core_p5.Rd_HBM_Edge_Valid,
        sg_mem_rd.HBM_Controller_Edge, sg_mem_rd.HBM_Controller_DValid,
        sg_mem_rd.HBM_Controller_Full,

        sg_mem_wr.Stage_Full,
        sg_mem_wr.Rd_HBM_Edge_Addr, sg_mem_wr.Rd_HBM_Edge_Valid,
        sg_mem_wr.Active_V_Edge, sg_mem_wr.Active_V_Edge_Valid);

   #if DEBUG6
        cout << "----------------------------------------HBM ITF start---------------------------------" << endl;
        cout << "clk=" << clk << endl;
        sg_view(sg_mem_wr.Stage_Full, "Full", 6);
        sg_view(sg_mem_wr.Rd_HBM_Edge_Addr, "E_Addr", 1);
        sg_view(sg_mem_wr.Rd_HBM_Edge_Valid, "E_Valid", 1);
        // sg_view(sg_p4_wr.RD_HBM_Edge_Mask, "V_Value_Addr", 16);
        sg_view(sg_mem_wr.Active_V_Edge, "E_ID", 16);
        sg_view(sg_mem_wr.Active_V_Edge_Valid, "E_Valid", 16);
        cout << "----------------------------------------HBM ITF end-----------------------------------" << endl;
    #endif

    HBM_Controller_IP_Ideal(
        sg_mem_rd.Rd_HBM_Edge_Addr, sg_mem_rd.Rd_HBM_Edge_Valid,

        sg_mem_wr.HBM_Controller_Full,
        sg_mem_wr.HBM_Controller_Edge, sg_mem_wr.HBM_Controller_DValid);

    pipes->read(&sg_mem_wr);
}


