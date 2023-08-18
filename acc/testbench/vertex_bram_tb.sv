`timescale 1ns / 1ps

`include "accelerator.vh"

module vertex_bram_tb();

    parameter V_ID_WIDTH = `V_ID_WIDTH;
    parameter VTX_BRAM_AWIDTH = `VTX_BRAM_AWIDTH;
    parameter VTX_BRAM_DWIDTH = `VTX_BRAM_DWIDTH;
    parameter ITERATION_DWIDTH = `ITERATION_DWIDTH;
    parameter V_VALUE_WIDTH = `V_VALUE_WIDTH;
    parameter CORE_NUM = `CORE_NUM;
    
    parameter integer LP_CLK_PERIOD_PS = 5000; // 200 MHz

    logic ap_clk = 0;

    initial begin: AP_CLK
        forever begin
            ap_clk = #(LP_CLK_PERIOD_PS / 2) ~ap_clk;
        end
    end

    logic [CORE_NUM-1:0] ap_rst = 'h0;

    // module input
    reg [CORE_NUM * VTX_BRAM_AWIDTH - 1 : 0]  rd_addr_src;
    reg [CORE_NUM - 1 : 0]                    rd_valid_src;
    reg [CORE_NUM - 1 : 0]                    wr_push_flag_dst;
    reg [CORE_NUM * VTX_BRAM_AWIDTH - 1 : 0]  wr_addr_dst;
    reg [CORE_NUM * VTX_BRAM_DWIDTH - 1 : 0]  wr_v_value_dst;
    reg [CORE_NUM - 1 : 0]                    wr_pull_first_flag_dst;
    reg [CORE_NUM - 1 : 0]                    wr_valid_dst;
    reg [CORE_NUM - 1 : 0]                    front_iteration_end;
    reg [CORE_NUM - 1 : 0]                    front_iteration_end_valid;
    reg [CORE_NUM * ITERATION_DWIDTH - 1 : 0] front_iteration_id;

    // module output
    wire [CORE_NUM - 1 : 0]                   next_rst;
    wire [CORE_NUM * V_VALUE_WIDTH - 1 : 0]   src_recv_update_v_value;
    wire [CORE_NUM - 1 : 0]                   src_recv_update_v_valid;
    wire [CORE_NUM * V_ID_WIDTH - 1 : 0]      backend_active_v_id;
    wire [CORE_NUM - 1 : 0]                   backend_active_v_updated;
    wire [CORE_NUM - 1 : 0]                   backend_active_v_pull_first_flag;
    wire [CORE_NUM - 1 : 0]                   backend_active_v_valid;
    wire [CORE_NUM - 1 : 0]                   iteration_end;
    wire [CORE_NUM - 1 : 0]                   iteration_end_valid;

    task automatic init_input_flag();
        rd_addr_src = 'h0;
        rd_valid_src = 'h0;
        wr_push_flag_dst = 'h0;
        wr_addr_dst = 'h0;
        wr_v_value_dst = 'h0;
        wr_pull_first_flag_dst = 'h0;
        wr_valid_dst = 'h0;
        front_iteration_end = 'h0;
        front_iteration_end_valid = 'h0;
        front_iteration_id = 'h0;
    endtask

    task automatic reset_module();
        ap_rst = {512{1'b1}};
        init_input_flag();
        repeat (10) @(posedge ap_clk);
        ap_rst = 'h0;
    endtask

    integer i,j;
    logic test_pass;

    task automatic test_Push_given_Rst_then_LocalBufferInit();
        reset_module();
        init_input_flag();
        test_pass = 1;
        repeat (1) @(posedge ap_clk);
        // CHECK necessary to check the FIFO status?
        for (i = 0; i < CORE_NUM; i = i + 1) begin
                assert(src_recv_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("src_recv_update_v_value init error!");
                    test_pass = 0;
                end
                assert(src_recv_update_v_valid[i] == 1'b0)
                else begin
                    $error("src_recv_update_v_valid init error");
                    test_pass = 0;
                end
                assert(backend_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("backend_active_v_id init error!");
                    test_pass = 0;
                end
                assert(backend_active_v_updated[i] == 1'b0)
                else begin
                    $error("backend_active_v_updated init error");
                    test_pass = 0;
                end
                assert(backend_active_v_pull_first_flag[i] == 1'b0)
                else begin
                    $error("backend_active_v_pull_first_flag init error");
                    test_pass = 0;
                end
                assert(backend_active_v_valid[i] == 1'b0)
                else begin
                    $error("backend_active_v_valid init error");
                    test_pass = 0;
                end
                assert(iteration_end[i] == 1'b0)
                else begin
                    $error("iteration_end init error");
                    test_pass = 0;
                end
                assert(iteration_end_valid[i] == 1'b0)
                else begin
                    $error("iteration_end_valid init error");
                    test_pass = 0;
                end
        end
            
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Push_given_Rst_then_LocalBufferInit");
    endtask
    
    task automatic test_Pull_given_RdAddrValid_then_ReadData();
        reset_module();
        init_input_flag();
        test_pass = 1;
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            rd_addr_src[i * VTX_BRAM_AWIDTH +: VTX_BRAM_AWIDTH] = i;
            rd_valid_src[i] = 1;
        end
        repeat (1) @(posedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
                assert(src_recv_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("src_recv_update_v_value init error!");
                    test_pass = 0;
                end
                assert(src_recv_update_v_valid[i] == 1'b0)
                else begin
                    $error("src_recv_update_v_valid init error");
                    test_pass = 0;
                end
                assert(backend_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("backend_active_v_id init error!");
                    test_pass = 0;
                end
                assert(backend_active_v_updated[i] == 1'b0)
                else begin
                    $error("backend_active_v_updated init error");
                    test_pass = 0;
                end
                assert(backend_active_v_pull_first_flag[i] == 1'b0)
                else begin
                    $error("backend_active_v_pull_first_flag init error");
                    test_pass = 0;
                end
                assert(backend_active_v_valid[i] == 1'b0)
                else begin
                    $error("backend_active_v_valid init error");
                    test_pass = 0;
                end
                assert(iteration_end[i] == 1'b0)
                else begin
                    $error("iteration_end init error");
                    test_pass = 0;
                end
                assert(iteration_end_valid[i] == 1'b0)
                else begin
                    $error("iteration_end_valid init error");
                    test_pass = 0;
                end
        end
        
        init_input_flag();
        repeat (2) @(posedge ap_clk);
        @(negedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
                assert(src_recv_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("src_recv_update_v_value init error!");
                    test_pass = 0;
                end
                assert(src_recv_update_v_valid[i] == 1'b1)
                else begin
                    $error("src_recv_update_v_valid init error");
                    test_pass = 0;
                end
                assert(backend_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("backend_active_v_id init error!");
                    test_pass = 0;
                end
                assert(backend_active_v_updated[i] == 1'b0)
                else begin
                    $error("backend_active_v_updated init error");
                    test_pass = 0;
                end
                assert(backend_active_v_pull_first_flag[i] == 1'b0)
                else begin
                    $error("backend_active_v_pull_first_flag init error");
                    test_pass = 0;
                end
                assert(backend_active_v_valid[i] == 1'b0)
                else begin
                    $error("backend_active_v_valid init error");
                    test_pass = 0;
                end
                assert(iteration_end[i] == 1'b0)
                else begin
                    $error("iteration_end init error");
                    test_pass = 0;
                end
                assert(iteration_end_valid[i] == 1'b0)
                else begin
                    $error("iteration_end_valid init error");
                    test_pass = 0;
                end
        end
        
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Pull_given_RdAddrValid_then_ReadData");
    endtask
    
    /*vtx_bram inst_vtx_bram_dut (
        .clk                            (ap_clk),
        .rst                            (ap_rst),
        .rd_addr_src                    (rd_addr_src),
        .rd_valid_src                   (rd_valid_src),
        .wr_push_flag_dst               (wr_push_flag_dst),
        .wr_addr_dst                    (wr_addr_dst),
        .wr_v_value_dst                 (wr_v_value_dst),
        .wr_pull_first_flag_dst         (wr_pull_first_flag_dst),
        .wr_valid_dst                   (wr_valid_dst),
        .front_iteration_end            (front_iteration_end),
        .front_iteration_end_valid      (front_iteration_end_valid),
        .front_iteration_id             (front_iteration_id),

        .next_rst                               (next_rst),
        .src_recv_update_v_value                (src_recv_update_v_value),
        .src_recv_update_v_valid                (src_recv_update_v_valid),
        .backend_active_v_id                    (backend_active_v_id),
        .backend_active_v_updated               (backend_active_v_updated),
        .backend_active_v_pull_first_flag       (backend_active_v_pull_first_flag),
        .backend_active_v_valid                 (backend_active_v_valid),
        .iteration_end                          (iteration_end),
        .iteration_end_valid                    (iteration_end_valid)
    );*/
    
   
    initial begin : vtx_bram_Test
        test_Push_given_Rst_then_LocalBufferInit();
        test_Pull_given_RdAddrValid_then_ReadData();
        repeat (10) @(posedge ap_clk);
        $finish;
    end

endmodule
