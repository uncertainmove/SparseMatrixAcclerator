#include "core.h"
#include "parameter.h"
#include "self_config.h"
#include "structure.h"
#include "debug.h"
#include "util.h"

extern int clk;
extern int rst_rd;

void Compute_Core(pipe_manager* pipes) {
    // from
    static Info_MEM sg_from_mem;
    static Info_P8 sg_from_p8;
    // to
    static Info_P1 sg_p1_rd, sg_p1_wr;
    static Info_P2 sg_p2_rd, sg_p2_wr;
    static Info_P4 sg_p4_rd, sg_p4_wr;
    static Info_P5 sg_p5_rd, sg_p5_wr;
    static Info_P7 sg_p7_rd, sg_p7_wr;
    static Info_P9 sg_p9_rd, sg_p9_wr;
    static Info_P10 sg_p10_rd, sg_p10_wr;
    static Info_P11 sg_p11_rd, sg_p11_wr;

    pipes->write(&sg_from_mem);
    pipes->write(&sg_from_p8);
    pipes->write(&sg_p1_rd);
    pipes->write(&sg_p2_rd);
    pipes->write(&sg_p4_rd);
    pipes->write(&sg_p5_rd);
    pipes->write(&sg_p7_rd);
    pipes->write(&sg_p9_rd);
    pipes->write(&sg_p10_rd);
    pipes->write(&sg_p11_rd);

    RD_ACTIVE_VERTEX(
        sg_p11_rd.Active_V_ID, sg_p11_rd.Active_V_Updated, sg_p11_rd.Active_V_DValid,
        sg_p11_rd.Iteration_End, sg_p11_rd.Iteration_End_DValid,
        sg_p2_rd.Stage_Full,

        sg_p1_wr.Active_V_ID, sg_p1_wr.Active_V_DValid,
        sg_p1_wr.Iteration_End, sg_p1_wr.Iteration_End_DValid,
        sg_p1_wr.Iteration_ID);

    #if DEBUG1
        cout << "----------------------------------------P1 start---------------------------------" << endl;
        cout << "clk=" << clk << endl;
        sg_view(sg_p1_wr.Active_V_ID, "V_ID", 16);
        sg_view(sg_p1_wr.Active_V_DValid, "V_Valid", 16);
        sg_view(sg_p1_wr.Iteration_End, "It_End", 16);
        sg_view(sg_p1_wr.Iteration_End_DValid, "It_End_Valid", 16);
        sg_view(sg_p1_wr.Iteration_ID, "It_ID", 16);
        cout << "----------------------------------------P1 end-----------------------------------" << endl;
    #endif

    Read_Active_Vertex_Offset(
        sg_p1_rd.Active_V_ID, sg_p1_rd.Active_V_DValid,
        sg_p1_rd.Iteration_End, sg_p1_rd.Iteration_End_DValid, sg_p1_rd.Iteration_ID,
        sg_p4_rd.Stage_Full,

        sg_p2_wr.Stage_Full,
        sg_p2_wr.Active_V_ID, sg_p2_wr.RD_Active_V_Offset_Addr,
        sg_p2_wr.RD_Active_V_Value_Addr, sg_p2_wr.Active_V_DValid,
        sg_p2_wr.Iteration_End, sg_p2_wr.Iteration_End_DValid, sg_p2_wr.Iteration_ID);
    
    #if DEBUG1
        cout << "----------------------------------------P2 start---------------------------------" << endl;
        cout << "clk=" << clk << endl;
        sg_view(sg_p2_wr.Stage_Full, "Full", 16);
        sg_view(sg_p2_wr.Active_V_ID, "V_ID", 16);
        sg_view(sg_p2_wr.RD_Active_V_Offset_Addr, "V_OFF_Addr", 16);
        sg_view(sg_p2_wr.RD_Active_V_Value_Addr, "V_Value_Addr", 16);
        sg_view(sg_p2_wr.Active_V_DValid, "V_Valid", 16);
        sg_view(sg_p2_wr.Iteration_End, "It_End", 16);
        sg_view(sg_p2_wr.Iteration_End_DValid, "It_End_Valid", 16);
        sg_view(sg_p2_wr.Iteration_ID, "It_ID", 16);
        cout << "----------------------------------------P2 end-----------------------------------" << endl;
    #endif
    
    RD_Active_Vertex_Edge(
        sg_p2_rd.Active_V_ID, sg_p2_rd.Active_V_DValid,
        sg_p2_rd.Iteration_End, sg_p2_rd.Iteration_End_DValid, sg_p2_rd.Iteration_ID,
        sg_from_mem.Active_V_LOffset, sg_from_mem.Active_V_ROffset,
        sg_from_mem.Active_V_DValid,
        sg_p10_rd.Src_Recv_Update_V_Value, sg_p10_rd.Src_Recv_Update_V_DValid,
        sg_p5_rd.Stage_Full,

        sg_p4_wr.Stage_Full,
        sg_p4_wr.Rd_HBM_Edge_Addr, sg_p4_wr.Rd_HBM_Edge_Mask,
        sg_p4_wr.HBM_Active_V_ID, sg_p4_wr.HBM_Active_V_Value, sg_p4_wr.Rd_HBM_Edge_Valid,
        sg_p4_wr.Iteration_End, sg_p4_wr.Iteration_End_DValid, sg_p4_wr.Iteration_ID);
    
   #if DEBUG1
        cout << "----------------------------------------P4 start---------------------------------" << endl;
        cout << "clk=" << clk << endl;
        sg_view(sg_p4_wr.Stage_Full, "Full", 16);
        sg_view(sg_p4_wr.Rd_HBM_Edge_Addr, "E_Addr", 16);
        // sg_view(sg_p4_wr.RD_HBM_Edge_Mask, "V_Value_Addr", 16);
        sg_view(sg_p4_wr.HBM_Active_V_ID, "V_ID", 16);
        sg_view(sg_p4_wr.HBM_Active_V_Value, "V_Value", 16);
        sg_view(sg_p4_wr.Rd_HBM_Edge_Valid, "V_Valid", 16);
        sg_view(sg_p4_wr.Iteration_End, "It_End", 16);
        sg_view(sg_p4_wr.Iteration_End_DValid, "It_End_Valid", 16);
        sg_view(sg_p4_wr.Iteration_ID, "It_ID", 16);
        cout << "----------------------------------------P4 end-----------------------------------" << endl;
    #endif

    Generate_HBM_Edge_Rqst(
        sg_p4_rd.Rd_HBM_Edge_Addr, sg_p4_rd.Rd_HBM_Edge_Mask,
        sg_p4_rd.Rd_HBM_Edge_Valid, sg_p4_rd.HBM_Active_V_ID, sg_p4_rd.HBM_Active_V_Value,
        sg_p4_rd.Iteration_End, sg_p4_rd.Iteration_End_DValid, sg_p4_rd.Iteration_ID,
        sg_from_mem.Stage_Full, sg_p7_rd.Stage_Full,

        sg_p5_wr.Stage_Full,
        sg_p5_wr.Rd_HBM_Edge_Addr, sg_p5_wr.Rd_HBM_Edge_Valid,
        sg_p5_wr.Active_V_ID, sg_p5_wr.Active_V_Value, sg_p5_wr.Active_V_DValid,
        sg_p5_wr.Iteration_End, sg_p5_wr.Iteration_End_DValid, sg_p5_wr.Iteration_ID);

   #if DEBUG1
        cout << "----------------------------------------P5 start---------------------------------" << endl;
        cout << "clk=" << clk << endl;
        sg_view(sg_p5_wr.Stage_Full, "Full", 16);
        sg_view(sg_p5_wr.Rd_HBM_Edge_Addr, "E_Addr", 1);
        sg_view(sg_p5_wr.Rd_HBM_Edge_Valid, "E_Valid", 1);
        // sg_view(sg_p4_wr.RD_HBM_Edge_Mask, "V_Value_Addr", 16);
        sg_view(sg_p5_wr.Active_V_ID, "V_ID", 16);
        sg_view(sg_p5_wr.Active_V_Value, "V_Value", 16);
        sg_view(sg_p5_wr.Active_V_DValid, "V_Valid", 16);
        sg_view(sg_p5_wr.Iteration_End, "It_End", 16);
        sg_view(sg_p5_wr.Iteration_End_DValid, "It_End_Valid", 16);
        sg_view(sg_p5_wr.Iteration_ID, "It_ID", 16);
        cout << "----------------------------------------P5 end-----------------------------------" << endl;
    #endif

    Schedule(
        sg_p5_rd.Active_V_ID, sg_p5_rd.Active_V_Value, sg_p5_rd.Active_V_DValid,
        sg_p5_rd.Iteration_End, sg_p5_rd.Iteration_End_DValid, sg_p5_rd.Iteration_ID,
        sg_from_mem.Active_V_Edge, sg_from_mem.Active_V_Edge_Valid,
        sg_from_p8.Stage_Full,

        sg_p7_wr.Stage_Full,
        sg_p7_wr.Update_V_ID, sg_p7_wr.Update_V_Value, sg_p7_wr.Update_V_DValid,
        sg_p7_wr.Iteration_End, sg_p7_wr.Iteration_End_DValid, sg_p7_wr.Iteration_ID);

   #if DEBUG1
        cout << "----------------------------------------P6 start---------------------------------" << endl;
        cout << "clk=" << clk << endl;
        sg_view(sg_p7_wr.Stage_Full, "Full", 16);
        sg_view(sg_p7_wr.Update_V_ID, "V_ID", 16);
        sg_view(sg_p7_wr.Update_V_Value, "V_Value", 16);
        sg_view(sg_p7_wr.Update_V_DValid, "V_Valid", 16);
        sg_view(sg_p7_wr.Iteration_End, "It_End", 16);
        sg_view(sg_p7_wr.Iteration_End_DValid, "It_End_Valid", 16);
        sg_view(sg_p7_wr.Iteration_ID, "It_ID", 16);
        cout << "----------------------------------------P6 end-----------------------------------" << endl;
    #endif

    Backend_Core(
        sg_from_p8.Hy1_Update_V_ID, sg_from_p8.Hy1_Update_V_Value, sg_from_p8.Hy1_Update_V_DValid,
        sg_from_p8.Hy1_Iteration_End, sg_from_p8.Hy1_Iteration_End_DValid, sg_from_p8.Hy1_Iteration_ID,
        sg_p10_rd.Stage_Full,

        sg_p9_wr.Dest_Core_Full,
        sg_p9_wr.Wr_Vertex_BRAM_Addr, sg_p9_wr.Wr_Vertex_BRAM_Data, sg_p9_wr.Wr_Vertex_BRAM_Valid,
        sg_p9_wr.Wr_Vertex_BRAM_Iteration_End, sg_p9_wr.Wr_Vertex_BRAM_Iteration_End_DValid,
        sg_p9_wr.Wr_Vertex_BRAM_Iteration_ID);

   #if DEBUG1
        cout << "----------------------------------------P6 start---------------------------------" << endl;
        cout << "clk=" << clk << endl;
        sg_view(sg_p7_wr.Stage_Full, "Full", 16);
        sg_view(sg_p7_wr.Update_V_ID, "V_ID", 16);
        sg_view(sg_p7_wr.Update_V_Value, "V_Value", 16);
        sg_view(sg_p7_wr.Update_V_DValid, "V_Valid", 16);
        sg_view(sg_p7_wr.Iteration_End, "It_End", 16);
        sg_view(sg_p7_wr.Iteration_End_DValid, "It_End_Valid", 16);
        sg_view(sg_p7_wr.Iteration_ID, "It_ID", 16);
        cout << "----------------------------------------P6 end-----------------------------------" << endl;
    #endif

    Vertex_RAM_Controller(
        sg_p2_rd.RD_Active_V_Value_Addr, sg_p2_rd.Active_V_DValid,
        sg_p9_rd.Wr_Vertex_BRAM_Addr, sg_p9_rd.Wr_Vertex_BRAM_Data, sg_p9_rd.Wr_Vertex_BRAM_Valid,
        sg_p9_rd.Wr_Vertex_BRAM_Iteration_End, sg_p9_rd.Wr_Vertex_BRAM_Iteration_End_DValid,
        sg_p2_rd.Iteration_ID,

        sg_from_mem.Delta1_Bram_Data, sg_from_mem.Delta1_Bram_Data_Valid,
        sg_from_mem.Delta2_Bram_Data, sg_from_mem.Delta2_Bram_Data_Valid,
        sg_from_mem.PR_Uram_Data, sg_from_mem.PR_Uram_Data_Valid,

        sg_p10_wr.Delta1_Bram_Rd_Addr, sg_p10_wr.Delta1_Bram_Rd_Valid,
        sg_p10_wr.Delta2_Bram_Rd_Addr, sg_p10_wr.Delta2_Bram_Rd_Valid,
        sg_p10_wr.Delta1_Bram_Wr_Addr, sg_p10_wr.Delta1_Bram_Wr_Value, sg_p10_wr.Delta1_Bram_Wr_Valid,
        sg_p10_wr.Delta2_Bram_Wr_Addr, sg_p10_wr.Delta2_Bram_Wr_Value, sg_p10_wr.Delta2_Bram_Wr_Valid,
        sg_p10_wr.PR_Uram_Rd_Addr, sg_p10_wr.PR_Uram_Rd_Valid,
        sg_p10_wr.PR_Uram_Wr_Addr, sg_p10_wr.PR_Uram_Wr_Value, sg_p10_wr.PR_Uram_Wr_Valid,

        sg_p10_wr.Stage_Full,
        sg_p10_wr.Src_Recv_Update_V_Value, sg_p10_wr.Src_Recv_Update_V_DValid,
        sg_p10_wr.Backend_Active_V_ID, sg_p10_wr.Backend_Active_V_Updated, sg_p10_wr.Backend_Active_V_DValid,
        sg_p10_wr.Iteration_End, sg_p10_wr.Iteration_End_DValid);

    Apply_Iteration_End(
        sg_p10_rd.Backend_Active_V_ID, sg_p10_rd.Backend_Active_V_Updated, sg_p10_rd.Backend_Active_V_DValid,
        sg_p10_rd.Iteration_End, sg_p10_rd.Iteration_End_DValid,

        sg_p11_wr.Active_V_ID, sg_p11_wr.Active_V_Updated, sg_p11_wr.Active_V_DValid,
        sg_p11_wr.Iteration_End, sg_p11_wr.Iteration_End_DValid);

/*
    if (clk == 463) {
        printf("sg_p10_wr: %d %d %d %d %d %d\n", sg_p10_wr.Delta1_Bram_Rd_Addr[0],
                sg_p10_wr.Delta2_Bram_Rd_Addr[0], sg_p10_wr.PR_Uram_Rd_Addr[0],
                sg_p10_wr.Delta1_Bram_Rd_Valid[0], sg_p10_wr.Delta2_Bram_Rd_Valid[0],
                sg_p10_wr.PR_Uram_Rd_Valid[0]);
    }
*/
    pipes->read(&sg_p1_wr);
    pipes->read(&sg_p2_wr);
    pipes->read(&sg_p4_wr);
    pipes->read(&sg_p5_wr);
    pipes->read(&sg_p7_wr);
    pipes->read(&sg_p9_wr);
    pipes->read(&sg_p10_wr);
    pipes->read(&sg_p11_wr);
}