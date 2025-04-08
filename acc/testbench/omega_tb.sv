`timescale 1ns / 1ps

`include "accelerator.vh"

module omega_tb();

    parameter V_ID_WIDTH = `V_ID_WIDTH;
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
    reg [CORE_NUM - 1 : 0]                    om1_front_push_flag;
    reg [CORE_NUM * V_ID_WIDTH - 1 : 0]       om1_front_update_v_id;
    reg [CORE_NUM * V_VALUE_WIDTH - 1 : 0]    om1_front_update_v_value;
    reg [CORE_NUM - 1 : 0]                    om1_front_pull_first_flag;
    reg [CORE_NUM - 1 : 0]                    om1_front_update_v_valid;
    reg [CORE_NUM - 1 : 0]                    om1_front_iteration_end;
    reg [CORE_NUM - 1 : 0]                    om1_front_iteration_end_valid;
    reg [CORE_NUM - 1 : 0]                    om2_front_push_flag;
    reg [CORE_NUM * V_ID_WIDTH - 1 : 0]       om2_front_update_v_id;
    reg [CORE_NUM * V_VALUE_WIDTH - 1 : 0]    om2_front_update_v_value;
    reg [CORE_NUM - 1 : 0]                    om2_front_pull_first_flag;
    reg [CORE_NUM - 1 : 0]                    om2_front_update_v_valid;
    reg [CORE_NUM - 1 : 0]                    om2_front_iteration_end;
    reg [CORE_NUM - 1 : 0]                    om2_front_iteration_end_valid;
    reg [CORE_NUM - 1 : 0]                    source_core_full;

    // module output
    wire [CORE_NUM - 1 : 0]                   om1_rst;
    wire [CORE_NUM - 1 : 0]                   om2_rst;
    wire [CORE_NUM - 1 : 0]                   om1_stage_full;
    wire [CORE_NUM - 1 : 0]                   om2_stage_full;
    wire [CORE_NUM - 1 : 0]                   om1_push_flag;
    wire [CORE_NUM * V_ID_WIDTH - 1 : 0]      om1_update_v_id;
    wire [CORE_NUM * V_VALUE_WIDTH - 1 : 0]   om1_update_v_value;
    wire [CORE_NUM - 1 : 0]                   om1_pull_first_flag;
    wire [CORE_NUM - 1 : 0]                   om1_update_v_valid;
    wire [CORE_NUM - 1 : 0]                   om1_iteration_end;
    wire [CORE_NUM - 1 : 0]                   om1_iteration_end_valid;
    wire [CORE_NUM - 1 : 0]                   om2_push_flag;
    wire [CORE_NUM * V_ID_WIDTH - 1 : 0]      om2_update_v_id;
    wire [CORE_NUM * V_VALUE_WIDTH - 1 : 0]   om2_update_v_value;
    wire [CORE_NUM - 1 : 0]                   om2_pull_first_flag;
    wire [CORE_NUM - 1 : 0]                   om2_update_v_valid;
    wire [CORE_NUM - 1 : 0]                   om2_iteration_end;
    wire [CORE_NUM - 1 : 0]                   om2_iteration_end_valid;
    
    task automatic init_input_flag();
        om1_front_push_flag = 'h0;
        om1_front_update_v_id = 'h0;
        om1_front_update_v_value = 'h0;
        om1_front_pull_first_flag = 'h0;
        om1_front_update_v_valid = 'h0;
        om1_front_iteration_end = 'h0;
        om1_front_iteration_end_valid = 'h0;
        om2_front_push_flag = 'h0;
        om2_front_update_v_id = 'h0;
        om2_front_update_v_value = 'h0;
        om2_front_pull_first_flag = 'h0;
        om2_front_update_v_valid = 'h0;
        om2_front_iteration_end = 'h0;
        om2_front_iteration_end_valid = 'h0;
        source_core_full = 'h0;
    endtask

    task automatic reset_module();
        ap_rst = {512{1'b1}};
        init_input_flag();
        repeat (10) @(posedge ap_clk);
        ap_rst = 'h0;
    endtask

    integer i,j;
    logic [CORE_NUM -1:0] om1_res_ready,om2_res_ready;
    logic test_pass;
    
    task automatic test_Push_given_Rst_then_LocalBufferInit();
        reset_module();
        init_input_flag();
        test_pass = 1;
        repeat (1) @(posedge ap_clk);
        
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(om1_stage_full[i] == 1'b0)
            else begin
                $error("om1_stage_full init error!");
                test_pass = 0;
            end
            assert(om1_push_flag[i] == 1'b0)
            else begin
                $error("om1_push_flag init error");
                test_pass = 0;
            end
            assert(om1_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("om1_update_v_id init error!");
                test_pass = 0;
            end
            assert(om1_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("om1_update_v_value init error");
                test_pass = 0;
            end
            assert(om1_pull_first_flag[i] == 1'b0)
            else begin
                $error("om1_pull_first_flag init error");
                test_pass = 0;
            end
            assert(om1_update_v_valid[i] == 1'b0)
            else begin
                $error("om1_update_v_valid init error");
                test_pass = 0;
            end
            assert(om1_iteration_end[i] == 1'b0)
            else begin
                $error("om1_iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, om1_iteration_end[i], 1'b0);
            end
            assert(om1_iteration_end_valid[i] == 1'b0)
            else begin
                $error("om1_iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, om1_iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
            assert(om2_stage_full[i] == 1'b0)
            else begin
                $error("om2_stage_full init error!");
                test_pass = 0;
            end
            assert(om2_push_flag[i] == 1'b0)
            else begin
                $error("om2_push_flag init error");
                test_pass = 0;
            end
            assert(om2_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("om2_update_v_id init error!");
                test_pass = 0;
            end
            assert(om2_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("om2_update_v_value init error");
                test_pass = 0;
            end
            assert(om2_pull_first_flag[i] == 1'b0)
            else begin
                $error("om2_pull_first_flag init error");
                test_pass = 0;
            end
            assert(om2_update_v_valid[i] == 1'b0)
            else begin
                $error("om2_update_v_valid init error");
                test_pass = 0;
            end
            assert(om2_iteration_end[i] == 1'b0)
            else begin
                $error("om2_iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, om2_iteration_end[i], 1'b0);
            end
            assert(om2_iteration_end_valid[i] == 1'b0)
            else begin
                $error("om2_iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, om2_iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
        end
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_Pull_given_Rst_then_LocalBufferInit");
    endtask
    
    task automatic test_PushInOut_given_DataValid_then_OutputValid();
        reset_module();
        init_input_flag();
        test_pass = 1;
        om1_res_ready = {512{1'b0}};
        om2_res_ready = {512{1'b0}};
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            om1_front_push_flag[i] = 1;
            om1_front_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i+1;
            om1_front_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] = 1;
            om1_front_pull_first_flag[i] = 0;
            om1_front_update_v_valid[i] = 1;
            om1_front_iteration_end[i] = 0;
            om1_front_iteration_end_valid = 0;
            om2_front_push_flag[i] = 1;
            om2_front_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i+2;
            om2_front_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] = 2;
            om2_front_pull_first_flag[i] = 0;
            om2_front_update_v_valid[i] = 1;
            om2_front_iteration_end[i] = 0;
            om2_front_iteration_end_valid = 0;
        end
        repeat (1) @(posedge ap_clk);
        init_input_flag();
        for (j = 0; j < 100; j = j + 1) begin
            repeat (1) @(posedge ap_clk);
            for (i = 0; i < CORE_NUM; i = i + 1) begin
                if(om1_update_v_valid[i] && om1_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] % CORE_NUM == i && om1_push_flag[i] && om1_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 1)begin
                    om1_res_ready[i] = 1;
                end
                if(om2_update_v_valid[i] && om2_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] % CORE_NUM == i && om2_push_flag[i] && om2_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 2)begin
                    om2_res_ready[i] = 1;
                end
            end
        end
        assert(om1_res_ready == {512{1'b1}})
        else begin
            $error("om1_res_ready init error!");
            test_pass = 0;
        end
        assert(om2_res_ready == {512{1'b1}})
        else begin
            $error("om2_res_ready init error!");
            test_pass = 0;
        end
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(om1_stage_full[i] == 1'b0)
            else begin
                $error("om1_stage_full init error!");
                test_pass = 0;
            end
            assert(om1_push_flag[i] == 1'b0)
            else begin
                $error("om1_push_flag init error");
                test_pass = 0;
            end
            assert(om1_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("om1_update_v_id init error!");
                test_pass = 0;
            end
            assert(om1_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("om1_update_v_value init error");
                test_pass = 0;
            end
            assert(om1_pull_first_flag[i] == 1'b0)
            else begin
                $error("om1_pull_first_flag init error");
                test_pass = 0;
            end
            assert(om1_update_v_valid[i] == 1'b0)
            else begin
                $error("om1_update_v_valid init error");
                test_pass = 0;
            end
            assert(om1_iteration_end[i] == 1'b0)
            else begin
                $error("om1_iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, om1_iteration_end[i], 1'b0);
            end
            assert(om1_iteration_end_valid[i] == 1'b0)
            else begin
                $error("om1_iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, om1_iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
            assert(om2_stage_full[i] == 1'b0)
            else begin
                $error("om2_stage_full init error!");
                test_pass = 0;
            end
            assert(om2_push_flag[i] == 1'b0)
            else begin
                $error("om2_push_flag init error");
                test_pass = 0;
            end
            assert(om2_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("om2_update_v_id init error!");
                test_pass = 0;
            end
            assert(om2_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("om2_update_v_value init error");
                test_pass = 0;
            end
            assert(om2_pull_first_flag[i] == 1'b0)
            else begin
                $error("om2_pull_first_flag init error");
                test_pass = 0;
            end
            assert(om2_update_v_valid[i] == 1'b0)
            else begin
                $error("om2_update_v_valid init error");
                test_pass = 0;
            end
            assert(om2_iteration_end[i] == 1'b0)
            else begin
                $error("om2_iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, om2_iteration_end[i], 1'b0);
            end
            assert(om2_iteration_end_valid[i] == 1'b0)
            else begin
                $error("om2_iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, om2_iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
        end
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_PushInOut_given_DataValid_then_OutputValid");
    endtask
    
    task automatic test_PullInOut_given_DataValid_then_OutputValid();
        reset_module();
        init_input_flag();
        test_pass = 1;
        om1_res_ready = {512{1'b0}};
        om2_res_ready = {512{1'b0}};
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            om1_front_push_flag[i] = 0;
            om1_front_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = 2;
            om1_front_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] = i+2;
            om1_front_pull_first_flag[i] = 0;
            om1_front_update_v_valid[i] = 1;
            om1_front_iteration_end[i] = 0;
            om1_front_iteration_end_valid = 0;
            om2_front_push_flag[i] = 0;
            om2_front_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i+2;
            om2_front_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] = 3;
            om2_front_pull_first_flag[i] = 0;
            om2_front_update_v_valid[i] = 1;
            om2_front_iteration_end[i] = 0;
            om2_front_iteration_end_valid = 0;
        end
        repeat (1) @(posedge ap_clk);
        init_input_flag();
        for (j = 0; j < 100; j = j + 1) begin
            repeat (1) @(posedge ap_clk);
            for (i = 0; i < CORE_NUM; i = i + 1) begin
                if(om1_update_v_valid[i] && om1_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 2 && !om1_push_flag[i] && om1_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] % CORE_NUM == i)begin
                    om1_res_ready[i] = 1;
                end
                if(om2_update_v_valid[i] && om2_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] % CORE_NUM == i && !om2_push_flag[i] && om2_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 3)begin
                    om2_res_ready[i] = 1;
                end
            end
        end
        assert(om1_res_ready == {512{1'b1}})
        else begin
            $error("om1_res_ready init error!");
            test_pass = 0;
        end
        assert(om2_res_ready == {512{1'b1}})
        else begin
            $error("om2_res_ready init error!");
            test_pass = 0;
        end
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            assert(om1_stage_full[i] == 1'b0)
            else begin
                $error("om1_stage_full init error!");
                test_pass = 0;
            end
            assert(om1_push_flag[i] == 1'b0)
            else begin
                $error("om1_push_flag init error");
                test_pass = 0;
            end
            assert(om1_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("om1_update_v_id init error!");
                test_pass = 0;
            end
            assert(om1_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("om1_update_v_value init error");
                test_pass = 0;
            end
            assert(om1_pull_first_flag[i] == 1'b0)
            else begin
                $error("om1_pull_first_flag init error");
                test_pass = 0;
            end
            assert(om1_update_v_valid[i] == 1'b0)
            else begin
                $error("om1_update_v_valid init error");
                test_pass = 0;
            end
            assert(om1_iteration_end[i] == 1'b0)
            else begin
                $error("om1_iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, om1_iteration_end[i], 1'b0);
            end
            assert(om1_iteration_end_valid[i] == 1'b0)
            else begin
                $error("om1_iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, om1_iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
            assert(om2_stage_full[i] == 1'b0)
            else begin
                $error("om2_stage_full init error!");
                test_pass = 0;
            end
            assert(om2_push_flag[i] == 1'b0)
            else begin
                $error("om2_push_flag init error");
                test_pass = 0;
            end
            assert(om2_update_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
            else begin
                $error("om2_update_v_id init error!");
                test_pass = 0;
            end
            assert(om2_update_v_value[i * V_VALUE_WIDTH +: V_VALUE_WIDTH] == 'h0)
            else begin
                $error("om2_update_v_value init error");
                test_pass = 0;
            end
            assert(om2_pull_first_flag[i] == 1'b0)
            else begin
                $error("om2_pull_first_flag init error");
                test_pass = 0;
            end
            assert(om2_update_v_valid[i] == 1'b0)
            else begin
                $error("om2_update_v_valid init error");
                test_pass = 0;
            end
            assert(om2_iteration_end[i] == 1'b0)
            else begin
                $error("om2_iteration_end[%3d] is 0x%0x, expect 0x%0x!", i, om2_iteration_end[i], 1'b0);
            end
            assert(om2_iteration_end_valid[i] == 1'b0)
            else begin
                $error("om2_iteration_end_valid[%3d] is 0x%0x, expect 0x%0x!", i, om2_iteration_end_valid[i], 1'b0);
                test_pass = 0;
            end
        end
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_PullInOut_given_DataValid_then_OutputValid");
    endtask
    
    omega_network inst_omega_dut (
        .clk                                             (ap_clk),
        .om1_front_rst                                   (ap_rst),
        .om1_front_push_flag                             (om1_front_push_flag),
        .om1_front_update_v_id                           (om1_front_update_v_id),
        .om1_front_update_v_value                        (om1_front_update_v_value),
        .om1_front_pull_first_flag                       (om1_front_pull_first_flag),
        .om1_front_update_v_valid                        (om1_front_update_v_valid),
        .om1_front_iteration_end                         (om1_front_iteration_end),
        .om1_front_iteration_end_valid                   (om1_front_iteration_end_valid),
        .om2_front_rst                                   (ap_rst),
        .om2_front_push_flag                             (om2_front_push_flag),
        .om2_front_update_v_id                           (om2_front_update_v_id),
        .om2_front_update_v_value                        (om2_front_update_v_value),
        .om2_front_pull_first_flag                       (om2_front_pull_first_flag),
        .om2_front_update_v_valid                        (om2_front_update_v_valid),
        .om2_front_iteration_end                         (om2_front_iteration_end),
        .om2_front_iteration_end_valid                   (om2_front_iteration_end_valid),
        .source_core_full                                (source_core_full),

        .om1_rst                                   (om1_rst),
        .om2_rst                                   (om2_rst),
        .om1_stage_full                            (om1_stage_full),
        .om2_stage_full                            (om2_stage_full),
        .om1_push_flag                             (om1_push_flag),
        .om1_update_v_id                           (om1_update_v_id),
        .om1_update_v_value                        (om1_update_v_value),
        .om1_pull_first_flag                       (om1_pull_first_flag),
        .om1_update_v_valid                        (om1_update_v_valid),
        .om1_iteration_end                         (om1_iteration_end),
        .om1_iteration_end_valid                   (om1_iteration_end_valid),
        .om2_push_flag                             (om2_push_flag),
        .om2_update_v_id                           (om2_update_v_id),
        .om2_update_v_value                        (om2_update_v_value),
        .om2_pull_first_flag                       (om2_pull_first_flag),
        .om2_update_v_valid                        (om2_update_v_valid),
        .om2_iteration_end                         (om2_iteration_end),
        .om2_iteration_end_valid                   (om2_iteration_end_valid)
    );

    initial begin : omega_network_Test
        test_Push_given_Rst_then_LocalBufferInit();
        test_PushInOut_given_DataValid_then_OutputValid();
        test_PullInOut_given_DataValid_then_OutputValid();
        repeat (10) @(posedge ap_clk);
        $finish;
    end
endmodule
