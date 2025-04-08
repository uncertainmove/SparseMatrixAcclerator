`timescale 1ns / 1ps

`include "accelerator.vh"

module M05_tb ();

    parameter V_ID_WIDTH = `V_ID_WIDTH;
    parameter V_OFF_AWIDTH = `V_OFF_AWIDTH;
    parameter V_OFF_DWIDTH = `V_OFF_DWIDTH;
    parameter V_VALUE_WIDTH = `V_VALUE_WIDTH;
    parameter CORE_NUM = `CORE_NUM;
    parameter HBM_AWIDTH = `HBM_AWIDTH;
    parameter HBM_EDGE_MASK = `HBM_EDGE_MASK;
    parameter PSEUDO_CHANNEL_NUM = `PSEUDO_CHANNEL_NUM;
    parameter GROUP_CORE_NUM = `GROUP_CORE_NUM;
    parameter CACHELINE_LEN_WIDTH = `CACHELINE_LEN_WIDTH;
    
    parameter integer LP_CLK_PERIOD_PS = 5000; // 200 MHz

    logic ap_clk = 0;

    initial begin: AP_CLK
        forever begin
            ap_clk = #(LP_CLK_PERIOD_PS / 2) ~ap_clk;
        end
    end

    logic [CORE_NUM-1:0] ap_rst = 'h0;

    // module input
    reg [CORE_NUM * HBM_AWIDTH - 1 : 0]       front_rd_hbm_edge_addr;
    reg [CORE_NUM * HBM_EDGE_MASK - 1 : 0]    front_rd_hbm_edge_mask;
    reg [CORE_NUM - 1 : 0]                    front_push_flag;
    reg [CORE_NUM * V_ID_WIDTH - 1 : 0]       front_active_v_id;
    reg [CORE_NUM * V_VALUE_WIDTH - 1 : 0]    front_active_v_value;
    reg [CORE_NUM - 1 : 0]                    front_rd_hbm_edge_valid;
    reg [CORE_NUM - 1 : 0]                    front_iteration_end;
    reg [CORE_NUM - 1 : 0]                    front_iteration_end_valid;
    reg [PSEUDO_CHANNEL_NUM - 1 : 0]          hbm_full;
    reg [PSEUDO_CHANNEL_NUM - 1 : 0]          next_stage_full;
    
    // module output
    wire [CORE_NUM - 1 : 0]                             next_rst;
    wire [CORE_NUM - 1 : 0]                             stage_full;
    wire [PSEUDO_CHANNEL_NUM * HBM_AWIDTH - 1 : 0]      rd_hbm_edge_addr;
    wire [PSEUDO_CHANNEL_NUM - 1 : 0]                   rd_hbm_edge_valid;
    wire [CORE_NUM - 1 : 0]                             push_flag;
    wire [CORE_NUM * V_ID_WIDTH - 1 : 0]                active_v_id;
    wire [CORE_NUM * V_VALUE_WIDTH - 1 : 0]             active_v_value;
    wire [CORE_NUM - 1 : 0]                             active_v_id_valid;
    wire [CORE_NUM - 1 : 0]                             iteration_end;
    wire [CORE_NUM - 1 : 0]                             iteration_end_valid;

    task automatic init_input_flag();
        front_rd_hbm_edge_addr = 'h0;
        front_rd_hbm_edge_mask = 'h0;
        front_push_flag = 'h0;
        front_active_v_id = 'h0;
        front_active_v_value = 'h0;
        front_rd_hbm_edge_valid = 'h0;
        front_iteration_end = 'h0;
        front_iteration_end_valid = 'h0;
        hbm_full = 'h0;
        next_stage_full = 'h0;
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
            for (j = 0; j < GROUP_CORE_NUM; j = j + 1) begin
                assert(stage_full[i] == 1'b0)
                else begin
                    $error("stage_full init error!");
                    test_pass = 0;
                end
                assert(push_flag[i] == 1'b0)
                else begin
                    $error("push_flag init error!");
                    test_pass = 0;
                end
                assert(active_v_id[(i * GROUP_CORE_NUM + j) * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("active_v_id init error!");
                    test_pass = 0;
                end
                assert(active_v_value[(i * GROUP_CORE_NUM + j) * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("active_v_value init error");
                    test_pass = 0;
                end
                assert(active_v_id_valid[i] == 1'b0)
                else begin
                    $error("active_v_id_valid init error");
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
            assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
            else begin
                $error("rd_hbm_edge_addr init error!");
                test_pass = 0;
            end
            assert(rd_hbm_edge_valid[i] == 1'b0)
            else begin
                $error("rd_hbm_edge_valid init error!");
                test_pass = 0;
            end
        end
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Push_given_Rst_then_LocalBufferInit");
    endtask
    
    task automatic test_Push_given_SameAddr_then_UseSingleData();
        reset_module();
        init_input_flag();
        test_pass = 1;
        
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            if (i % 2 == 1)begin
                front_rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] = 1;
            end
            front_rd_hbm_edge_mask[i * HBM_EDGE_MASK + (i % 16)] = 1;
            front_rd_hbm_edge_valid[i] = 1;
            front_push_flag[i] = 1;
            front_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i;
            front_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] = 1;
        end
        
        repeat (1) @(posedge ap_clk);
        // CHECK necessary to check the FIFO status?
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
            for (j = 0; j < GROUP_CORE_NUM; j = j + 1) begin
                assert(stage_full[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("stage_full init write error!");
                    test_pass = 0;
                end
                assert(push_flag[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("push_flag init write error!");
                    test_pass = 0;
                end
                assert(active_v_id[(i * GROUP_CORE_NUM + j) * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("active_v_id init write error!");
                    test_pass = 0;
                end
                assert(active_v_value[(i * GROUP_CORE_NUM + j) * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("active_v_value init write error");
                    test_pass = 0;
                end
                assert(active_v_id_valid[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("active_v_id_valid init write error");
                    test_pass = 0;
                end
                assert(iteration_end[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), iteration_end[(i * GROUP_CORE_NUM + j)], 1'b0);
                end
                assert(iteration_end_valid[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), iteration_end_valid[(i * GROUP_CORE_NUM + j)], 1'b0);
                    test_pass = 0;
                end
            end
            assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
            else begin
                $error("rd_hbm_edge_addr init write error!");
                test_pass = 0;
            end
            assert(rd_hbm_edge_valid[i] == 1'b0)
            else begin
                $error("rd_hbm_edge_valid init write error!");
                test_pass = 0;
            end
        end
        
        //run twice
        init_input_flag();
        repeat (3) @(posedge ap_clk);
        @(negedge ap_clk);
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
            for (j = 0; j < GROUP_CORE_NUM; j = j + 1) begin
                if(j % 2 == 1)begin
                    assert(stage_full[(i * GROUP_CORE_NUM + j)] == 1'b0)
                    else begin
                        $error("stage_full output error!");
                        test_pass = 0;
                    end
                    assert(push_flag[(i * GROUP_CORE_NUM + j)] == 1'b1)
                    else begin
                        $error("push_flag[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), push_flag[(i * GROUP_CORE_NUM + j)], 1'b1);
                        test_pass = 0;
                    end
                    assert(active_v_id[(i * GROUP_CORE_NUM + j) * V_ID_WIDTH +: V_ID_WIDTH] == {V_ID_WIDTH{1'b1}})
                    else begin
                        $error("active_v_id[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), active_v_id[(i * GROUP_CORE_NUM + j) * V_ID_WIDTH +: V_ID_WIDTH], {V_ID_WIDTH{1'b1}});
                        test_pass = 0;
                    end
                    assert(active_v_value[(i * GROUP_CORE_NUM + j) * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 1)
                    else begin
                        $error("active_v_value[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), active_v_value[(i * GROUP_CORE_NUM + j) * V_VALUE_WIDTH +: V_VALUE_WIDTH], 1'b1);
                        test_pass = 0;
                    end
                    assert(active_v_id_valid[(i * GROUP_CORE_NUM + j)] == 1'b1)
                    else begin
                        $error("active_v_id_valid output error");
                        test_pass = 0;
                    end
                    assert(iteration_end[(i * GROUP_CORE_NUM + j)] == 1'b0)
                    else begin
                        $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), iteration_end[(i * GROUP_CORE_NUM + j)], 1'b0);
                    end
                    assert(iteration_end_valid[(i * GROUP_CORE_NUM + j)] == 1'b0)
                    else begin
                        $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), iteration_end_valid[(i * GROUP_CORE_NUM + j)], 1'b0);
                        test_pass = 0;
                    end
                end
                else begin
                    assert(stage_full[(i * GROUP_CORE_NUM + j)] == 1'b0)
                    else begin
                        $error("stage_full output error!");
                        test_pass = 0;
                    end
                    assert(push_flag[(i * GROUP_CORE_NUM + j)] == 1'b1)
                    else begin
                        $error("push_flag[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), push_flag[(i * GROUP_CORE_NUM + j)], 1'b1);
                        test_pass = 0;
                    end
                    assert(active_v_id[(i * GROUP_CORE_NUM + j) * V_ID_WIDTH +: V_ID_WIDTH] == i * GROUP_CORE_NUM + j)
                    else begin
                        $error("active_v_id[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j) , active_v_id[(i * GROUP_CORE_NUM + j) * V_ID_WIDTH +: V_ID_WIDTH], i * GROUP_CORE_NUM + j);
                        test_pass = 0;
                    end
                    assert(active_v_value[(i * GROUP_CORE_NUM + j) * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 1)
                    else begin
                        $error("active_v_value[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), active_v_value[(i * GROUP_CORE_NUM + j) * V_VALUE_WIDTH +: V_VALUE_WIDTH], 1'b1);
                        test_pass = 0;
                    end
                    assert(active_v_id_valid[(i * GROUP_CORE_NUM + j)] == 1'b1)
                    else begin
                        $error("active_v_id_valid output error");
                        test_pass = 0;
                    end
                    assert(iteration_end[(i * GROUP_CORE_NUM + j)] == 1'b0)
                    else begin
                        $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), iteration_end[(i * GROUP_CORE_NUM + j)], 1'b0);
                    end
                    assert(iteration_end_valid[(i * GROUP_CORE_NUM + j)] == 1'b0)
                    else begin
                        $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), iteration_end_valid[(i * GROUP_CORE_NUM + j)], 1'b0);
                        test_pass = 0;
                    end
                end
            end
            assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
            else begin
                $error("rd_hbm_edge_addr output error!");
                test_pass = 0;
            end
            assert(rd_hbm_edge_valid[i] == 1'b1)
            else begin
                $error("rd_hbm_edge_valid output error!");
                test_pass = 0;
            end
        end
        
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Push_given_SameAddr_then_UseSingleData");
    endtask
    
    task automatic test_Push_given_AddrValid_then_OutputAddr();
        reset_module();
        init_input_flag();
        test_pass = 1;
        
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            front_rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] = i;
            for(j = 0; j <= i % 16; j = j + 1) begin
                front_rd_hbm_edge_mask[i * HBM_EDGE_MASK + j] = 1;
            end
            front_rd_hbm_edge_valid[i] = 1;
            front_push_flag[i] = 1;
            front_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i;
            front_active_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] = 1;
        end
        
        repeat (1) @(posedge ap_clk);
        // CHECK necessary to check the FIFO status?
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
            for (j = 0; j < GROUP_CORE_NUM; j = j + 1) begin
                assert(stage_full[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("stage_full init write error!");
                    test_pass = 0;
                end
                assert(push_flag[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("push_flag init write error!");
                    test_pass = 0;
                end
                assert(active_v_id[(i * GROUP_CORE_NUM + j) * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("active_v_id init write error!");
                    test_pass = 0;
                end
                assert(active_v_value[(i * GROUP_CORE_NUM + j) * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
                else begin
                    $error("active_v_value init write error");
                    test_pass = 0;
                end
                assert(active_v_id_valid[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("active_v_id_valid init write error");
                    test_pass = 0;
                end
                assert(iteration_end[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), iteration_end[(i * GROUP_CORE_NUM + j)], 1'b0);
                end
                assert(iteration_end_valid[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), iteration_end_valid[(i * GROUP_CORE_NUM + j)], 1'b0);
                    test_pass = 0;
                end
            end
            assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == 'h0)
            else begin
                $error("rd_hbm_edge_addr init write error!");
                test_pass = 0;
            end
            assert(rd_hbm_edge_valid[i] == 1'b0)
            else begin
                $error("rd_hbm_edge_valid init write error!");
                test_pass = 0;
            end
        end
        
        //run twice
        init_input_flag();
        repeat (3) @(posedge ap_clk);
        @(negedge ap_clk);
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
            for (j = 0; j < GROUP_CORE_NUM; j = j + 1) begin
                assert(stage_full[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("stage_full init write error!");
                    test_pass = 0;
                end
                assert(push_flag[(i * GROUP_CORE_NUM + j)] == 1'b1)
                else begin
                    $error("push_flag init write error!");
                    test_pass = 0;
                end
                if(j == 0)begin
                    assert(active_v_id[(i * GROUP_CORE_NUM + j) * V_ID_WIDTH +: V_ID_WIDTH] == i * GROUP_CORE_NUM)
                    else begin
                        $error("active_v_id init write error!");
                        test_pass = 0;
                    end
                end
                else begin
                    assert(active_v_id[(i * GROUP_CORE_NUM + j) * V_ID_WIDTH +: V_ID_WIDTH] == {V_ID_WIDTH{1'b1}})
                    else begin
                        $error("active_v_id init write error!");
                        test_pass = 0;
                    end
                end
                
                assert(active_v_value[(i * GROUP_CORE_NUM + j) * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 1)
                else begin
                    $error("active_v_value init write error");
                    test_pass = 0;
                end
                assert(active_v_id_valid[(i * GROUP_CORE_NUM + j)] == 1'b1)
                else begin
                    $error("active_v_id_valid init write error");
                    test_pass = 0;
                end
                assert(iteration_end[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), iteration_end[(i * GROUP_CORE_NUM + j)], 1'b0);
                end
                assert(iteration_end_valid[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), iteration_end_valid[(i * GROUP_CORE_NUM + j)], 1'b0);
                    test_pass = 0;
                end
            end
            assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == i * GROUP_CORE_NUM)
            else begin
                $error("rd_hbm_edge_addr output error!");
                test_pass = 0;
            end
            assert(rd_hbm_edge_valid[i] == 1'b1)
            else begin
                $error("rd_hbm_edge_valid output error!");
                test_pass = 0;
            end
        end
        
        //run three times
        init_input_flag();
        repeat (1) @(posedge ap_clk);
        @(negedge ap_clk);
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
            for (j = 0; j < GROUP_CORE_NUM; j = j + 1) begin
                assert(stage_full[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("stage_full init write error!");
                    test_pass = 0;
                end
                assert(push_flag[(i * GROUP_CORE_NUM + j)] == 1'b1)
                else begin
                    $error("push_flag init write error!");
                    test_pass = 0;
                end
                if(j == 0 || j == 1)begin
                    assert(active_v_id[(i * GROUP_CORE_NUM + j) * V_ID_WIDTH +: V_ID_WIDTH] == i * GROUP_CORE_NUM + 1)
                    else begin
                        $error("active_v_id init write error!");
                        test_pass = 0;
                    end
                end
                else begin
                    assert(active_v_id[(i * GROUP_CORE_NUM + j) * V_ID_WIDTH +: V_ID_WIDTH] == {V_ID_WIDTH{1'b1}})
                    else begin
                        $error("active_v_id init write error!");
                        test_pass = 0;
                    end
                end
                assert(active_v_value[(i * GROUP_CORE_NUM + j) * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 1)
                else begin
                    $error("active_v_value init write error");
                    test_pass = 0;
                end
                assert(active_v_id_valid[(i * GROUP_CORE_NUM + j)] == 1'b1)
                else begin
                    $error("active_v_id_valid init write error");
                    test_pass = 0;
                end
                assert(iteration_end[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), iteration_end[(i * GROUP_CORE_NUM + j)], 1'b0);
                end
                assert(iteration_end_valid[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), iteration_end_valid[(i * GROUP_CORE_NUM + j)], 1'b0);
                    test_pass = 0;
                end
            end
            assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == i * GROUP_CORE_NUM + 1)
            else begin
                $error("rd_hbm_edge_addr output error!");
                test_pass = 0;
            end
            assert(rd_hbm_edge_valid[i] == 1'b1)
            else begin
                $error("rd_hbm_edge_valid output error!");
                test_pass = 0;
            end
        end
        
        //run four times
        init_input_flag();
        repeat (1) @(posedge ap_clk);
        @(negedge ap_clk);
        for (i = 0; i < PSEUDO_CHANNEL_NUM; i = i + 1) begin
            for (j = 0; j < GROUP_CORE_NUM; j = j + 1) begin
                assert(stage_full[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("stage_full init write error!");
                    test_pass = 0;
                end
                assert(push_flag[(i * GROUP_CORE_NUM + j)] == 1'b1)
                else begin
                    $error("push_flag init write error!");
                    test_pass = 0;
                end
                if(j == 0 || j == 1 || j == 2)begin
                    assert(active_v_id[(i * GROUP_CORE_NUM + j) * V_ID_WIDTH +: V_ID_WIDTH] == i * GROUP_CORE_NUM + 2)
                    else begin
                        $error("active_v_id init write error!");
                        test_pass = 0;
                    end
                end
                else begin
                    assert(active_v_id[(i * GROUP_CORE_NUM + j) * V_ID_WIDTH +: V_ID_WIDTH] == {V_ID_WIDTH{1'b1}})
                    else begin
                        $error("active_v_id init write error!");
                        test_pass = 0;
                    end
                end
                assert(active_v_value[(i * GROUP_CORE_NUM + j) * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 1)
                else begin
                    $error("active_v_value init write error");
                    test_pass = 0;
                end
                assert(active_v_id_valid[(i * GROUP_CORE_NUM + j)] == 1'b1)
                else begin
                    $error("active_v_id_valid init write error");
                    test_pass = 0;
                end
                assert(iteration_end[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("iteration_end[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), iteration_end[(i * GROUP_CORE_NUM + j)], 1'b0);
                end
                assert(iteration_end_valid[(i * GROUP_CORE_NUM + j)] == 1'b0)
                else begin
                    $error("iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", (i * GROUP_CORE_NUM + j), iteration_end_valid[(i * GROUP_CORE_NUM + j)], 1'b0);
                    test_pass = 0;
                end
            end
            assert(rd_hbm_edge_addr[i * HBM_AWIDTH +: HBM_AWIDTH] == i * GROUP_CORE_NUM + 2)
            else begin
                $error("rd_hbm_edge_addr output error!");
                test_pass = 0;
            end
            assert(rd_hbm_edge_valid[i] == 1'b1)
            else begin
                $error("rd_hbm_edge_valid output error!");
                test_pass = 0;
            end
        end
        
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Push_given_AddrValid_then_OutputAddr");
    endtask
    
    
    generate_hbm_edge_rqst inst_M05_dut (
        .clk                            (ap_clk),
        .rst                            (ap_rst),
        .front_rd_hbm_edge_addr         (front_rd_hbm_edge_addr),
        .front_rd_hbm_edge_mask         (front_rd_hbm_edge_mask),
        .front_push_flag                (front_push_flag),
        .front_active_v_id              (front_active_v_id),
        .front_active_v_value           (front_active_v_value),
        .front_rd_hbm_edge_valid        (front_rd_hbm_edge_valid),
        .front_iteration_end            (front_iteration_end),
        .front_iteration_end_valid      (front_iteration_end_valid),
        .hbm_full                       (hbm_full),
        .next_stage_full                (next_stage_full),

        .next_rst                       (next_rst),
        .stage_full                     (stage_full),
        .rd_hbm_edge_addr               (rd_hbm_edge_addr),
        .rd_hbm_edge_valid              (rd_hbm_edge_valid),
        .push_flag                      (push_flag),
        .active_v_id                    (active_v_id),
        .active_v_value                 (active_v_value),
        .active_v_id_valid              (active_v_id_valid),
        .iteration_end                  (iteration_end),
        .iteration_end_valid            (iteration_end_valid)
    );
    
   
    initial begin : Generate_Hbm_Edge_Rqst_Test
        test_Push_given_Rst_then_LocalBufferInit();
        test_Push_given_SameAddr_then_UseSingleData();
        test_Push_given_AddrValid_then_OutputAddr();
        repeat (10) @(posedge ap_clk);
        $finish;
    end

endmodule
