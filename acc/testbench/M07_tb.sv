`timescale 1ns / 1ps

`include "accelerator.vh"

module M07_tb();

    parameter V_ID_WIDTH = `V_ID_WIDTH;
    parameter V_VALUE_WIDTH = `V_VALUE_WIDTH;
    parameter CORE_NUM = `CORE_NUM;
    parameter VTX_BRAM_AWIDTH = `VTX_NUM;
    parameter VTX_BRAM_DWIDTH = `VTX_NUM;
    parameter ITERATION_DWIDTH = `ITERATION_WIDTH;
    
    parameter integer LP_CLK_PERIOD_PS = 5000; // 200 MHz

    logic ap_clk = 0;

    initial begin: AP_CLK
        forever begin
            ap_clk = #(LP_CLK_PERIOD_PS / 2) ~ap_clk;
        end
    end

    logic [CORE_NUM-1:0] ap_rst = 'h0;

    // module input
    reg [CORE_NUM - 1 : 0]                    om1_recv_push_flag;
    reg [CORE_NUM * V_ID_WIDTH - 1 : 0]       om1_recv_update_v_id;
    reg [CORE_NUM * V_VALUE_WIDTH - 1 : 0]    om1_recv_update_v_value;
    reg [CORE_NUM - 1 : 0]                    om1_recv_update_v_pull_first_flag;
    reg [CORE_NUM - 1 : 0]                    om1_recv_update_v_valid;
    reg [CORE_NUM - 1 : 0]                    om1_recv_iteration_end;
    reg [CORE_NUM - 1 : 0]                    om1_recv_iteration_end_valid;
    reg [CORE_NUM - 1 : 0]                    om2_recv_push_flag;
    reg [CORE_NUM * V_ID_WIDTH - 1 : 0]       om2_recv_update_v_id;
    reg [CORE_NUM * V_VALUE_WIDTH - 1 : 0]    om2_recv_update_v_value;
    reg [CORE_NUM - 1 : 0]                    om2_recv_update_v_pull_first_flag;
    reg [CORE_NUM - 1 : 0]                    om2_recv_update_v_valid;
    reg [CORE_NUM - 1 : 0]                    om2_recv_iteration_end;
    reg [CORE_NUM - 1 : 0]                    om2_recv_iteration_end_valid;
    reg [CORE_NUM * VTX_BRAM_DWIDTH - 1 : 0]  vertex_bram_data;
    reg [CORE_NUM - 1 : 0]                    vertex_bram_valid;
    reg [CORE_NUM * ITERATION_DWIDTH - 1 : 0] om_global_iteration_id;

    // module output
    wire [CORE_NUM - 1 : 0]                   next_rst1;
    wire [CORE_NUM - 1 : 0]                   next_rst2;
    wire [CORE_NUM - 1 : 0]                   source_core_full;
    wire [CORE_NUM * VTX_BRAM_AWIDTH - 1 : 0] rd_vertex_bram_addr;
    wire [CORE_NUM - 1 : 0]                   rd_vertex_bram_valid;
    wire [CORE_NUM - 1 : 0]                   wr_vertex_bram_push_flag;
    wire [CORE_NUM * VTX_BRAM_AWIDTH - 1 : 0] wr_vertex_bram_addr;
    wire [CORE_NUM * VTX_BRAM_DWIDTH - 1 : 0] wr_vertex_bram_data;
    wire [CORE_NUM - 1 : 0]                   wr_vertex_bram_pull_first_flag;
    wire [CORE_NUM - 1 : 0]                   wr_vertex_bram_valid;
    wire [CORE_NUM - 1 : 0]                   wr_vertex_bram_iteration_end;
    wire [CORE_NUM - 1 : 0]                   wr_vertex_bram_iteration_end_valid;
    wire [CORE_NUM - 1 : 0]                   om2_send_push_flag;
    wire [CORE_NUM * V_ID_WIDTH - 1 : 0]      om2_send_update_v_id;
    wire [CORE_NUM * V_VALUE_WIDTH - 1 : 0]   om2_send_update_v_value;
    wire [CORE_NUM - 1 : 0]                   om2_send_update_v_pull_first_flag;
    wire [CORE_NUM - 1 : 0]                   om2_send_update_v_valid;
    wire [CORE_NUM - 1 : 0]                   om2_send_iteration_end;
    wire [CORE_NUM - 1 : 0]                   om2_send_iteration_end_valid;
    
    task automatic init_input_flag();
        om1_recv_push_flag = 'h0;
        om1_recv_update_v_id = 'h0;
        om1_recv_update_v_value = 'h0;
        om1_recv_update_v_pull_first_flag = 'h0;
        om1_recv_update_v_valid = 'h0;
        om1_recv_iteration_end = 'h0;
        om1_recv_iteration_end_valid = 'h0;
        om2_recv_push_flag = 'h0;
        om2_recv_update_v_id = 'h0;
        om2_recv_update_v_value = 'h0;
        om2_recv_update_v_pull_first_flag = 'h0;
        om2_recv_update_v_valid = 'h0;
        om2_recv_iteration_end = 'h0;
        om2_recv_iteration_end_valid = 'h0;
        vertex_bram_data = 'h0;
        vertex_bram_valid = 'h0;
        om_global_iteration_id = 'h0;
    endtask

    task automatic reset_module();
        ap_rst = {512{1'b1}};
        init_input_flag();
        repeat (10) @(posedge ap_clk);
        ap_rst = 'h0;
    endtask

    integer i,j,k,t;
    logic test_pass;
    
    task automatic test_Pull_given_Rst_then_LocalBufferInit();
        reset_module();
        init_input_flag();
        test_pass = 1;
        repeat (1) @(posedge ap_clk);
        
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(source_core_full[i] == 1'b0)
            else begin
                $error("source_core_full init error!");
                test_pass = 0;
            end
            assert(rd_vertex_bram_addr[i * VTX_BRAM_AWIDTH +: VTX_BRAM_AWIDTH] == 'h0)
            else begin
                $error("rd_vertex_bram_addr init error!");
                test_pass = 0;
            end
            assert(rd_vertex_bram_valid[i] == 1'b0)
            else begin
                $error("rd_vertex_bram_valid init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_push_flag[i] == 1'b0)
            else begin
                $error("wr_vertex_bram_push_flag init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_addr[i * VTX_BRAM_AWIDTH +: VTX_BRAM_AWIDTH] == 'h0)
            else begin
                $error("wr_vertex_bram_addr init error!");
                test_pass = 0;
            end
            assert(wr_vertex_bram_data[i * VTX_BRAM_DWIDTH +: VTX_BRAM_DWIDTH] == 'h0)
            else begin
                $error("wr_vertex_bram_data init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_pull_first_flag[i] == 1'b0)
            else begin
                $error("wr_vertex_bram_pull_first_flag init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_valid[i] == 1'b0)
            else begin
                $error("wr_vertex_bram_valid init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_iteration_end[i] == 1'b0)
            else begin
                $error("wr_vertex_bram_iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, wr_vertex_bram_iteration_end[i], 1'b0);
            end
            assert(wr_vertex_bram_iteration_end_valid[i] == 1'b0)
            else begin
                $error("wr_vertex_bram_iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, wr_vertex_bram_iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
            assert(om2_send_push_flag[i] == 1'b0)
            else begin
                $error("om2_send_push_flag init error!");
                test_pass = 0;
            end
            assert(om2_send_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("om2_send_update_v_id init error");
                test_pass = 0;
            end
            assert(om2_send_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("om2_send_update_v_value init error");
                test_pass = 0;
            end
            assert(om2_send_update_v_pull_first_flag[i] == 1'b0)
            else begin
                $error("om2_send_update_v_pull_first_flag init error");
                test_pass = 0;
            end
            assert(om2_send_update_v_valid[i] == 1'b0)
            else begin
                $error("om2_send_update_v_valid init error");
                test_pass = 0;
            end
            assert(om2_send_iteration_end[i] == 1'b0)
            else begin
                $error("om2_send_iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, om2_send_iteration_end[i], 1'b0);
            end
            assert(om2_send_iteration_end_valid[i] == 1'b0)
            else begin
                $error("om2_send_iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, om2_send_iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
        end
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Pull_given_Rst_then_LocalBufferInit");
    endtask
    
    task automatic test_Pull_given_OM1Valid_then_StoreAndOutput();
        reset_module();
        init_input_flag();
        test_pass = 1;
        
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            om1_recv_push_flag[i] = 0;
            om1_recv_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i;
            om1_recv_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] = i+1;
            om1_recv_update_v_valid[i] = 1;
            om1_recv_update_v_pull_first_flag[i] = 1;
            om1_recv_iteration_end[i] = 0;
            om1_recv_iteration_end_valid[i] = 0;
        end
        repeat (1) @(posedge ap_clk);
        
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(source_core_full[i] == 1'b0)
            else begin
                $error("source_core_full init error!");
                test_pass = 0;
            end
            assert(rd_vertex_bram_addr[i * VTX_BRAM_AWIDTH +: VTX_BRAM_AWIDTH] == i+1)
            else begin
                $error("rd_vertex_bram_addr init error!");
                test_pass = 0;
            end
            assert(rd_vertex_bram_valid[i] == 1'b1)
            else begin
                $error("rd_vertex_bram_valid init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_push_flag[i] == 1'b0)
            else begin
                $error("wr_vertex_bram_push_flag init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_addr[i * VTX_BRAM_AWIDTH +: VTX_BRAM_AWIDTH] == 'h0)
            else begin
                $error("wr_vertex_bram_addr init error!");
                test_pass = 0;
            end
            assert(wr_vertex_bram_data[i * VTX_BRAM_DWIDTH +: VTX_BRAM_DWIDTH] == 'h0)
            else begin
                $error("wr_vertex_bram_data init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_pull_first_flag[i] == 1'b0)
            else begin
                $error("wr_vertex_bram_pull_first_flag init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_valid[i] == 1'b0)
            else begin
                $error("wr_vertex_bram_valid init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_iteration_end[i] == 1'b0)
            else begin
                $error("wr_vertex_bram_iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, wr_vertex_bram_iteration_end[i], 1'b0);
            end
            assert(wr_vertex_bram_iteration_end_valid[i] == 1'b0)
            else begin
                $error("wr_vertex_bram_iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, wr_vertex_bram_iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
            assert(om2_send_push_flag[i] == 1'b0)
            else begin
                $error("om2_send_push_flag init error!");
                test_pass = 0;
            end
            assert(om2_send_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("om2_send_update_v_id init error");
                test_pass = 0;
            end
            assert(om2_send_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("om2_send_update_v_value init error");
                test_pass = 0;
            end
            assert(om2_send_update_v_pull_first_flag[i] == 1'b0)
            else begin
                $error("om2_send_update_v_pull_first_flag init error");
                test_pass = 0;
            end
            assert(om2_send_update_v_valid[i] == 1'b0)
            else begin
                $error("om2_send_update_v_valid init error");
                test_pass = 0;
            end
            assert(om2_send_iteration_end[i] == 1'b0)
            else begin
                $error("om2_send_iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, om2_send_iteration_end[i], 1'b0);
            end
            assert(om2_send_iteration_end_valid[i] == 1'b0)
            else begin
                $error("om2_send_iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, om2_send_iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
        end
        init_input_flag();
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            vertex_bram_data[i * VTX_BRAM_DWIDTH +: VTX_BRAM_DWIDTH] = 2;
            vertex_bram_valid[i] = 1;
        end
        repeat (3) @(posedge ap_clk);
        @(negedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(source_core_full[i] == 1'b0)
            else begin
                $error("source_core_full init error!");
                test_pass = 0;
            end
            assert(rd_vertex_bram_addr[i * VTX_BRAM_AWIDTH +: VTX_BRAM_AWIDTH] == 'h0)
            else begin
                $error("rd_vertex_bram_addr init error!");
                test_pass = 0;
            end
            assert(rd_vertex_bram_valid[i] == 1'b0)
            else begin
                $error("rd_vertex_bram_valid init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_push_flag[i] == 1'b0)
            else begin
                $error("wr_vertex_bram_push_flag init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_addr[i * VTX_BRAM_AWIDTH +: VTX_BRAM_AWIDTH] == 'h0)
            else begin
                $error("wr_vertex_bram_addr init error!");
                test_pass = 0;
            end
            assert(wr_vertex_bram_data[i * VTX_BRAM_DWIDTH +: VTX_BRAM_DWIDTH] == 'h0)
            else begin
                $error("wr_vertex_bram_data init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_pull_first_flag[i] == 1'b0)
            else begin
                $error("wr_vertex_bram_pull_first_flag init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_valid[i] == 1'b0)
            else begin
                $error("wr_vertex_bram_valid init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_iteration_end[i] == 1'b0)
            else begin
                $error("wr_vertex_bram_iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, wr_vertex_bram_iteration_end[i], 1'b0);
            end
            assert(wr_vertex_bram_iteration_end_valid[i] == 1'b0)
            else begin
                $error("wr_vertex_bram_iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, wr_vertex_bram_iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
            assert(om2_send_push_flag[i] == 1'b0)
            else begin
                $error("om2_send_push_flag init error!");
                test_pass = 0;
            end
            assert(om2_send_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == i)
            else begin
                $error("om2_send_update_v_id init error");
                test_pass = 0;
            end
            assert(om2_send_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 2)
            else begin
                $error("om2_send_update_v_value init error");
                test_pass = 0;
            end
            assert(om2_send_update_v_pull_first_flag[i] == 1'b1)
            else begin
                $error("om2_send_update_v_pull_first_flag init error");
                test_pass = 0;
            end
            assert(om2_send_update_v_valid[i] == 1'b1)
            else begin
                $error("om2_send_update_v_valid init error");
                test_pass = 0;
            end
            assert(om2_send_iteration_end[i] == 1'b0)
            else begin
                $error("om2_send_iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, om2_send_iteration_end[i], 1'b0);
            end
            assert(om2_send_iteration_end_valid[i] == 1'b0)
            else begin
                $error("om2_send_iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, om2_send_iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
        end
        
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Pull_given_OM1Valid_then_StoreAndOutput");
    endtask
    
    task automatic test_Pull_given_OM2Valid_then_StoreAndOutput();
        reset_module();
        init_input_flag();
        test_pass = 1;
        
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            om2_recv_push_flag[i] = 0;
            om2_recv_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i;
            om2_recv_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] = i%64;
            om2_recv_update_v_valid[i] = 1;
            om2_recv_update_v_pull_first_flag[i] = 1;
            om2_recv_iteration_end[i] = 0;
            om2_recv_iteration_end_valid[i] = 0;
        end
        repeat (1) @(posedge ap_clk);
        
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(source_core_full[i] == 1'b0)
            else begin
                $error("source_core_full init error!");
                test_pass = 0;
            end
            assert(rd_vertex_bram_addr[i * VTX_BRAM_AWIDTH +: VTX_BRAM_AWIDTH] == 'h0)
            else begin
                $error("rd_vertex_bram_addr init error!");
                test_pass = 0;
            end
            assert(rd_vertex_bram_valid[i] == 1'b0)
            else begin
                $error("rd_vertex_bram_valid init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_push_flag[i] == 1'b0)
            else begin
                $error("wr_vertex_bram_push_flag init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_addr[i * VTX_BRAM_AWIDTH +: VTX_BRAM_AWIDTH] == i)
            else begin
                $error("wr_vertex_bram_addr init error!");
                test_pass = 0;
            end
            assert(wr_vertex_bram_data[i * VTX_BRAM_DWIDTH +: VTX_BRAM_DWIDTH] == i%64)
            else begin
                $error("wr_vertex_bram_data init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_pull_first_flag[i] == 1'b1)
            else begin
                $error("wr_vertex_bram_pull_first_flag init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_valid[i] == 1'b1)
            else begin
                $error("wr_vertex_bram_valid init error");
                test_pass = 0;
            end
            assert(wr_vertex_bram_iteration_end[i] == 1'b0)
            else begin
                $error("wr_vertex_bram_iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, wr_vertex_bram_iteration_end[i], 1'b0);
            end
            assert(wr_vertex_bram_iteration_end_valid[i] == 1'b0)
            else begin
                $error("wr_vertex_bram_iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, wr_vertex_bram_iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
            assert(om2_send_push_flag[i] == 1'b0)
            else begin
                $error("om2_send_push_flag init error!");
                test_pass = 0;
            end
            assert(om2_send_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("om2_send_update_v_id init error");
                test_pass = 0;
            end
            assert(om2_send_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("om2_send_update_v_value init error");
                test_pass = 0;
            end
            assert(om2_send_update_v_pull_first_flag[i] == 1'b0)
            else begin
                $error("om2_send_update_v_pull_first_flag init error");
                test_pass = 0;
            end
            assert(om2_send_update_v_valid[i] == 1'b0)
            else begin
                $error("om2_send_update_v_valid init error");
                test_pass = 0;
            end
            assert(om2_send_iteration_end[i] == 1'b0)
            else begin
                $error("om2_send_iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, om2_send_iteration_end[i], 1'b0);
            end
            assert(om2_send_iteration_end_valid[i] == 1'b0)
            else begin
                $error("om2_send_iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, om2_send_iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
        end
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Pull_given_OM2Valid_then_StoreAndOutput");
    endtask
    
    backend_core inst_M07_dut (
        .clk                                            (ap_clk),
        .om1_rst                                        (ap_rst),
        .om1_recv_push_flag                             (om1_recv_push_flag),
        .om1_recv_update_v_id                           (om1_recv_update_v_id),
        .om1_recv_update_v_value                        (om1_recv_update_v_value),
        .om1_recv_update_v_pull_first_flag              (om1_recv_update_v_pull_first_flag),
        .om1_recv_update_v_valid                        (om1_recv_update_v_valid),
        .om1_recv_iteration_end                         (om1_recv_iteration_end),
        .om1_recv_iteration_end_valid                   (om1_recv_iteration_end_valid),
        .om2_rst                                        (ap_rst),
        .om2_recv_push_flag                             (om2_recv_push_flag),
        .om2_recv_update_v_id                           (om2_recv_update_v_id),
        .om2_recv_update_v_value                        (om2_recv_update_v_value),
        .om2_recv_update_v_pull_first_flag              (om2_recv_update_v_pull_first_flag),
        .om2_recv_update_v_valid                        (om2_recv_update_v_valid),
        .om2_recv_iteration_end                         (om2_recv_iteration_end),
        .om2_recv_iteration_end_valid                   (om2_recv_iteration_end_valid),
        .vertex_bram_data                               (vertex_bram_data),
        .vertex_bram_valid                              (vertex_bram_valid),
        .om_global_iteration_id                         (om_global_iteration_id),

        .next_rst1                                      (next_rst1),
        .next_rst2                                      (next_rst2),
        .source_core_full                               (source_core_full),
        .rd_vertex_bram_addr                            (rd_vertex_bram_addr),
        .rd_vertex_bram_valid                           (rd_vertex_bram_valid),
        .wr_vertex_bram_push_flag                       (wr_vertex_bram_push_flag),
        .wr_vertex_bram_addr                            (wr_vertex_bram_addr),
        .wr_vertex_bram_data                            (wr_vertex_bram_data),
        .wr_vertex_bram_pull_first_flag                 (wr_vertex_bram_pull_first_flag),
        .wr_vertex_bram_valid                           (wr_vertex_bram_valid),
        .wr_vertex_bram_iteration_end                   (wr_vertex_bram_iteration_end),
        .wr_vertex_bram_iteration_end_valid             (wr_vertex_bram_iteration_end_valid),
        .om2_send_push_flag                             (om2_send_push_flag),
        .om2_send_update_v_id                           (om2_send_update_v_id),
        .om2_send_update_v_value                        (om2_send_update_v_value),
        .om2_send_update_v_pull_first_flag              (om2_send_update_v_pull_first_flag),
        .om2_send_update_v_valid                        (om2_send_update_v_valid),
        .om2_send_iteration_end                         (om2_send_iteration_end),
        .om2_send_iteration_end_valid                   (om2_send_iteration_end_valid)
    );

    initial begin : backend_core_Test
        test_Pull_given_Rst_then_LocalBufferInit();
        test_Pull_given_OM1Valid_then_StoreAndOutput();
        test_Pull_given_OM2Valid_then_StoreAndOutput();
        repeat (10) @(posedge ap_clk);
        $finish;
    end
endmodule
