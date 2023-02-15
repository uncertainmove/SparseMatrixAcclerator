#include "acc.h"

using namespace std;


queue<int> active_v_queue[2][CORE_NUM];
queue<int> active_v_id_buffer[CORE_NUM], active_v_l_offset_buffer[CORE_NUM], active_v_r_offset_buffer[CORE_NUM];
queue<int> active_v_id_buffer_2[CORE_NUM], active_v_l_offset_buffer_2[CORE_NUM], active_v_r_offset_buffer_2[CORE_NUM];
queue<int> schedule_src_id_buffer[CORE_NUM], schedule_dst_id_buffer[CORE_NUM];
queue<int> schedule_src_id_buffer_2[CORE_NUM], schedule_dst_id_buffer_2[CORE_NUM];

bool vis_bitmap[MAX_VERTEX_NUM];
int loffset_bram[MAX_VERTEX_NUM], roffset_bram[MAX_VERTEX_NUM];
pair<int, int> edge_mem[MAX_EDGE_NUM];
int vertex_ram[MAX_VERTEX_NUM];

int active_v_id[CORE_NUM], active_edge[CORE_NUM], edge_valid[CORE_NUM];
int bitmap_line_id[CORE_NUM];

int origin_bfs_ans[MAX_VERTEX_NUM];
queue<int> origin_bfs_active_vertex;
vector<int> ngh[MAX_VERTEX_NUM];


int hyperx_1_src_id[CORE_NUM], hyperx_1_dst_id[CORE_NUM];
int hyperx_2_src_id[CORE_NUM], hyperx_2_dst_id[CORE_NUM];

int hyperx_1_src_id_out[CORE_NUM], hyperx_1_dst_id_out[CORE_NUM];
int hyperx_2_src_id_out[CORE_NUM], hyperx_2_dst_id_out[CORE_NUM];

int hyperx_network_1();
int hyperx_network_2();

int vertex_not_read[32] = {798169, 802022, 859188, 737253, 236717, 800651, 884620, 1038374, 246686, 72581,
                            514920, 939780, 433575, 644849, 887126, 817258, 812863, 802479, 701232, 798386,
                            1030895, 609384, 504066, 426492, 16463, 644226, 506990, 1045487, 628265, 735673,
                            507966, 584249};
/*
int vertex_not_read[32] = {2472, 1355, 9395, 9636, 1431, 12919, 1488, 6682, 9609, 12355, 13358, 317, 12851,
                           4939, 10307, 3782, 14884, 1842, 2086, 9255, 7582, 12890, 13303, 2337, 10501,
                           14495, 5721, 7027, 10490, 2990};
*/
int vertex_not_read_flag[MAX_VERTEX_NUM];

