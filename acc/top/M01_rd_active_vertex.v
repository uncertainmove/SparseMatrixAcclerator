`timescale 1ns / 1ps

`include "accelerator.vh"

module rd_active_vertex #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    CORE_NUM = `CORE_NUM,
    ITERATION_WIDTH = `ITERATION_WIDTH
) (
    input                                   clk,
    input                                   rst,
    input [31 : 0]                          vertex_num,
    input [31 : 0]                          iteration_num,
    input [CORE_NUM * V_ID_WIDTH - 1 : 0]   backend_active_v_id,
    input [CORE_NUM - 1 : 0]                backend_active_v_updated,
    input [CORE_NUM - 1 : 0]                backend_active_v_id_valid,
    input [CORE_NUM - 1 : 0]                backend_iteration_end,
    input [CORE_NUM - 1 : 0]                backend_iteration_end_valid,
    input [CORE_NUM * ITERATION_WIDTH - 1 : 0]  backend_iteration_id,
    input [CORE_NUM - 1 : 0]                next_stage_full,

    output reg [CORE_NUM - 1 : 0]           next_rst,
    output [CORE_NUM - 1 : 0]               iteration_done,
    output [CORE_NUM * V_ID_WIDTH - 1 : 0]  active_v_id,
    output [CORE_NUM - 1 : 0]               active_v_id_valid,
    output [CORE_NUM - 1 : 0]               iteration_end,
    output [CORE_NUM - 1 : 0]               iteration_end_valid,
    output [CORE_NUM * ITERATION_WIDTH - 1 : 0] iteration_id
);
    generate
        genvar i;
        for (i = 0; i < CORE_NUM; i = i + 1) begin
            always @ (posedge clk) begin
                next_rst[i] <= rst;
            end
            rd_active_vertex_single #(.CORE_ID(i)) RD_ACTIVE_VERTEX_SINGLE (
                .clk                    (clk),
                .rst                    (rst),
                .vertex_num             (vertex_num),
                .iteration_num          (iteration_num),
                .backend_active_v_id    (backend_active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH]),
                .backend_active_v_updated   (backend_active_v_updated[i]),
                .backend_active_v_id_valid  (backend_active_v_id_valid[i]),
                .backend_iteration_end      (backend_iteration_end[i]),
                .backend_iteration_end_valid    (backend_iteration_end_valid[i]),
                .backend_iteration_id       (backend_iteration_id[(i + 1) * ITERATION_WIDTH - 1 : i * ITERATION_WIDTH]),
                .next_stage_full            (next_stage_full[i]),

                .iteration_done             (iteration_done[i]),
                .active_v_id                (active_v_id[(i + 1) * V_ID_WIDTH - 1 : i * V_ID_WIDTH]),
                .active_v_id_valid          (active_v_id_valid[i]),
                .iteration_end              (iteration_end[i]),
                .iteration_end_valid        (iteration_end_valid[i]),
                .iteration_id               (iteration_id[(i + 1) * ITERATION_WIDTH - 1 : i * ITERATION_WIDTH])
            );
        end
    endgenerate

endmodule

module rd_active_vertex_single #(parameter
    V_ID_WIDTH = `V_ID_WIDTH,
    VTX_NUM = `VTX_NUM,
    CORE_ID = 0,
    CORE_NUM = `CORE_NUM,
    CORE_NUM_WIDTH = `CORE_NUM_WIDTH,
    BITMAP_COMPRESSED_LENGTH = `BITMAP_COMPRESSED_LENGTH,
    BITMAP_COMPRESSED_LENGTH_WIDTH = `BITMAP_COMPRESSED_LENGTH_WIDTH,
    BITMAP_COMPRESSED_NUM_WIDTH = `BITMAP_COMPRESSED_NUM_WIDTH,
    ITERATION_WIDTH = `ITERATION_WIDTH,
    MAX_ITERATION_NUM = `MAX_ITERATION_NUM
) (
    input                           clk,
    input                           rst,
    input [31 : 0]                  vertex_num,
    input [31 : 0]                  iteration_num,
    input [V_ID_WIDTH - 1 : 0]      backend_active_v_id,
    input                           backend_active_v_updated,
    input                           backend_active_v_id_valid,
    input                           backend_iteration_end,
    input                           backend_iteration_end_valid,
    input [ITERATION_WIDTH - 1 : 0] backend_iteration_id,
    input                           next_stage_full,

    output                          iteration_done,
    output reg [V_ID_WIDTH - 1 : 0] active_v_id,
    output reg                      active_v_id_valid,
    output reg                      iteration_end,
    output reg                      iteration_end_valid,
    output [ITERATION_WIDTH - 1 : 0]   iteration_id
);
    reg [ITERATION_WIDTH - 1 : 0]          local_iteration_id;
    reg [BITMAP_COMPRESSED_NUM_WIDTH : 0]   local_init_bitmap_id;
    wire [BITMAP_COMPRESSED_LENGTH - 1 : 0] local_vis_bitmap [0 : 1];
    reg [BITMAP_COMPRESSED_NUM_WIDTH : 0]   local_vis_bitmap_index;
    reg [BITMAP_COMPRESSED_LENGTH - 1 : 0]  local_vis_bitmap_now;
    reg                                     local_vis_bitmap_index_lock;
    reg [BITMAP_COMPRESSED_NUM_WIDTH : 0]   prefetch_addr;
    reg [2 : 0]                             prefetch_addr_valid; // bram delay
    // set to 0 when detect falling-edge
    reg                                     iteration_lock;

    wire [31 : 0] bitmap_compressed_num;
    wire now_bitmap_id, next_bitmap_id;
    wire [BITMAP_COMPRESSED_LENGTH - 1 : 0] prefetch_buffer_top;
    wire prefetch_buffer_empty, prefetch_buffer_almost_full;
    wire [BITMAP_COMPRESSED_NUM_WIDTH - 1 : 0]      backend_v_bitmap_id1;
    wire [BITMAP_COMPRESSED_LENGTH_WIDTH - 1 : 0]   backend_v_bitmap_id2;
    wire [BITMAP_COMPRESSED_LENGTH_WIDTH - 1 : 0]   active_vertex_index;
    wire                                            active_vertex_bitmap;

    assign bitmap_compressed_num = ((vertex_num + `CORE_NUM * `BITMAP_COMPRESSED_LENGTH - 1) >> (`CORE_NUM_WIDTH + `BITMAP_COMPRESSED_LENGTH_WIDTH));
    assign backend_v_bitmap_id1 = backend_active_v_id >> CORE_NUM_WIDTH >> BITMAP_COMPRESSED_LENGTH_WIDTH;
    assign backend_v_bitmap_id2 = backend_active_v_id[CORE_NUM_WIDTH + BITMAP_COMPRESSED_LENGTH_WIDTH - 1 : CORE_NUM_WIDTH];
    assign now_bitmap_id = local_iteration_id[0];
    assign next_bitmap_id = local_iteration_id[0] ^ 1;
    assign iteration_id = local_iteration_id;
    assign iteration_done = (local_iteration_id == iteration_num);

    btree_cal_active_vtx_in_bitmap BTREE (
        .bitmap     (local_vis_bitmap_now),

        .index      (active_vertex_index),
        .all_visited(active_vertex_bitmap)
    );

    // Note: write_width=1, write_depth=2048
    //       read_width=32, read_depth=64
    vis_bitmap  VIS_BITMAP_BRAM_IP_SINGLE_0 (
        .clka       (clk),
        .ena        (!rst),
        .wea        ((backend_active_v_id_valid && !next_bitmap_id) || (local_vis_bitmap_index < bitmap_compressed_num && active_vertex_bitmap && !now_bitmap_id)),
        .addra      (!next_bitmap_id ? (backend_active_v_id >> CORE_NUM_WIDTH) : {{5'b0},{((local_vis_bitmap_index << 5) + active_vertex_index)}}),
        .dina       (!next_bitmap_id ? backend_active_v_updated : 1'b0),
        .clkb       (clk),
        .enb        (!rst && !now_bitmap_id),
        .addrb      (prefetch_addr),
        .doutb      (local_vis_bitmap[0])
    );

    vis_bitmap_1  VIS_BITMAP_BRAM_IP_SINGLE_1 (
        .clka       (clk),
        .ena        (!rst),
        .wea        ((backend_active_v_id_valid && next_bitmap_id) || (local_vis_bitmap_index < bitmap_compressed_num && active_vertex_bitmap && now_bitmap_id)),
        .addra      (next_bitmap_id ? (backend_active_v_id >> CORE_NUM_WIDTH) : {{5'b0},{((local_vis_bitmap_index << 5) + active_vertex_index)}}),
        .dina       (next_bitmap_id ? backend_active_v_updated : 1'b0),
        .clkb       (clk),
        .enb        (!rst && now_bitmap_id),
        .addrb      (prefetch_addr),
        .doutb      (local_vis_bitmap[1])
    );

    // pull mode : store prefetch bitmap
    prefetch_fifo_ft PREFETCH_FIFO_FT (
        .clk        (clk),
        .srst       (rst),
        .din        (local_vis_bitmap[now_bitmap_id]),
        .wr_en      (prefetch_addr_valid[2]),
        .rd_en      (local_vis_bitmap_index < bitmap_compressed_num && local_init_bitmap_id == bitmap_compressed_num && !active_vertex_bitmap && !prefetch_buffer_empty),
        .dout       (prefetch_buffer_top),
        .full       (),
        .prog_full  (prefetch_buffer_almost_full),
        .empty      (prefetch_buffer_empty),
        .valid      ()
    );

    always @ (posedge clk) begin
        if (rst || iteration_done) begin
            iteration_lock <= 1;
        end else begin
            if (backend_iteration_end && backend_iteration_end_valid && backend_iteration_id == local_iteration_id) begin
                iteration_lock <= 0;
            end else begin
                iteration_lock <= 1;
            end
        end
    end

    // output vid
    // Warning: isolate vertex need to be filter in M3
    always @ (posedge clk) begin
        if (rst || local_init_bitmap_id < bitmap_compressed_num || iteration_done) begin
            active_v_id <= 0;
            active_v_id_valid <= 0;
        end else begin
            if (next_stage_full) begin
                active_v_id <= 0;
                active_v_id_valid <= 0;
            end else begin
                if (local_vis_bitmap_index < bitmap_compressed_num && active_vertex_bitmap) begin
                    active_v_id <= (local_vis_bitmap_index << BITMAP_COMPRESSED_LENGTH_WIDTH << CORE_NUM_WIDTH) + (active_vertex_index << CORE_NUM_WIDTH) + CORE_ID;
                    active_v_id_valid <= ((local_vis_bitmap_index << BITMAP_COMPRESSED_LENGTH_WIDTH << CORE_NUM_WIDTH) + (active_vertex_index << CORE_NUM_WIDTH) + CORE_ID) < vertex_num;
                end else begin
                    active_v_id <= 0;
                    active_v_id_valid <= 0;
                end
            end
        end
    end

    wire iteration_change_flag = backend_iteration_end && backend_iteration_end_valid && backend_iteration_id == local_iteration_id && iteration_lock;
    // output iteration end
    always @ (posedge clk) begin
        if (rst) begin
            local_iteration_id <= 0;
            iteration_end <= 0;
            iteration_end_valid <= 0;
        end else begin
            if (iteration_end) begin
                // check if prefetch buffer is empty
                if (iteration_change_flag) begin
                    local_iteration_id <= local_iteration_id + 1;
                    iteration_end <= 0;
                    iteration_end_valid <= 0;
                end
            end else begin
                if (local_vis_bitmap_index >= bitmap_compressed_num - 1 && !active_vertex_bitmap && prefetch_buffer_empty && !local_vis_bitmap_index_lock) begin
                    iteration_end <= 1;
                    iteration_end_valid <= 1;
                end
            end
        end
    end

    // pull mode : bitmap prefetch addr
    always @ (posedge clk) begin
        if (rst || local_init_bitmap_id < bitmap_compressed_num || iteration_done) begin
            prefetch_addr <= {(BITMAP_COMPRESSED_NUM_WIDTH + 1){1'b1}};
            prefetch_addr_valid <= 0;
        end else begin
            // prefetch
            // 按照点编号范围预取数据
            if (prefetch_addr == {(BITMAP_COMPRESSED_NUM_WIDTH + 1){1'b1}} || (prefetch_addr < bitmap_compressed_num - 1)) begin
                if (!prefetch_buffer_almost_full) begin
                    prefetch_addr <= prefetch_addr + 1;
                    prefetch_addr_valid[0] <= 1;
                end else begin
                    prefetch_addr_valid[0] <= 0;
                end
            end else begin
                prefetch_addr_valid[0] <= 0;
                if (iteration_change_flag) begin
                    prefetch_addr <= {(BITMAP_COMPRESSED_NUM_WIDTH + 1){1'b1}};
                end
            end
            prefetch_addr_valid[1] <= prefetch_addr_valid[0];
            prefetch_addr_valid[2] <= prefetch_addr_valid[1];
        end
    end

    // bitmap read in local reg
    always @ (posedge clk) begin
        if (rst || local_init_bitmap_id < bitmap_compressed_num || iteration_change_flag || iteration_done) begin
            local_vis_bitmap_now <= {BITMAP_COMPRESSED_LENGTH{1'b0}};
            local_vis_bitmap_index <= 0;
            local_vis_bitmap_index_lock <= 1;
        end else begin
            if (!active_vertex_bitmap) begin
                if(!prefetch_buffer_empty)begin
                    local_vis_bitmap_now <= prefetch_buffer_top;
                    if((local_vis_bitmap_index == 0 && local_vis_bitmap_index_lock)) begin
                        local_vis_bitmap_index_lock <= 0;
                    end else begin
                        local_vis_bitmap_index <= local_vis_bitmap_index + 1;
                    end
                end
            end else begin
                if (!next_stage_full) begin
                    local_vis_bitmap_now[active_vertex_index] = 1'b0;
                end
            end
        end
    end


    // bitmap initial & write
    // note: use bram ip initial
    always @ (posedge clk) begin
        if (rst || iteration_done) begin
            local_init_bitmap_id <= 0;
        end else begin
            if (local_init_bitmap_id < bitmap_compressed_num) begin
                local_init_bitmap_id <= local_init_bitmap_id + 1;
            end
        end
    end

endmodule

module btree_cal_active_vtx_in_bitmap #(parameter
    BITMAP_COMPRESSED_LENGTH = `BITMAP_COMPRESSED_LENGTH,
    BITMAP_COMPRESSED_LENGTH_WIDTH = `BITMAP_COMPRESSED_LENGTH_WIDTH
) (
    input [BITMAP_COMPRESSED_LENGTH - 1 : 0] bitmap,

    output [BITMAP_COMPRESSED_LENGTH_WIDTH - 1 : 0] index, // 第一个为1的位置
    output all_visited // 0代表全部visited, 1代表存在未visitied的数据
);

    wire [BITMAP_COMPRESSED_LENGTH_WIDTH - 1 : 0] index_level0 [0 : 31];
    wire [31 : 0] visited_level0;
    wire [BITMAP_COMPRESSED_LENGTH_WIDTH - 1 : 0] index_level1 [0 : 15];
    wire [15 : 0] visited_level1;
    wire [BITMAP_COMPRESSED_LENGTH_WIDTH - 1 : 0] index_level2 [0 : 7];
    wire [7 : 0] visited_level2;
    wire [BITMAP_COMPRESSED_LENGTH_WIDTH - 1 : 0] index_level3 [0 : 3];
    wire [3 : 0] visited_level3;
    wire [BITMAP_COMPRESSED_LENGTH_WIDTH - 1 : 0] index_level4 [0 : 1];
    wire [1 : 0] visited_level4;

    assign index = visited_level4[0] ? index_level4[0] : index_level4[1];
    assign all_visited = visited_level4[0] || visited_level4[1];
    generate
        genvar i;
        for (i = 0; i < 32; i = i + 1) begin
            assign index_level0[i] = i;
            assign visited_level0[i] = bitmap[i];
        end
        for (i = 0; i < 16; i = i + 1) begin
            assign index_level1[i] = visited_level0[2 * i] ? index_level0[2 * i] : index_level0[2 * i + 1];
            assign visited_level1[i] = visited_level0[2 * i] || visited_level0[2 * i + 1];
        end
        for (i = 0; i < 8; i = i + 1) begin
            assign index_level2[i] = visited_level1[2 * i] ? index_level1[2 * i] : index_level1[2 * i + 1];
            assign visited_level2[i] = visited_level1[2 * i] || visited_level1[2 * i + 1];
        end
        for (i = 0; i < 4; i = i + 1) begin
            assign index_level3[i] = visited_level2[2 * i] ? index_level2[2 * i] : index_level2[2 * i + 1];
            assign visited_level3[i] = visited_level2[2 * i] || visited_level2[2 * i + 1];
        end
        for (i = 0; i < 2; i = i + 1) begin
            assign index_level4[i] = visited_level3[2 * i] ? index_level3[2 * i] : index_level3[2 * i + 1];
            assign visited_level4[i] = visited_level3[2 * i] || visited_level3[2 * i + 1];
        end
    endgenerate

endmodule


