`timescale 1ns / 1ps

`include "accelerator.vh"

module M06_tb();

    parameter V_ID_WIDTH = `V_ID_WIDTH;
    parameter V_VALUE_WIDTH = `V_VALUE_WIDTH;
    parameter CORE_NUM = `CORE_NUM;
    parameter PSEUDO_CHANNEL_NUM = `PSEUDO_CHANNEL_NUM;
    parameter GROUP_CORE_NUM = `GROUP_CORE_NUM;
    parameter POWERLAW_VTX_NUM = `POWERLAW_VTX_NUM;
    
    parameter integer LP_CLK_PERIOD_PS = 5000; // 200 MHz

    logic ap_clk = 0;

    initial begin: AP_CLK
        forever begin
            ap_clk = #(LP_CLK_PERIOD_PS / 2) ~ap_clk;
        end
    end

    logic [CORE_NUM-1:0] ap_rst = 'h0;

    // module input
    reg [CORE_NUM - 1 : 0]                    front_push_flag;
    reg [CORE_NUM * V_ID_WIDTH - 1 : 0]       front_active_v_id;
    reg [CORE_NUM * V_VALUE_WIDTH - 1 : 0]    front_active_v_value;
    reg [CORE_NUM - 1 : 0]                    front_active_v_valid;
    reg [CORE_NUM - 1 : 0]                    front_iteration_end;
    reg [CORE_NUM - 1 : 0]                    front_iteration_end_valid;
    reg [CORE_NUM * V_ID_WIDTH - 1 : 0]       hbm_interface_active_v_edge;
    reg [CORE_NUM - 1 : 0]                    hbm_interface_active_v_edge_valid;
    reg [CORE_NUM - 1 : 0]                    front2_push_flag;
    reg [CORE_NUM * V_ID_WIDTH - 1 : 0]       front2_active_v_id;
    reg [CORE_NUM * V_VALUE_WIDTH - 1 : 0]    front2_active_v_value;
    reg [CORE_NUM * V_ID_WIDTH - 1 : 0]       front2_active_v_edge;
    reg [CORE_NUM - 1 : 0]                    front2_active_v_valid;
    reg [CORE_NUM - 1 : 0]                    front2_iteration_end;
    reg [CORE_NUM - 1 : 0]                    front2_iteration_end_valid;
    reg [CORE_NUM * POWERLAW_VTX_NUM - 1 : 0] front_global_powerlaw_v_visit;
    reg [CORE_NUM - 1 : 0]                    next_stage_full;

    // module output
    wire [CORE_NUM - 1 : 0]                   next_rst;
    wire [PSEUDO_CHANNEL_NUM - 1 : 0]         stage_full1;
    wire [CORE_NUM - 1 : 0]                   stage_full2;
    wire [CORE_NUM - 1 : 0]                   iteration_end;
    wire [CORE_NUM - 1 : 0]                   iteration_end_valid;
    wire [CORE_NUM - 1 : 0]                   push_flag;
    wire [CORE_NUM * V_ID_WIDTH - 1 : 0]      update_v_id;
    wire [CORE_NUM * V_VALUE_WIDTH - 1 : 0]   update_v_value;
    wire [CORE_NUM - 1 : 0]                   pull_first_flag;
    wire [CORE_NUM - 1 : 0]                   update_v_valid;
    
    task automatic init_input_flag();
        front_push_flag = 'h0;
        front_active_v_id = 'h0;
        front_active_v_value = 'h0;
        front_active_v_valid = 'h0;
        front_iteration_end = 'h0;
        front_iteration_end_valid = 'h0;
        hbm_interface_active_v_edge = 'h0;
        hbm_interface_active_v_edge_valid = 'h0;
        front2_push_flag = 'h0;
        front2_active_v_id = 'h0;
        front2_active_v_value = 'h0;
        front2_active_v_edge = 'h0;
        front2_active_v_valid = 'h0;
        front2_iteration_end = 'h0;
        front2_iteration_end_valid = 'h0;
        front_global_powerlaw_v_visit = 'h0;
        next_stage_full = 'h0;
    endtask

    task automatic reset_module();
        ap_rst = {512{1'b1}};
        init_input_flag();
        repeat (10) @(posedge ap_clk);
        ap_rst = 'h0;
    endtask

    integer i,j,k,t;
    logic test_pass;
    
    task automatic test_PushPull_given_Rst_then_LocalBufferInit();
        reset_module();
        init_input_flag();
        test_pass = 1;
        repeat (1) @(posedge ap_clk);
        
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(stage_full1[i%32] == 1'b0)
            else begin
                $error("stage_full1 init error!");
                test_pass = 0;
            end
            assert(stage_full2[i] == 1'b0)
            else begin
                $error("stage_full2 init error!");
                test_pass = 0;
            end
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag init error!");
                test_pass = 0;
            end
            assert(update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("update_v_id init error!");
                test_pass = 0;
            end
            assert(update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("update_v_value init error");
                test_pass = 0;
            end
            assert(pull_first_flag[i] == 1'b0)
            else begin
                $error("pull_first_flag init error");
                test_pass = 0;
            end
            assert(update_v_valid[i] == 1'b0)
            else begin
                $error("update_v_valid init error");
                test_pass = 0;
            end
            assert(iteration_end[i] == 1'b0)
            else begin
                $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
            end
            assert(iteration_end_valid[i] == 1'b0)
            else begin
                $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
        end
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_PushPull_given_Rst_then_LocalBufferInit");
    endtask
    
    task automatic test_Pull_given_Front2Invalid_then_StoreAndOutput();
        reset_module();
        init_input_flag();
        test_pass = 1;
        
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            front2_push_flag[i] = 0;
            front2_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i;
            front2_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] = 1;
            front2_active_v_edge[i * V_ID_WIDTH +: V_ID_WIDTH] = i+2;
            front2_active_v_valid[i] = 1;
        end
        repeat (1) @(posedge ap_clk);
        @(negedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(stage_full1[i%32] == 1'b0)
            else begin
                $error("stage_full1 init error!");
                test_pass = 0;
            end
            assert(stage_full2[i] == 1'b0)
            else begin
                $error("stage_full2 init error!");
                test_pass = 0;
            end
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag init error!");
                test_pass = 0;
            end
            assert(update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("update_v_id init error!");
                test_pass = 0;
            end
            assert(update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("update_v_value init error");
                test_pass = 0;
            end
            assert(pull_first_flag[i] == 1'b0)
            else begin
                $error("pull_first_flag init error");
                test_pass = 0;
            end
            assert(update_v_valid[i] == 1'b0)
            else begin
                $error("update_v_valid init error");
                test_pass = 0;
            end
            assert(iteration_end[i] == 1'b0)
            else begin
                $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
            end
            assert(iteration_end_valid[i] == 1'b0)
            else begin
                $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
        end
        repeat (3) @(posedge ap_clk);
        @(negedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(stage_full1[i%32] == 1'b0)
            else begin
                $error("stage_full1 init error!");
                test_pass = 0;
            end
            assert(stage_full2[i] == 1'b0)
            else begin
                $error("stage_full2 init error!");
                test_pass = 0;
            end
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag init error!");
                test_pass = 0;
            end
            assert(update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == i)
            else begin
                $error("update_v_id init error!");
                test_pass = 0;
            end
            assert(update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == i+2)
            else begin
                $error("update_v_value init error");
                test_pass = 0;
            end
            assert(pull_first_flag[i] == 1'b1)
            else begin
                $error("pull_first_flag init error");
                test_pass = 0;
            end
            assert(update_v_valid[i] == 1'b1)
            else begin
                $error("update_v_valid init error");
                test_pass = 0;
            end
            assert(iteration_end[i] == 1'b0)
            else begin
                $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
            end
            assert(iteration_end_valid[i] == 1'b0)
            else begin
                $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
        end
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Pull_given_Front2Invalid_then_StoreAndOutput");
    endtask
    
    task automatic test_Push_given_FrontInvalid_then_StoreAndOutput();
        reset_module();
        init_input_flag();
        test_pass = 1;
        
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            front_push_flag[i] = 1;
            front_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i;
            front_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] = 1;
            front_active_v_valid[i] = 1;
        end
        repeat (1) @(posedge ap_clk);
        @(negedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(stage_full1[i%32] == 1'b0)
            else begin
                $error("stage_full1 init error!");
                test_pass = 0;
            end
            assert(stage_full2[i] == 1'b0)
            else begin
                $error("stage_full2 init error!");
                test_pass = 0;
            end
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag init error!");
                test_pass = 0;
            end
            assert(update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("update_v_id init error!");
                test_pass = 0;
            end
            assert(update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("update_v_value init error");
                test_pass = 0;
            end
            assert(pull_first_flag[i] == 1'b0)
            else begin
                $error("pull_first_flag init error");
                test_pass = 0;
            end
            assert(update_v_valid[i] == 1'b0)
            else begin
                $error("update_v_valid init error");
                test_pass = 0;
            end
            assert(iteration_end[i] == 1'b0)
            else begin
                $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
            end
            assert(iteration_end_valid[i] == 1'b0)
            else begin
                $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
        end
        init_input_flag();
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            hbm_interface_active_v_edge[i * V_ID_WIDTH +: V_ID_WIDTH] = i+1;
            hbm_interface_active_v_edge_valid[i] = 1;
        end
        repeat (1) @(posedge ap_clk);
        @(negedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(stage_full1[i%32] == 1'b0)
            else begin
                $error("stage_full1 init error!");
                test_pass = 0;
            end
            assert(stage_full2[i] == 1'b0)
            else begin
                $error("stage_full2 init error!");
                test_pass = 0;
            end
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag init error!");
                test_pass = 0;
            end
            assert(update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("update_v_id init error!");
                test_pass = 0;
            end
            assert(update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("update_v_value init error");
                test_pass = 0;
            end
            assert(pull_first_flag[i] == 1'b0)
            else begin
                $error("pull_first_flag init error");
                test_pass = 0;
            end
            assert(update_v_valid[i] == 1'b0)
            else begin
                $error("update_v_valid init error");
                test_pass = 0;
            end
            assert(iteration_end[i] == 1'b0)
            else begin
                $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
            end
            assert(iteration_end_valid[i] == 1'b0)
            else begin
                $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
        end
        init_input_flag();
        repeat (3) @(posedge ap_clk);
        @(negedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(stage_full1[i%32] == 1'b0)
            else begin
                $error("stage_full1 init error!");
                test_pass = 0;
            end
            assert(stage_full2[i] == 1'b0)
            else begin
                $error("stage_full2 init error!");
                test_pass = 0;
            end
            assert(push_flag[i] == 1'b1)
            else begin
                $error("push_flag init error!");
                test_pass = 0;
            end
            assert(update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == i+1)
            else begin
                $error("update_v_id init error!");
                test_pass = 0;
            end
            assert(update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 1)
            else begin
                $error("update_v_value init error");
                test_pass = 0;
            end
            assert(pull_first_flag[i] == 1'b0)
            else begin
                $error("pull_first_flag init error");
                test_pass = 0;
            end
            assert(update_v_valid[i] == 1'b1)
            else begin
                $error("update_v_valid init error");
                test_pass = 0;
            end
            assert(iteration_end[i] == 1'b0)
            else begin
                $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
            end
            assert(iteration_end_valid[i] == 1'b0)
            else begin
                $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
        end
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Push_given_FrontInvalid_then_StoreAndOutput");
    endtask
    
    task automatic test_InOut_given_DataValid_then_OutputValid();
        reset_module();
        init_input_flag();
        test_pass = 1;
        
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            front_push_flag[i] = 1;
            front_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = (i%2 ? -1 : 0);
            front_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] = 1;
            front_active_v_valid[i] = 1;
            front_iteration_end[i] = 0;
            front_iteration_end_valid[i] = 0;
            hbm_interface_active_v_edge[i * V_ID_WIDTH +: V_ID_WIDTH] = 0;
            hbm_interface_active_v_edge_valid[i] = 0;
        end
        repeat (1) @(posedge ap_clk);
        @(negedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(stage_full1[i%32] == 1'b0)
            else begin
                $error("stage_full1 init error!");
                test_pass = 0;
            end
            assert(stage_full2[i] == 1'b0)
            else begin
                $error("stage_full2 init error!");
                test_pass = 0;
            end
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag init error!");
                test_pass = 0;
            end
            assert(update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("update_v_id init error!");
                test_pass = 0;
            end
            assert(update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("update_v_value init error");
                test_pass = 0;
            end
            assert(pull_first_flag[i] == 1'b0)
            else begin
                $error("pull_first_flag init error");
                test_pass = 0;
            end
            assert(update_v_valid[i] == 1'b0)
            else begin
                $error("update_v_valid init error");
                test_pass = 0;
            end
            assert(iteration_end[i] == 1'b0)
            else begin
                $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
            end
            assert(iteration_end_valid[i] == 1'b0)
            else begin
                $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
        end
        init_input_flag();
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            hbm_interface_active_v_edge[i * V_ID_WIDTH +: V_ID_WIDTH] = i+1;
            hbm_interface_active_v_edge_valid[i] = 1;
        end
        repeat (1) @(posedge ap_clk);
        @(negedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(stage_full1[i%32] == 1'b0)
            else begin
                $error("stage_full1 init error!");
                test_pass = 0;
            end
            assert(stage_full2[i] == 1'b0)
            else begin
                $error("stage_full2 init error!");
                test_pass = 0;
            end
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag init error!");
                test_pass = 0;
            end
            assert(update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("update_v_id init error!");
                test_pass = 0;
            end
            assert(update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("update_v_value init error");
                test_pass = 0;
            end
            assert(pull_first_flag[i] == 1'b0)
            else begin
                $error("pull_first_flag init error");
                test_pass = 0;
            end
            assert(update_v_valid[i] == 1'b0)
            else begin
                $error("update_v_valid init error");
                test_pass = 0;
            end
            assert(iteration_end[i] == 1'b0)
            else begin
                $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
            end
            assert(iteration_end_valid[i] == 1'b0)
            else begin
                $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
        end
        init_input_flag();
        repeat (3) @(posedge ap_clk);
        @(negedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            if(i%2)begin
                assert(stage_full1[i%32] == 1'b0)
                else begin
                    $error("stage_full1 init error!");
                    test_pass = 0;
                end
                assert(stage_full2[i] == 1'b0)
                else begin
                    $error("stage_full2 init error!");
                    test_pass = 0;
                end
                assert(push_flag[i] == 1'b0)
                else begin
                    $error("push_flag init error!");
                    test_pass = 0;
                end
                assert(update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("update_v_id init error!");
                    test_pass = 0;
                end
                assert(update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("update_v_value init error");
                    test_pass = 0;
                end
                assert(pull_first_flag[i] == 1'b0)
                else begin
                    $error("pull_first_flag init error");
                    test_pass = 0;
                end
                assert(update_v_valid[i] == 1'b0)
                else begin
                    $error("update_v_valid init error");
                    test_pass = 0;
                end
                assert(iteration_end[i] == 1'b0)
                else begin
                    $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
                end
                assert(iteration_end_valid[i] == 1'b0)
                else begin
                    $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                    test_pass = 0;
                end
            end
            else begin
                assert(stage_full1[i%32] == 1'b0)
                else begin
                    $error("stage_full1 init error!");
                    test_pass = 0;
                end
                assert(stage_full2[i] == 1'b0)
                else begin
                    $error("stage_full2 init error!");
                    test_pass = 0;
                end
                assert(push_flag[i] == 1'b1)
                else begin
                    $error("push_flag init error!");
                    test_pass = 0;
                end
                assert(update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == i+1)
                else begin
                    $error("update_v_id init error!");
                    test_pass = 0;
                end
                assert(update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 1)
                else begin
                    $error("update_v_value init error");
                    test_pass = 0;
                end
                assert(pull_first_flag[i] == 1'b0)
                else begin
                    $error("pull_first_flag init error");
                    test_pass = 0;
                end
                assert(update_v_valid[i] == 1'b1)
                else begin
                    $error("update_v_valid init error");
                    test_pass = 0;
                end
                assert(iteration_end[i] == 1'b0)
                else begin
                    $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b0);
                end
                assert(iteration_end_valid[i] == 1'b0)
                else begin
                    $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b0);
                    test_pass = 0;
                end
            end
        end
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_InOut_given_DataValid_then_OutputValid");
    endtask
    
    /*scheduler inst_M06_dut (
        .clk                                    (ap_clk),
        .rst                                    (ap_rst),
        .front_push_flag                        (front_push_flag),
        .front_active_v_id                      (front_active_v_id),
        .front_active_v_value                   (front_active_v_value),
        .front_active_v_valid                   (front_active_v_valid),
        .hbm_interface_active_v_edge            (hbm_interface_active_v_edge),
        .hbm_interface_active_v_edge_valid      (hbm_interface_active_v_edge_valid),
        .front_iteration_end                    (front_iteration_end),
        .front_iteration_end_valid              (front_iteration_end_valid),
        .front2_push_flag                       (front2_push_flag),
        .front2_active_v_id                     (front2_active_v_id),
        .front2_active_v_value                  (front2_active_v_value),
        .front2_active_v_edge                   (front2_active_v_edge),
        .front2_active_v_valid                  (front2_active_v_valid),
        .front2_iteration_end                   (front2_iteration_end),
        .front2_iteration_end_valid             (front2_iteration_end_valid),
        .front_global_powerlaw_v_visit          (front_global_powerlaw_v_visit),
        .next_stage_full                        (next_stage_full),

        .next_rst                               (next_rst),
        .stage_full1                            (stage_full1),
        .stage_full2                            (stage_full2),
        .push_flag                              (push_flag),
        .update_v_id                            (update_v_id),
        .update_v_value                         (update_v_value),
        .pull_first_flag                        (pull_first_flag),
        .update_v_valid                         (update_v_valid),
        .iteration_end                          (iteration_end),
        .iteration_end_valid                    (iteration_end_valid)
    );*/

    initial begin : scheduler_Test
        test_PushPull_given_Rst_then_LocalBufferInit();
        test_Pull_given_Front2Invalid_then_StoreAndOutput();
        test_Push_given_FrontInvalid_then_StoreAndOutput();
        test_InOut_given_DataValid_then_OutputValid();
        repeat (10) @(posedge ap_clk);
        $finish;
    end
endmodule