int main()
{
    int ROOT;
    int v_num, e_num;
    int clk, rst, push_flag, iteration_id;
    int visited_edge_num = 0;

    cout << "Input ROOT ID: ";
    cin >> ROOT;

    //initialization
    FILE *p1;
    int src, dst;

    p1 = fopen("../data/test_graph20_degree_reorder.txt","r");
    if (p1 == NULL) {
        cout << "input file error" << endl;
        return -1;
    }

    fscanf(p1, "%d %d", &v_num, &e_num);
    for (int i = 0; i< e_num; ++ i) {
        if (i % 1000000 == 0) cout << "now: " <<  i << endl;
        fscanf(p1, "%d %d", &src, &dst);
        ngh[dst].push_back(src);
        //if (i < 10) cout << src << " " << dst << endl;
    }
    fclose(p1);

    //original bfs
    origin_bfs_active_vertex.push(ROOT);
    for (int i = 0; i < v_num; ++ i) origin_bfs_ans[i] = 1000000000;
    origin_bfs_ans[ROOT] = 0;
    while (origin_bfs_active_vertex.size() != 0) {
        int v = origin_bfs_active_vertex.front();
        origin_bfs_active_vertex.pop();

        vector<int>::iterator it;
        for (it = ngh[v].begin(); it != ngh[v].end(); ++it) {
            if (origin_bfs_ans[v] + 1 < origin_bfs_ans[*it]) {
                origin_bfs_ans[*it] = origin_bfs_ans[v] + 1;
                origin_bfs_active_vertex.push(*it);
            }
        }
    }

    int now_offset = 0;
    for (int i = 0; i < v_num; ++ i) {
        loffset_bram[i] = now_offset;
        roffset_bram[i] = now_offset + ngh[i].size();

        vector<int>::iterator it;
        for (it = ngh[i].begin(); it != ngh[i].end(); ++ it) {
            edge_mem[now_offset] = make_pair(i, *it);
            now_offset ++;
        }
        vertex_ram[i] = 1000000000;
    }
    vertex_ram[ROOT] = 0;
    active_v_queue[0][ROOT % CORE_NUM].push(ROOT);
    memset(vis_bitmap, 0, sizeof(vis_bitmap));
    vis_bitmap[ROOT] = 1;

    memset(vertex_not_read_flag, 0, sizeof(vertex_not_read_flag));
    for (int i = 0; i < v_num; ++ i)
        for (int j = 0; j < 32; ++ j)
            if (i == vertex_not_read[j])
                vertex_not_read_flag[i] = 1;

    p1 = fopen("debug_trace2.txt", "w");

    FILE* channel_trace[PSEUDO_CHANNEL_NUM];
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        string nm = "../data/channel_trace" + to_string(i);
        channel_trace[i] = fopen(nm.c_str(), "w");
    }
    for (clk = 0; clk < MAX_CLK && iteration_id <= 5; clk ++) {
        if (clk % 100000 == 0) cout << clk << endl;
        if (clk < 10) {
            rst = 1;
            push_flag = 1;
            iteration_id = 0;
            for (int i = 0; i < CORE_NUM; ++ i) bitmap_line_id[i] = 0;
        }
        else {
            rst = 0;
        }

        if (!rst) {

            if (iteration_id <= 1 || iteration_id >= 5) {
                push_flag = 1;
            }
            else {
                push_flag = 0;
            }

            if (push_flag) {
                //schedule vertex
                for (int i = 0; i < CORE_NUM; ++ i) {
                    if (active_v_queue[iteration_id % 2][i].size() != 0) {
                        active_v_id_buffer[i].push(active_v_queue[iteration_id % 2][i].front());
                        active_v_l_offset_buffer[i].push(loffset_bram[active_v_queue[iteration_id % 2][i].front()]);
                        active_v_r_offset_buffer[i].push(roffset_bram[active_v_queue[iteration_id % 2][i].front()]);

                        active_v_queue[iteration_id % 2][i].pop();
                    }
                }
            }
            else {
                for (int i = 0; i < CORE_NUM; ++ i) {
                    int now_bitmap_line_id, max_bitmap_line_id = (bitmap_line_id[i] / 32 + 1) * 32;
                    for (now_bitmap_line_id = bitmap_line_id[i]; now_bitmap_line_id < max_bitmap_line_id; ++now_bitmap_line_id){
                        int v_id = now_bitmap_line_id * CORE_NUM + i;
                        if (v_id < v_num) {
                            if (vis_bitmap[v_id] == 0) {
                                active_v_id_buffer_2[i].push(v_id);
                                active_v_l_offset_buffer_2[i].push(loffset_bram[v_id]);
                                active_v_r_offset_buffer_2[i].push(roffset_bram[v_id]);

                                bitmap_line_id[i] = now_bitmap_line_id + 1;
                                break;
                            }
                            bitmap_line_id[i] = now_bitmap_line_id + 1;
                        }
                    }
                }
            }

            //initialization
            int min_edge_addr[PSEUDO_CHANNEL_NUM];
            int active_buffer_empty[CORE_NUM], active_buffer_isolated_v[CORE_NUM];
            for (int i = 0; i < CORE_NUM; ++ i) {
                active_buffer_empty[i] = (active_v_id_buffer[i].size() == 0);
                active_buffer_isolated_v[i] = !active_buffer_empty[i] && active_v_l_offset_buffer[i].front() >= active_v_r_offset_buffer[i].front();
            }

            for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
                min_edge_addr[i] = 1000000000;
                for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
                    int core_id = i * GROUP_CORE_NUM + j;
                    if (!active_buffer_empty[core_id] && !active_buffer_isolated_v[core_id]) {
                        min_edge_addr[i] = min(min_edge_addr[i], active_v_l_offset_buffer[core_id].front());
                    }
                }
            }

            for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
                for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
                    edge_valid[i * GROUP_CORE_NUM + j] = 0;
                }
                for (int j = 0; j < GROUP_CORE_NUM; ++ j){
                    int front_core_id = i * GROUP_CORE_NUM + j;
                    if (!active_buffer_empty[front_core_id] && !active_buffer_isolated_v[front_core_id] && active_v_l_offset_buffer[front_core_id].front() / CACHELINE_LEN == min_edge_addr[i] / CACHELINE_LEN) {
                        int loffset = active_v_l_offset_buffer[front_core_id].front();
                        int roffset = active_v_r_offset_buffer[front_core_id].front();

                        if (loffset / CACHELINE_LEN == roffset / CACHELINE_LEN) {
                            for (int k = loffset % CACHELINE_LEN; k < roffset % CACHELINE_LEN; ++ k) {
                                edge_valid[i * GROUP_CORE_NUM + k] = 1;
                            }
                        }
                        else {
                            for (int k = loffset % CACHELINE_LEN; k < CACHELINE_LEN; ++k) {
                                edge_valid[i * GROUP_CORE_NUM + k] = 1;
                            }
                        }
                    }
                }
            }

            for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
                for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
                    int front_core_id = i * GROUP_CORE_NUM + j;
                    if (!active_buffer_empty[front_core_id]) {
                        if (active_buffer_isolated_v[front_core_id]) {
                            vis_bitmap[active_v_id_buffer[front_core_id].front()] = 1;
                            active_v_id_buffer[front_core_id].pop();
                            active_v_l_offset_buffer[front_core_id].pop();
                            active_v_r_offset_buffer[front_core_id].pop();
                        }
                        else {
                            if (active_v_l_offset_buffer[front_core_id].front() / CACHELINE_LEN == min_edge_addr[i] / CACHELINE_LEN) {
                                active_v_l_offset_buffer[front_core_id].front() = (active_v_l_offset_buffer[front_core_id].front() / CACHELINE_LEN + 1) * CACHELINE_LEN;

                                if (active_v_l_offset_buffer[front_core_id].front() >= active_v_r_offset_buffer[front_core_id].front()) {
                                    active_v_id_buffer[front_core_id].pop();
                                    active_v_l_offset_buffer[front_core_id].pop();
                                    active_v_r_offset_buffer[front_core_id].pop();
                                }
                            }
                        }
                    }
                }
            }

            for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
                if (min_edge_addr[i] != 1000000000) {
                    int rd_edge_addr = (min_edge_addr[i] / CACHELINE_LEN) * CACHELINE_LEN;
                    fprintf(channel_trace[i], "0x%08X READ %d\n", rd_edge_addr * 4, clk * 5);

                    for (int j = 0; j < CACHELINE_LEN; ++ j) {
                        if (edge_valid[i * GROUP_CORE_NUM + j]) {
                            active_v_id[i * GROUP_CORE_NUM + j] = edge_mem[rd_edge_addr + j].first;
                            active_edge[i * GROUP_CORE_NUM + j] = edge_mem[rd_edge_addr + j].second;
                        }
                        else {
                            active_v_id[i * GROUP_CORE_NUM + j] = -1;
                            active_edge[i * GROUP_CORE_NUM + j] = -1;
                        }
                    }

                    visited_edge_num += 16;
                }
                else {
                    for (int j = 0; j < CACHELINE_LEN; ++ j) {
                        active_v_id[i * GROUP_CORE_NUM + j] = -1;
                        active_edge[i * GROUP_CORE_NUM + j] = -1;
                    }
                }
            }

            //schedule
            for (int i = 0; i < CORE_NUM; ++ i) {
                if (push_flag) {
                    int dst_id = active_edge[i];

                    hyperx_1_src_id[i] = dst_id;
                    hyperx_1_dst_id[i] = dst_id;
                }
                else {
                    if (active_edge[i] != -1) {
                        int src = active_edge[i];
                        int dst = active_v_id[i];

                        schedule_src_id_buffer[i].push(src);
                        schedule_dst_id_buffer[i].push(dst);
                    }

                    if (active_v_id_buffer_2[i].size() != 0) {
                        int src = edge_mem[active_v_l_offset_buffer_2[i].front()].second;
                        int dst = active_v_id_buffer_2[i].front();

                        if (active_v_l_offset_buffer_2[i].front() < active_v_r_offset_buffer_2[i].front()) {

                            schedule_src_id_buffer_2[i].push(src);
                            schedule_dst_id_buffer_2[i].push(dst);
                        }
                        else {
                            vis_bitmap[dst] = 1; //some error here
                        }

                        active_v_id_buffer_2[i].pop();
                        active_v_l_offset_buffer_2[i].pop();
                        active_v_r_offset_buffer_2[i].pop();
                    }

                    if (schedule_src_id_buffer[i].size() > schedule_src_id_buffer_2[i].size()) {
                        int src_id = schedule_src_id_buffer[i].front();
                        int dst_id = schedule_dst_id_buffer[i].front();

                        hyperx_1_src_id[i] = src_id;
                        hyperx_1_dst_id[i] = dst_id;

                        schedule_src_id_buffer[i].pop();
                        schedule_dst_id_buffer[i].pop();
                    }
                    else {
                        if (schedule_src_id_buffer_2[i].size() != 0) {
                            int src_id = schedule_src_id_buffer_2[i].front();
                            int dst_id = schedule_dst_id_buffer_2[i].front();

                            hyperx_1_src_id[i] = src_id;
                            hyperx_1_dst_id[i] = dst_id;

                            schedule_src_id_buffer_2[i].pop();
                            schedule_dst_id_buffer_2[i].pop();

                            visited_edge_num++;
                        }
                        else {
                            hyperx_1_src_id[i] = -1;
                            hyperx_1_dst_id[i] = -1;
                        }
                    }
                }
            }
            /*
            if (iteration_id == 2){
                for (int i = 0; i < CORE_NUM; ++ i)
                    fprintf(p1, "%d ", hyperx_1_src_id[i]);
                fprintf(p1, "\n");
            }
            */
            //hyperx network
            int hyperx_empty_1 = hyperx_network_1();

            for (int i = 0; i < CORE_NUM; ++ i){
                hyperx_2_src_id[i] = hyperx_1_src_id_out[i];
                hyperx_2_dst_id[i] = hyperx_1_dst_id_out[i];
            }

            int hyperx_empty_2 = hyperx_network_2();

            for (int i = 0; i < CORE_NUM; ++ i) {
                if (push_flag) {
                    int dst_id = hyperx_2_dst_id_out[i];
                    if (dst_id != -1) {
                        if (vertex_ram[dst_id] > iteration_id + 1) {
                            vertex_ram[dst_id] = iteration_id + 1;
                            active_v_queue[(iteration_id + 1) % 2][dst_id % CORE_NUM].push(dst_id);
                            vis_bitmap[dst_id] = 1;

                            if (vertex_ram[dst_id] != origin_bfs_ans[dst_id]) {
                                cout << dst_id << " " << vertex_ram[dst_id] << " " << origin_bfs_ans[dst_id] << endl;
                                return -1;
                            }
                        }
                    }
                }
                else {
                    int src_id = hyperx_2_src_id_out[i];
                    int dst_id = hyperx_2_dst_id_out[i];
                    if (src_id != -1) {
                        //first edge
                        if (src_id == edge_mem[loffset_bram[dst_id]].second) {
                            if (vertex_ram[src_id] == iteration_id) {
                                vertex_ram[dst_id] = iteration_id + 1;
                                if (iteration_id == 4) active_v_queue[(iteration_id + 1) % 2][dst_id % CORE_NUM].push(dst_id);
                                vis_bitmap[dst_id] = 1;

                                if (vertex_ram[dst_id] != origin_bfs_ans[dst_id]) {
                                    cout << "pull schedule 2 " << dst_id << " " << vertex_ram[dst_id] << " " << origin_bfs_ans[dst_id] << endl;
                                    return -1;
                                }
                            }
                            else {
                                if (loffset_bram[dst_id] + 1 < roffset_bram[dst_id]) {
                                    active_v_id_buffer[i].push(dst_id);
                                    active_v_l_offset_buffer[i].push(loffset_bram[dst_id] + 1);
                                    active_v_r_offset_buffer[i].push(roffset_bram[dst_id]);
                                }
                            }
                        }
                        else {
                            if (vertex_ram[src_id] == iteration_id) {
                                vertex_ram[dst_id] = iteration_id + 1;
                                if (iteration_id == 4) active_v_queue[(iteration_id + 1) % 2][dst_id % CORE_NUM].push(dst_id);
                                vis_bitmap[dst_id] = 1;


                                if (vertex_ram[dst_id] != origin_bfs_ans[dst_id]) {
                                    cout << "pull schedule 1 " << dst_id << " " << vertex_ram[dst_id] << " " << origin_bfs_ans[dst_id] << endl;
                                    return -1;
                                }
                            }
                        }
                    }
                }
            }

            int iteration_end = hyperx_empty_1 && hyperx_empty_2;
            for (int i = 0; i < CORE_NUM; ++ i) {
                if (push_flag) {
                    iteration_end &= (active_v_queue[iteration_id % 2][i].size() == 0);
                }
                else {
                    iteration_end &= (bitmap_line_id[i] * CORE_NUM + i >= v_num);
                }
                iteration_end &= (active_v_id_buffer[i].size() == 0);
            }
            if (iteration_end) {
                int updated_vertex_num = 0;
                for (int i = 0; i < v_num; ++ i) {
                    if (vertex_ram[i] == iteration_id + 1)
                        updated_vertex_num ++;
                }
                cout << "iteration " << iteration_id << " end at clk " << clk << " updated vertice " << updated_vertex_num << " visited edges " <<  visited_edge_num << endl;
                iteration_id ++;

                for (int i = 0; i < CORE_NUM; ++ i) bitmap_line_id[i] = 0;
            }

            int all_end = 1;

            for (int i = 0; i < CORE_NUM; ++ i) {
                if (push_flag) {
                    all_end &= (active_v_queue[iteration_id % 2][i].size() == 0);
                }
                else {
                    all_end = 0;
                }
            }
            if (iteration_end && all_end || iteration_id >= 8) {
                cout << e_num << " " << visited_edge_num << " " << e_num / (2 * 5 * clk) << "GTEPS" << endl;
                break;
            }
        }
    }

    fclose(p1);
    for (int i = 0; i < v_num; ++ i) {
        if (vertex_ram[i] != origin_bfs_ans[i]) {
            cout << i << " " << vertex_ram[i] << " " << origin_bfs_ans[i] << endl;
            return -1;
        }
    }
    FILE *p2 = fopen("test_ans.txt", "w");
    for (int i = 0; i <v_num; ++ i) {
        fprintf(p2, "%d\n", vis_bitmap[i]);
    }
    fclose(p2);
}

