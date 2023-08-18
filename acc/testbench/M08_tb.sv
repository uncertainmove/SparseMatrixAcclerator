`timescale 1ns / 1ps

`include "accelerator.vh"

module M08_tb();
    
    parameter V_ID_WIDTH = `V_ID_WIDTH;
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
    reg [CORE_NUM * V_ID_WIDTH - 1 : 0]       front_active_v_id;
    reg [CORE_NUM - 1 : 0]                    front_active_v_updated;
    reg [CORE_NUM - 1 : 0]                    front_active_v_pull_first_flag;
    reg [CORE_NUM - 1 : 0]                    front_active_v_valid;
    reg [CORE_NUM - 1 : 0]                    front_iteration_end;
    reg [CORE_NUM - 1 : 0]                    front_iteration_end_valid;

    // module output
    wire [CORE_NUM * V_ID_WIDTH - 1 : 0]      active_v_id;
    wire [CORE_NUM - 1 : 0]                   active_v_updated;
    wire [CORE_NUM - 1 : 0]                   active_v_pull_first_flag;
    wire [CORE_NUM - 1 : 0]                   active_v_valid;
    wire [CORE_NUM - 1 : 0]                   iteration_end;
    wire [CORE_NUM - 1 : 0]                   iteration_end_valid;

    task automatic init_input_flag();
        front_active_v_id = 'h0;
        front_active_v_updated = 'h0;
        front_active_v_pull_first_flag = 'h0;
        front_active_v_valid = 'h0;
        front_iteration_end = 'h0;
        front_iteration_end_valid = 'h0;
    endtask

    task automatic reset_module();
        ap_rst = {512{1'b1}};
        init_input_flag();
        repeat (10) @(posedge ap_clk);
        ap_rst = 'h0;
    endtask

    integer i,j;
    logic test_pass;

    task automatic test_given_Rst_then_LocalBufferInit();
        reset_module();
        init_input_flag();
        test_pass = 1;
        repeat (1) @(posedge ap_clk);
        // CHECK
        for (i = 0; i < CORE_NUM; i = i + 1) begin
                assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == 'h0)
                else begin
                    $error("active_v_id init error!");
                    test_pass = 0;
                end
                assert(active_v_updated[i] == 1'b0)
                else begin
                    $error("active_v_updated init error");
                    test_pass = 0;
                end
                assert(active_v_pull_first_flag[i] == 1'b0)
                else begin
                    $error("active_v_pull_first_flag init error");
                    test_pass = 0;
                end
                assert(active_v_valid[i] == 1'b0)
                else begin
                    $error("active_v_valid init error");
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
            
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_given_Rst_then_LocalBufferInit");
    endtask
    
    task automatic test_given_updateData_then_outputData();
        reset_module();
        init_input_flag();
        test_pass = 1;
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            front_active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] = i;
            front_active_v_updated[i] = 1;
            front_active_v_pull_first_flag[i] = 1;
            front_active_v_valid[i] = 1;
        end
        repeat (1) @(posedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
                assert(active_v_id[i * V_ID_WIDTH +: V_ID_WIDTH] == i)
                else begin
                    $error("active_v_id init error!");
                    test_pass = 0;
                end
                assert(active_v_updated[i] == 1'b1)
                else begin
                    $error("active_v_updated init error");
                    test_pass = 0;
                end
                assert(active_v_pull_first_flag[i] == 1'b1)
                else begin
                    $error("active_v_pull_first_flag init error");
                    test_pass = 0;
                end
                assert(active_v_valid[i] == 1'b1)
                else begin
                    $error("active_v_valid init error");
                    test_pass = 0;
                end
        end
        
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_given_updateData_then_outputData");
    endtask
    
    task automatic test_given_OnlyallEnd_then_End();
        reset_module();
        init_input_flag();
        test_pass = 1;
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            if(i%2==0)begin
                front_iteration_end[i] = 1;
            end
            front_iteration_end_valid[i] = 1;
        end
        repeat (1) @(posedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
                assert(iteration_end[i] == 1'b0)
                else begin
                    $error("iteration_end init error");
                    test_pass = 0;
                end
                assert(iteration_end_valid[i] == 1'b1)
                else begin
                    $error("iteration_end_valid init error");
                    test_pass = 0;
                end
        end
        init_input_flag();
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            if(i!=0)begin
                front_iteration_end[i] = 1;
            end
            front_iteration_end_valid[i] = 1;
        end
        repeat (1) @(posedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
                assert(iteration_end[i] == 1'b0)
                else begin
                    $error("iteration_end init error");
                    test_pass = 0;
                end
                assert(iteration_end_valid[i] == 1'b1)
                else begin
                    $error("iteration_end_valid init error");
                    test_pass = 0;
                end
        end
        init_input_flag();
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            front_iteration_end[i] = 1;
            front_iteration_end_valid[i] = 1;
        end
        repeat (1) @(posedge ap_clk);
        @(negedge ap_clk);
        for (i = 0; i < CORE_NUM; i = i + 1) begin
                assert(iteration_end[i] == 1'b1)
                else begin
                    $error("iteration_end init error");
                    test_pass = 0;
                end
                assert(iteration_end_valid[i] == 1'b1)
                else begin
                    $error("iteration_end_valid init error");
                    test_pass = 0;
                end
        end
        
        if (test_pass)  $display("\033[32m[PASSED]\033[m test_given_OnlyallEnd_then_End");
    endtask
    
    apply_iteration_end inst_apply_iteration_end_dut (
        .clk                                    (ap_clk),
        .rst                                    (ap_rst),
        .front_active_v_id                      (front_active_v_id),
        .front_active_v_updated                 (front_active_v_updated),
        .front_active_v_pull_first_flag         (front_active_v_pull_first_flag),
        .front_active_v_valid                   (front_active_v_valid),
        .front_iteration_end                    (front_iteration_end),
        .front_iteration_end_valid              (front_iteration_end_valid),

        .active_v_id                            (active_v_id),
        .active_v_updated                       (active_v_updated),
        .active_v_pull_first_flag               (active_v_pull_first_flag),
        .active_v_valid                         (active_v_valid),
        .iteration_end                          (iteration_end),
        .iteration_end_valid                    (iteration_end_valid)
    );
    
   
    initial begin : apply_iteration_end_Test
        test_given_Rst_then_LocalBufferInit();
        test_given_updateData_then_outputData();
        test_given_OnlyallEnd_then_End();
        repeat (10) @(posedge ap_clk);
        $finish;
    end

        
endmodule
