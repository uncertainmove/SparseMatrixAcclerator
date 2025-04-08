`timescale 1ns / 1ps

`include "accelerator.vh"

module M02_tb ();

    parameter integer V_ID_WIDTH = `V_ID_WIDTH;
    parameter integer V_OFF_AWIDTH = `V_OFF_AWIDTH;
    parameter integer V_OFF_DWIDTH = `V_OFF_DWIDTH;
    parameter integer V_VALUE_WIDTH = `V_VALUE_WIDTH;
    parameter integer CORE_NUM = `CORE_NUM;
    parameter integer CORE_NUM_WIDTH = `CORE_NUM_WIDTH;

    parameter integer LP_CLK_PERIOD_PS = 5000; // 200 MHz

    logic ap_clk = 1;

    initial begin: AP_CLK
        forever begin
            ap_clk = #(LP_CLK_PERIOD_PS / 2) ~ap_clk;
        end
    end

    logic ap_rst = 0;

    // module input
    reg [CORE_NUM - 1 : 0]                  front_push_flag;
    reg [CORE_NUM * V_ID_WIDTH - 1 : 0]     front_active_v_id;
    reg [CORE_NUM * V_VALUE_WIDTH - 1 : 0]  front_active_v_value;
    reg [CORE_NUM - 1 : 0]                  front_active_v_pull_first_flag;
    reg [CORE_NUM - 1 : 0]                  front_active_v_valid;
    reg [CORE_NUM - 1 : 0]                  front_iteration_end;
    reg [CORE_NUM - 1 : 0]                  front_iteration_end_valid;
    reg [CORE_NUM - 1 : 0]                  next_stage_full;
    // module output
    wire [CORE_NUM - 1 : 0]                 next_rst;
    wire [CORE_NUM - 1 : 0]                 stage_full;
    wire [CORE_NUM - 1 : 0]                 push_flag;
    wire [CORE_NUM * V_ID_WIDTH - 1 : 0]    active_v_id;
    wire [CORE_NUM * V_VALUE_WIDTH - 1 : 0] active_v_value;
    wire [CORE_NUM * V_OFF_AWIDTH - 1 : 0]  rd_active_v_offset_addr;
    wire [CORE_NUM - 1 : 0]                 active_v_pull_first_flag;
    wire [CORE_NUM - 1 : 0]                 active_v_id_valid;
    wire [CORE_NUM - 1 : 0]                 iteration_end;
    wire [CORE_NUM - 1 : 0]                 iteration_end_valid;

    task automatic init_input_flag();
        front_push_flag = 'h0;
        front_active_v_id = 'h0;
        front_active_v_value = 'h0;
        front_active_v_pull_first_flag = 'h0;
        front_active_v_valid = 'h0;
        front_iteration_end = 'h0;
        front_iteration_end_valid = 'h0;
        next_stage_full = 'h0;
    endtask

    task automatic reset_module();
        ap_rst = 1;
        init_input_flag();
        repeat (10) @(posedge ap_clk);
        ap_rst = 0;
        // TODO add check point?
    endtask

    integer i;
    logic test_pass;

    task automatic test_PushPull_given_Rst_then_LocalBufferInit();
        reset_module();
        init_input_flag();
        test_pass = 1;
        repeat (1) @(posedge ap_clk);
        // note: all signals check in the negtive edge of ap_clk
        @(negedge ap_clk);
        // CHECK necessary to check the FIFO status?
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(stage_full[i] == 1'b0)
            else begin
                $error("stage_full[%3d] is 0x%0x, expect 0x%0x!", i, stage_full[i], 1'b0);
                test_pass = 0;
            end
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag[%3d] is 0x%0x, expect 0x%0x!", i, push_flag[i], 1'b0);
                test_pass = 0;
            end
            assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("active_v_id[%3d] is 0x%0x, expect 0x%0x!", i, active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH], 'h0);
                test_pass = 0;
            end
            assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("active_v_value[%3d] is 0x%0x, expect 0x%0x!", i, active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH], 'h0);
                test_pass = 0;
            end
            assert(rd_active_v_offset_addr[i * V_OFF_AWIDTH +: V_OFF_AWIDTH] == 'h0)
            else begin
                $error("rd_active_v_offset_addr[%3d] is 0x%0x, expect 0x%0x!", i, rd_active_v_offset_addr[i * V_OFF_AWIDTH +: V_OFF_AWIDTH], 'h0);
                test_pass = 0;
            end
            assert(active_v_id_valid[i] == 1'b0)
            else begin
                $error("active_v_id_valid[%3d] is 0x%0x, expect 0x%0x!", i, active_v_id_valid[i], 1'b0);
                test_pass = 0;
            end
            assert(active_v_pull_first_flag[i] == 1'b0)
            else begin
                $error("active_v_pull_first_flag[%3d] is 0x%0x, expect 0x%0x!", i, active_v_pull_first_flag[i], 1'b0);
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
        if (test_pass)  $display("[PASSED] test_PushPull_given_Rst_then_LocalBufferInit");
    endtask

    task automatic test_Push_given_InputActiveId_then_OutputActiveId();
        reset_module();
        init_input_flag();
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            front_push_flag[i] = 1'b1;
            front_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i;
            front_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] = 'h1;
            front_active_v_valid[i] = 1'b1;
        end
        test_pass = 1;
        repeat (1) @(posedge ap_clk);
        // note: all signals check in the negtive edge of ap_clk
        @(negedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(stage_full[i] == 1'b0)
            else begin
                $error("stage_full[%3d] is 0x%0x, expect 0x%0x!", i, stage_full[i], 1'b0);
                test_pass = 0;
            end
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag[%3d] is 0x%0x, expect 0x%0x!", i, push_flag[i], 1'b0);
                test_pass = 0;
            end
            assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("active_v_id[%3d] is 0x%0x, expect 0x%0x!", i, active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH], 'h0);
                test_pass = 0;
            end
            assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("active_v_value[%3d] is 0x%0x, expect 0x%0x!", i, active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH], 'h0);
                test_pass = 0;
            end
            assert(rd_active_v_offset_addr[i * V_OFF_AWIDTH +: V_OFF_AWIDTH] == 'h0)
            else begin
                $error("rd_active_v_offset_addr[%3d] is 0x%0x, expect 0x%0x!", i, rd_active_v_offset_addr[i * V_OFF_AWIDTH +: V_OFF_AWIDTH], 'h0);
                test_pass = 0;
            end
            assert(active_v_id_valid[i] == 1'b0)
            else begin
                $error("active_v_id_valid[%3d] is 0x%0x, expect 0x%0x!", i, active_v_id_valid[i], 1'b0);
                test_pass = 0;
            end
            assert(active_v_pull_first_flag[i] == 1'b0)
            else begin
                $error("active_v_pull_first_flag[%3d] is 0x%0x, expect 0x%0x!", i, active_v_pull_first_flag[i], 1'b0);
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
        repeat (1) @(posedge ap_clk);
        // note: all signals check in the negtive edge of ap_clk
        @(negedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(stage_full[i] == 1'b0)
            else begin
                $error("stage_full[%3d] is 0x%0x, expect 0x%0x!", i, stage_full[i], 1'b0);
                test_pass = 0;
            end
            assert(push_flag[i] == 1'b1)
            else begin
                $error("push_flag[%3d] is 0x%0x, expect 0x%0x!", i, push_flag[i], 1'b1);
                test_pass = 0;
            end
            assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == i)
            else begin
                $error("active_v_id[%3d] is 0x%0x, expect 0x%0x!", i, active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH], i);
                test_pass = 0;
            end
            assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h1)
            else begin
                $error("active_v_value[%3d] is 0x%0x, expect 0x%0x!", i, active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH], 'h1);
                test_pass = 0;
            end
            assert(rd_active_v_offset_addr[i * V_OFF_AWIDTH +: V_OFF_AWIDTH] == i / CORE_NUM)
            else begin
                $error("rd_active_v_offset_addr[%3d] is 0x%0x, expect 0x%0x!", i, rd_active_v_offset_addr[i * V_OFF_AWIDTH +: V_OFF_AWIDTH], i / CORE_NUM);
                test_pass = 0;
            end
            assert(active_v_id_valid[i] == 1'b1)
            else begin
                $error("active_v_id_valid[%3d] is 0x%0x, expect 0x%0x!", i, active_v_id_valid[i], 1'b1);
                test_pass = 0;
            end
            assert(active_v_pull_first_flag[i] == 1'b0)
            else begin
                $error("active_v_pull_first_flag[%3d] is 0x%0x, expect 0x%0x!", i, active_v_pull_first_flag[i], 1'b0);
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
        if (test_pass)  $display("[PASSED] test_Push_given_InputActiveId_then_OutputActiveId");
    endtask

    task automatic test_FirstEdgePull_given_InputActiveId_then_OutputActiveId();
        reset_module();
        init_input_flag();
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            front_push_flag[i] = 1'b0;
            front_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i;
            front_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] = 'h1;
            front_active_v_valid[i] = 1'b1;
            front_active_v_pull_first_flag[i] = 1'b1;
        end
        test_pass = 1;
        repeat (1) @(posedge ap_clk);
        // note: all signals check in the negtive edge of ap_clk
        @(negedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(stage_full[i] == 1'b0)
            else begin
                $error("stage_full[%3d] is 0x%0x, expect 0x%0x!", i, stage_full[i], 1'b0);
                test_pass = 0;
            end
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag[%3d] is 0x%0x, expect 0x%0x!", i, push_flag[i], 1'b0);
                test_pass = 0;
            end
            assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("active_v_id[%3d] is 0x%0x, expect 0x%0x!", i, active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH], 'h0);
                test_pass = 0;
            end
            assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("active_v_value[%3d] is 0x%0x, expect 0x%0x!", i, active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH], 'h0);
                test_pass = 0;
            end
            assert(rd_active_v_offset_addr[i * V_OFF_AWIDTH +: V_OFF_AWIDTH] == 'h0)
            else begin
                $error("rd_active_v_offset_addr[%3d] is 0x%0x, expect 0x%0x!", i, rd_active_v_offset_addr[i * V_OFF_AWIDTH +: V_OFF_AWIDTH], 'h0);
                test_pass = 0;
            end
            assert(active_v_id_valid[i] == 1'b0)
            else begin
                $error("active_v_id_valid[%3d] is 0x%0x, expect 0x%0x!", i, active_v_id_valid[i], 1'b0);
                test_pass = 0;
            end
            assert(active_v_pull_first_flag[i] == 1'b0)
            else begin
                $error("active_v_pull_first_flag[%3d] is 0x%0x, expect 0x%0x!", i, active_v_pull_first_flag[i], 1'b0);
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
        repeat (1) @(posedge ap_clk);
        // note: all signals check in the negtive edge of ap_clk
        @(negedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(stage_full[i] == 1'b0)
            else begin
                $error("stage_full[%3d] is 0x%0x, expect 0x%0x!", i, stage_full[i], 1'b0);
                test_pass = 0;
            end
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag[%3d] is 0x%0x, expect 0x%0x!", i, push_flag[i], 1'b0);
                test_pass = 0;
            end
            assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == i)
            else begin
                $error("active_v_id[%3d] is 0x%0x, expect 0x%0x!", i, active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH], i);
                test_pass = 0;
            end
            assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h1)
            else begin
                $error("active_v_value[%3d] is 0x%0x, expect 0x%0x!", i, active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH], 'h1);
                test_pass = 0;
            end
            assert(rd_active_v_offset_addr[i * V_OFF_AWIDTH +: V_OFF_AWIDTH] == i / CORE_NUM)
            else begin
                $error("rd_active_v_offset_addr[%3d] is 0x%0x, expect 0x%0x!", i, rd_active_v_offset_addr[i * V_OFF_AWIDTH +: V_OFF_AWIDTH], i / CORE_NUM);
                test_pass = 0;
            end
            assert(active_v_id_valid[i] == 1'b1)
            else begin
                $error("active_v_id_valid[%3d] is 0x%0x, expect 0x%0x!", i, active_v_id_valid[i], 1'b1);
                test_pass = 0;
            end
            assert(active_v_pull_first_flag[i] == 1'b1)
            else begin
                $error("active_v_pull_first_flag[%3d] is 0x%0x, expect 0x%0x!", i, active_v_pull_first_flag[i], 1'b1);
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
        if (test_pass)  $display("[PASSED] test_FirstEdgePull_given_InputActiveId_then_OutputActiveId");
    endtask

    task automatic test_NotFirstEdgePull_given_InputActiveId_then_OutputActiveId();
        reset_module();
        init_input_flag();
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            front_push_flag[i] = 1'b0;
            front_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i;
            front_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] = 'h1;
            front_active_v_valid[i] = 1'b1;
            front_active_v_pull_first_flag[i] = 1'b0;
        end
        test_pass = 1;
        repeat (1) @(posedge ap_clk);
        // note: all signals check in the negtive edge of ap_clk
        @(negedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(stage_full[i] == 1'b0)
            else begin
                $error("stage_full[%3d] is 0x%0x, expect 0x%0x!", i, stage_full[i], 1'b0);
                test_pass = 0;
            end
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag[%3d] is 0x%0x, expect 0x%0x!", i, push_flag[i], 1'b0);
                test_pass = 0;
            end
            assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("active_v_id[%3d] is 0x%0x, expect 0x%0x!", i, active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH], 'h0);
                test_pass = 0;
            end
            assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("active_v_value[%3d] is 0x%0x, expect 0x%0x!", i, active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH], 'h0);
                test_pass = 0;
            end
            assert(rd_active_v_offset_addr[i * V_OFF_AWIDTH +: V_OFF_AWIDTH] == 'h0)
            else begin
                $error("rd_active_v_offset_addr[%3d] is 0x%0x, expect 0x%0x!", i, rd_active_v_offset_addr[i * V_OFF_AWIDTH +: V_OFF_AWIDTH], 'h0);
                test_pass = 0;
            end
            assert(active_v_id_valid[i] == 1'b0)
            else begin
                $error("active_v_id_valid[%3d] is 0x%0x, expect 0x%0x!", i, active_v_id_valid[i], 1'b0);
                test_pass = 0;
            end
            assert(active_v_pull_first_flag[i] == 1'b0)
            else begin
                $error("active_v_pull_first_flag[%3d] is 0x%0x, expect 0x%0x!", i, active_v_pull_first_flag[i], 1'b0);
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
        repeat (1) @(posedge ap_clk);
        // note: all signals check in the negtive edge of ap_clk
        @(negedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(stage_full[i] == 1'b0)
            else begin
                $error("stage_full[%3d] is 0x%0x, expect 0x%0x!", i, stage_full[i], 1'b0);
                test_pass = 0;
            end
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag[%3d] is 0x%0x, expect 0x%0x!", i, push_flag[i], 1'b0);
                test_pass = 0;
            end
            assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == i)
            else begin
                $error("active_v_id[%3d] is 0x%0x, expect 0x%0x!", i, active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH], i);
                test_pass = 0;
            end
            assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h1)
            else begin
                $error("active_v_value[%3d] is 0x%0x, expect 0x%0x!", i, active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH], 'h1);
                test_pass = 0;
            end
            assert(rd_active_v_offset_addr[i * V_OFF_AWIDTH +: V_OFF_AWIDTH] == i / CORE_NUM)
            else begin
                $error("rd_active_v_offset_addr[%3d] is 0x%0x, expect 0x%0x!", i, rd_active_v_offset_addr[i * V_OFF_AWIDTH +: V_OFF_AWIDTH], i / CORE_NUM);
                test_pass = 0;
            end
            assert(active_v_id_valid[i] == 1'b1)
            else begin
                $error("active_v_id_valid[%3d] is 0x%0x, expect 0x%0x!", i, active_v_id_valid[i], 1'b1);
                test_pass = 0;
            end
            assert(active_v_pull_first_flag[i] == 1'b0)
            else begin
                $error("active_v_pull_first_flag[%3d] is 0x%0x, expect 0x%0x!", i, active_v_pull_first_flag[i], 1'b0);
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
        if (test_pass)  $display("[PASSED] test_NotFirstEdgePull_given_InputActiveId_then_OutputActiveId");
    endtask

    task automatic test_Push_given_IterationEnd_while_BufferEmpty_then_OutputIterationEnd();
        reset_module();
        init_input_flag();
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            front_iteration_end[i] = 1'b1;
            front_iteration_end_valid[i] = 1'b1;
        end
        test_pass = 1;
        repeat (1) @(posedge ap_clk);
        // note: all signals check in the negtive edge of ap_clk
        @(negedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(stage_full[i] == 1'b0)
            else begin
                $error("stage_full[%3d] is 0x%0x, expect 0x%0x!", i, stage_full[i], 1'b0);
                test_pass = 0;
            end
            assert(push_flag[i] == 1'b0)
            else begin
                $error("push_flag[%3d] is 0x%0x, expect 0x%0x!", i, push_flag[i], 1'b0);
                test_pass = 0;
            end
            assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("active_v_id[%3d] is 0x%0x, expect 0x%0x!", i, active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH], 'h0);
                test_pass = 0;
            end
            assert(active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("active_v_value[%3d] is 0x%0x, expect 0x%0x!", i, active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH], 'h0);
                test_pass = 0;
            end
            assert(rd_active_v_offset_addr[i * V_OFF_AWIDTH +: V_OFF_AWIDTH] == 'h0)
            else begin
                $error("rd_active_v_offset_addr[%3d] is 0x%0x, expect 0x%0x!", i, rd_active_v_offset_addr[i * V_OFF_AWIDTH +: V_OFF_AWIDTH], 'h0);
                test_pass = 0;
            end
            assert(active_v_id_valid[i] == 1'b0)
            else begin
                $error("active_v_id_valid[%3d] is 0x%0x, expect 0x%0x!", i, active_v_id_valid[i], 1'b0);
                test_pass = 0;
            end
            assert(active_v_pull_first_flag[i] == 1'b0)
            else begin
                $error("active_v_pull_first_flag[%3d] is 0x%0x, expect 0x%0x!", i, active_v_pull_first_flag[i], 1'b0);
                test_pass = 0;
            end
            assert(iteration_end[i] == 1'b1)
            else begin
                $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end[i], 1'b1);
            end
            assert(iteration_end_valid[i] == 1'b1)
            else begin
                $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, iteration_end_valid[i], 1'b1);
                test_pass = 0;
            end
        end
        if (test_pass)  $display("[PASSED] test_Push_given_IterationEnd_while_BufferEmpty_then_OutputIterationEnd");
    endtask

    // DUT instantiation
    rd_active_vertex_offset #(
        .V_ID_WIDTH     (V_ID_WIDTH),
        .V_OFF_AWIDTH   (V_OFF_AWIDTH),
        .V_OFF_DWIDTH   (V_OFF_DWIDTH),
        .V_VALUE_WIDTH  (V_VALUE_WIDTH),
        .CORE_NUM       (CORE_NUM)
    )
    inst_M02_dut (
        .clk                            (ap_clk),
        .rst                            ({CORE_NUM{ap_rst}}),
        .front_push_flag                (front_push_flag),
        .front_active_v_id              (front_active_v_id),
        .front_active_v_value           (front_active_v_value),
        .front_active_v_pull_first_flag (front_active_v_pull_first_flag),
        .front_active_v_valid           (front_active_v_valid),
        .front_iteration_end            (front_iteration_end),
        .front_iteration_end_valid      (front_iteration_end_valid),
        .next_stage_full                (next_stage_full),

        .next_rst                   (next_rst),
        .stage_full                 (stage_full),
        .push_flag                  (push_flag),
        .active_v_id                (active_v_id),
        .active_v_value             (active_v_value),
        .rd_active_v_offset_addr    (rd_active_v_offset_addr),
        .active_v_pull_first_flag   (active_v_pull_first_flag),
        .active_v_id_valid          (active_v_id_valid),
        .iteration_end              (iteration_end),
        .iteration_end_valid        (iteration_end_valid)
    );

    initial begin : Read_Active_Vertex_Offset_Test
        test_PushPull_given_Rst_then_LocalBufferInit();
        test_Push_given_InputActiveId_then_OutputActiveId();
        test_FirstEdgePull_given_InputActiveId_then_OutputActiveId();
        test_NotFirstEdgePull_given_InputActiveId_then_OutputActiveId();
        test_Push_given_IterationEnd_while_BufferEmpty_then_OutputIterationEnd();
        repeat (10) @(posedge ap_clk);
        $finish;
    end

endmodule