int hyperx_network_1() {
    static queue<int> row_router_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], row_router_buffer2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static queue<int> column_router_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], column_router_buffer2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static queue<int> queue_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], queue_buffer2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static queue<int> hyper_out[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], hyper_out2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];

    int row_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], column_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], queue_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];

    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            row_buffer_empty[i][j] = (row_router_buffer[i][j].size() == 0);
            column_buffer_empty[i][j] = (column_router_buffer[i][j].size() == 0);
            queue_buffer_empty[i][j] = (queue_buffer[i][j].size() == 0);
        }
    }

    for (int i = 0; i < CORE_NUM; ++ i) {
        if (hyperx_1_src_id[i] != -1) {
            queue_buffer[i / GROUP_CORE_NUM][i % GROUP_CORE_NUM].push(hyperx_1_src_id[i]);
            queue_buffer2[i / GROUP_CORE_NUM][i % GROUP_CORE_NUM].push(hyperx_1_dst_id[i]);
        }
    }

    //queue routing
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (!queue_buffer_empty[i][j]) {
                int dst_id = queue_buffer[i][j].front();
                int another_id = queue_buffer2[i][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;

                if (vertex_not_read_flag[dst_id]) {
                    hyper_out[i][j].push(dst_id);
                    hyper_out2[i][j].push(another_id);
                }
                else {
                if (row_id == i) {
                    row_router_buffer[i][j].push(dst_id);
                    row_router_buffer2[i][j].push(another_id);
                }
                else {
                    if (col_id == j) {
                        column_router_buffer[i][j].push(dst_id);
                        column_router_buffer2[i][j].push(another_id);
                    }
                    else {
                        if (row_router_buffer[i][j].size() > column_router_buffer[i][j].size()) {
                            row_router_buffer[i][j].push(dst_id);
                            row_router_buffer2[i][j].push(another_id);
                        }
                        else {
                            column_router_buffer[i][j].push(dst_id);
                            column_router_buffer2[i][j].push(another_id);
                        }
                    }
                }
                }
                queue_buffer[i][j].pop();
                queue_buffer2[i][j].pop();
            }
        }
    }

    //row routing
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (!row_buffer_empty[i][j]) {
                int dst_id = row_router_buffer[i][j].front();
                int another_id = row_router_buffer2[i][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;

                if (col_id != j) {
                    row_router_buffer[i][col_id].push(dst_id);
                    row_router_buffer2[i][col_id].push(another_id);
                }
                else {
                    if (row_id != i) {
                        column_router_buffer[i][j].push(dst_id);
                        column_router_buffer2[i][j].push(another_id);
                    }
                    else {
                        hyper_out[i][j].push(dst_id);
                        hyper_out2[i][j].push(another_id);
                    }
                }

                row_router_buffer[i][j].pop();
                row_router_buffer2[i][j].pop();
            }
        }
    }

    //column routing
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (!column_buffer_empty[i][j]) {
                int dst_id = column_router_buffer[i][j].front();
                int another_id = column_router_buffer2[i][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;

                if (row_id != i) {
                    column_router_buffer[row_id][j].push(dst_id);
                    column_router_buffer2[row_id][j].push(another_id);
                }
                else {
                    if (col_id != j) {
                        row_router_buffer[i][j].push(dst_id);
                        row_router_buffer2[i][j].push(another_id);
                    }
                    else {
                        hyper_out[i][j].push(dst_id);
                        hyper_out2[i][j].push(another_id);
                    }
                }

                column_router_buffer[i][j].pop();
                column_router_buffer2[i][j].pop();
            }
        }
    }

    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (hyper_out[i][j].size() != 0) {
                int dst_id = hyper_out[i][j].front();
                int another_id = hyper_out2[i][j].front();

                //if ((dst_id % CORE_NUM) / GROUP_CORE_NUM != i || dst_id % GROUP_CORE_NUM != j) {
                //    cout << "error " << dst_id << " " << i << " " << j;
                //    return -1;
                //}
                hyperx_1_src_id_out[i * GROUP_CORE_NUM + j] = dst_id;
                hyperx_1_dst_id_out[i * GROUP_CORE_NUM + j] = another_id;

                hyper_out[i][j].pop();
                hyper_out2[i][j].pop();
            }
            else {
                hyperx_1_src_id_out[i * GROUP_CORE_NUM + j] = -1;
                hyperx_1_dst_id_out[i * GROUP_CORE_NUM + j] = -1;
            }
        }
    }

    int all_empty = 1;
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            all_empty &= (queue_buffer[i][j].size() == 0);
            all_empty &= (row_router_buffer[i][j].size() == 0);
            all_empty &= (column_router_buffer[i][j].size() == 0);
            all_empty &= (hyper_out[i][j].size() == 0);
        }
    }
    return all_empty;
}

