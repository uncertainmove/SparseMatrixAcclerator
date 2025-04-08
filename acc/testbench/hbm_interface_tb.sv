`timescale 1ns / 1ps

`include "accelerator.vh"

module hbm_interface_tb();

    parameter V_ID_WIDTH = `V_ID_WIDTH;
    parameter V_OFF_AWIDTH = `V_OFF_AWIDTH;
    parameter V_OFF_DWIDTH = `V_OFF_DWIDTH;
    parameter V_VALUE_WIDTH = `V_VALUE_WIDTH;
    parameter CORE_NUM = `CORE_NUM;
    parameter HBM_AWIDTH = `HBM_AWIDTH;
    parameter HBM_DWIDTH = `HBM_DWIDTH;
    parameter HBM_EDGE_MASK = `HBM_EDGE_MASK;
    parameter PSEUDO_CHANNEL_NUM = `PSEUDO_CHANNEL_NUM;
    parameter GROUP_CORE_NUM = `GROUP_CORE_NUM;
    
    parameter integer LP_CLK_PERIOD_PS = 5000; // 200 MHz

    logic ap_clk = 0;

    initial begin: AP_CLK
        forever begin
            ap_clk = #(LP_CLK_PERIOD_PS / 2) ~ap_clk;
        end
    end

    logic [CORE_NUM-1:0] ap_rst = 'h0;

    // module input
    reg [PSEUDO_CHANNEL_NUM * HBM_AWIDTH - 1 : 0]     front_rd_hbm_edge_addr;
    reg [PSEUDO_CHANNEL_NUM - 1 : 0]                  front_rd_hbm_edge_valid;
    reg [PSEUDO_CHANNEL_NUM * HBM_DWIDTH - 1 : 0]     hbm_controller_edge;
    reg [PSEUDO_CHANNEL_NUM - 1 : 0]                  hbm_controller_valid;
    reg [PSEUDO_CHANNEL_NUM - 1 : 0]                  hbm_controller_full;
    
    // module output
    wire [PSEUDO_CHANNEL_NUM - 1 : 0]                 stage_full;
    wire [PSEUDO_CHANNEL_NUM * HBM_AWIDTH - 1 : 0]    rd_hbm_edge_addr;
    wire [PSEUDO_CHANNEL_NUM - 1 : 0]                 rd_hbm_edge_valid;
    wire [CORE_NUM * HBM_AWIDTH - 1 : 0]              active_v_edge;
    wire [CORE_NUM - 1 : 0]                           active_v_edge_valid;

    task automatic init_input_flag();
        front_rd_hbm_edge_addr = 'h0;
        front_rd_hbm_edge_valid = 'h0;
        hbm_controller_edge = 'h0;
        hbm_controller_valid = 'h0;
        hbm_controller_full = 'h0;
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
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
                assert(stage_full[i] == 1'b0)
                else begin
                    $error("stage_full init error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
                else begin
                    $error("rd_hbm_edge_addr init error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_valid[i] == 1'b0)
                else begin
                    $error("rd_hbm_edge_valid init error");
                    test_pass = 0;
                end
        end
        
        for (i = 0; i < CORE_NUM; i = i + 1) begin
                assert(active_v_edge[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
                else begin
                    $error("active_v_edge init error!");
                    test_pass = 0;
                end
                assert(active_v_edge_valid[i] == 1'b0)
                else begin
                    $error("active_v_edge_valid init error");
                    test_pass = 0;
                end
        end
            
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Push_given_Rst_then_LocalBufferInit");
    endtask
    
    task automatic test_AddrInOut_given_AddrValid_then_OutputAddr();
        reset_module();
        init_input_flag();
        test_pass = 1;
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
            front_rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] = i;
            front_rd_hbm_edge_valid[i] = 1;
        end
        repeat (1) @(posedge ap_clk);
        // CHECK
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
                assert(stage_full[i] == 1'b0)
                else begin
                    $error("stage_full init error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
                else begin
                    $error("rd_hbm_edge_addr init error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_valid[i] == 1'b0)
                else begin
                    $error("rd_hbm_edge_valid init error");
                    test_pass = 0;
                end
        end
        
        for (i = 0; i < CORE_NUM; i = i + 1) begin
                assert(active_v_edge[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
                else begin
                    $error("active_v_edge init error!");
                    test_pass = 0;
                end
                assert(active_v_edge_valid[i] == 1'b0)
                else begin
                    $error("active_v_edge_valid init error");
                    test_pass = 0;
                end
        end
        init_input_flag();
        repeat (1) @(posedge ap_clk);
        @(negedge ap_clk);
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
                assert(stage_full[i] == 1'b0)
                else begin
                    $error("stage_full init error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == i)
                else begin
                    $error("rd_hbm_edge_addr init error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_valid[i] == 1'b1)
                else begin
                    $error("rd_hbm_edge_valid init error");
                    test_pass = 0;
                end
        end
        
        for (i = 0; i < CORE_NUM; i = i + 1) begin
                assert(active_v_edge[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
                else begin
                    $error("active_v_edge init error!");
                    test_pass = 0;
                end
                assert(active_v_edge_valid[i] == 1'b0)
                else begin
                    $error("active_v_edge_valid init error");
                    test_pass = 0;
                end
        end
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_AddrInOut_given_AddrValid_then_OutputAddr");
    endtask
    
    task automatic test_DataInOut_given_DataValid_then_OutputData();
        reset_module();
        init_input_flag();
        test_pass = 1;
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
            hbm_controller_edge[i * HBM_DWIDTH +: HBM_DWIDTH] = {16{32'b00000000000000000000000100000001}};
            hbm_controller_valid[i] = 1;
        end
        repeat (1) @(posedge ap_clk);
        // CHECK
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
                assert(stage_full[i] == 1'b0)
                else begin
                    $error("stage_full init error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
                else begin
                    $error("rd_hbm_edge_addr init error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_valid[i] == 1'b0)
                else begin
                    $error("rd_hbm_edge_valid init error");
                    test_pass = 0;
                end
        end
        
        for (i = 0; i < CORE_NUM; i = i + 1) begin
                assert(active_v_edge[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
                else begin
                    $error("active_v_edge init error!");
                    test_pass = 0;
                end
                assert(active_v_edge_valid[i] == 1'b0)
                else begin
                    $error("active_v_edge_valid init error");
                    test_pass = 0;
                end
        end
        init_input_flag();
        repeat (1) @(posedge ap_clk);
        @(negedge ap_clk);
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
                assert(stage_full[i] == 1'b0)
                else begin
                    $error("stage_full init error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
                else begin
                    $error("rd_hbm_edge_addr init error!");
                    test_pass = 0;
                end
                assert(rd_hbm_edge_valid[i] == 1'b0)
                else begin
                    $error("rd_hbm_edge_valid init error");
                    test_pass = 0;
                end
        end
        
        for (i = 0; i < CORE_NUM; i = i + 1) begin
                assert(active_v_edge[i * HBM_AWIDTH +: HBM_AWIDTH] == 32'b00000000000000000000000100000001)
                else begin
                    $error("active_v_edge init error!");
                    test_pass = 0;
                end
                assert(active_v_edge_valid[i] == 1'b1)
                else begin
                    $error("active_v_edge_valid init error");
                    test_pass = 0;
                end
        end
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_DataInOut_given_DataValid_then_OutputData");
    endtask
    
    hbm_interface inst_hbm_interface_dut (
        .clk                            (ap_clk),
        .rst                            (ap_rst),
        .front_rd_hbm_edge_addr         (front_rd_hbm_edge_addr),
        .front_rd_hbm_edge_valid        (front_rd_hbm_edge_valid),
        .hbm_controller_edge            (hbm_controller_edge),
        .hbm_controller_valid           (hbm_controller_valid),
        .hbm_controller_full            (hbm_controller_full),

        .stage_full                     (stage_full),
        .rd_hbm_edge_addr               (rd_hbm_edge_addr),
        .rd_hbm_edge_valid              (rd_hbm_edge_valid),
        .active_v_edge                  (active_v_edge),
        .active_v_edge_valid            (active_v_edge_valid)
    );
    
   
    initial begin : hbm_interface_Test
        test_Push_given_Rst_then_LocalBufferInit();
        test_AddrInOut_given_AddrValid_then_OutputAddr();
        test_DataInOut_given_DataValid_then_OutputData();
        repeat (10) @(posedge ap_clk);
        $finish;
    end

endmodule