int hyperx_network_2() {
    static queue<int> row_router_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], row_router_buffer2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static queue<int> column_router_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], column_router_buffer2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static queue<int> queue_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], queue_buffer2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static queue<int> hyper_out[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], hyper_out2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];

    int row_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], column_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], queue_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];

    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            row_buffer_empty[i][j] = (row_router_buffer[i][j].size() == 0);
            column_buffer_empty[i][j] = (column_router_buffer[i][j].size() == 0);
            queue_buffer_empty[i][j] = (queue_buffer[i][j].size() == 0);
        }
    }

    for (int i = 0; i < CORE_NUM; ++ i) {
        if (hyperx_2_src_id[i] != -1) {
            queue_buffer[i / GROUP_CORE_NUM][i % GROUP_CORE_NUM].push(hyperx_2_dst_id[i]);
            queue_buffer2[i / GROUP_CORE_NUM][i % GROUP_CORE_NUM].push(hyperx_2_src_id[i]);
        }
    }

    //queue routing
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (!queue_buffer_empty[i][j]) {
                int dst_id = queue_buffer[i][j].front();
                int another_id = queue_buffer2[i][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;

                if (row_id == i) {
                    row_router_buffer[i][j].push(dst_id);
                    row_router_buffer2[i][j].push(another_id);
                }
                else {
                    if (col_id == j) {
                        column_router_buffer[i][j].push(dst_id);
                        column_router_buffer2[i][j].push(another_id);
                    }
                    else {
                        if (row_router_buffer[i][j].size() > column_router_buffer[i][j].size()) {
                            row_router_buffer[i][j].push(dst_id);
                            row_router_buffer2[i][j].push(another_id);
                        }
                        else {
                            column_router_buffer[i][j].push(dst_id);
                            column_router_buffer2[i][j].push(another_id);
                        }
                    }
                }

                queue_buffer[i][j].pop();
                queue_buffer2[i][j].pop();
            }
        }
    }

    //row routing
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (!row_buffer_empty[i][j]) {
                int dst_id = row_router_buffer[i][j].front();
                int another_id = row_router_buffer2[i][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;

                if (col_id != j) {
                    row_router_buffer[i][col_id].push(dst_id);
                    row_router_buffer2[i][col_id].push(another_id);
                }
                else {
                    if (row_id != i) {
                        column_router_buffer[i][j].push(dst_id);
                        column_router_buffer2[i][j].push(another_id);
                    }
                    else {
                        hyper_out[i][j].push(dst_id);
                        hyper_out2[i][j].push(another_id);
                    }
                }

                row_router_buffer[i][j].pop();
                row_router_buffer2[i][j].pop();
            }
        }
    }

    //column routing
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (!column_buffer_empty[i][j]) {
                int dst_id = column_router_buffer[i][j].front();
                int another_id = column_router_buffer2[i][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;

                if (row_id != i) {
                    column_router_buffer[row_id][j].push(dst_id);
                    column_router_buffer2[row_id][j].push(another_id);
                }
                else {
                    if (col_id != j) {
                        row_router_buffer[i][j].push(dst_id);
                        row_router_buffer2[i][j].push(another_id);
                    }
                    else {
                        hyper_out[i][j].push(dst_id);
                        hyper_out2[i][j].push(another_id);
                    }
                }

                column_router_buffer[i][j].pop();
                column_router_buffer2[i][j].pop();
            }
        }
    }

    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (hyper_out[i][j].size() != 0) {
                int dst_id = hyper_out[i][j].front();
                int another_id = hyper_out2[i][j].front();

                //if ((dst_id % CORE_NUM) / GROUP_CORE_NUM != i || dst_id % GROUP_CORE_NUM != j) {
                //    cout << "error " << dst_id << " " << i << " " << j;
                //    return -1;
                //}
                hyperx_2_src_id_out[i * GROUP_CORE_NUM + j] = another_id;
                hyperx_2_dst_id_out[i * GROUP_CORE_NUM + j] = dst_id;

                hyper_out[i][j].pop();
                hyper_out2[i][j].pop();
            }
            else {
                hyperx_2_src_id_out[i * GROUP_CORE_NUM + j] = -1;
                hyperx_2_dst_id_out[i * GROUP_CORE_NUM + j] = -1;
            }
        }
    }

    int all_empty = 1;
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            all_empty &= (queue_buffer[i][j].size() == 0);
            all_empty &= (row_router_buffer[i][j].size() == 0);
            all_empty &= (column_router_buffer[i][j].size() == 0);
            all_empty &= (hyper_out[i][j].size() == 0);
        }
    }
    return all_empty;
}

