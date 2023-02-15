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

int tmp_om1_v_id_in[OMEGA_DEPTH + 1][CORE_NUM], tmp_om1_v_value_in[OMEGA_DEPTH + 1][CORE_NUM];
int tmp_om2_v_id_in[OMEGA_DEPTH + 1][CORE_NUM], tmp_om2_v_value_in[OMEGA_DEPTH + 1][CORE_NUM];

int array2n[10];

int Omega_Network_1();
int Omega_Network_2();

int Omega_Network_1_Single(int Omega_Depth_ID, int Omega_Switch_ID,
                            int V_ID_In1, int V_Value_In1,
                            int V_ID_In2, int V_Value_In2,

                            int *Om1_V_ID_Out1, int *Om1_V_Value_Out1,
                            int *Om1_V_ID_Out2, int *Om1_V_Value_Out2);

int Omega_Network_2_Single(int Omega_Depth_ID, int Omega_Switch_ID,
                            int V_ID_In1, int V_Value_In1,
                            int V_ID_In2, int V_Value_In2,

                            int *Om2_V_ID_Out1, int *Om2_V_Value_Out1,
                            int *Om2_V_ID_Out2, int *Om2_V_Value_Out2);

int vertex_not_read[32] = {798169, 802022, 859188, 737253, 236717, 800651, 884620, 1038374, 246686, 72581,
                           514920, 939780, 433575, 644849, 887126, 817258, 812863, 802479, 701232, 798386,
                           1030895, 609384, 504066, 426492, 16463, 644226, 506990, 1045487, 628265, 735673,
                           507966, 584249};
int vertex_not_read_flag[MAX_VERTEX_NUM];

int ROOT;
int v_num, e_num;
int clk, rst, push_flag, iteration_id;
int visited_edge_num = 0;

int main()
{
    cout << "Input ROOT ID: ";
    cin >> ROOT;

    //initialization
    FILE *fp;
    fp = fopen("./sim_channel_0_addr_trace.txt", "w");
    FILE *p1;
    int src, dst;

    p1 = fopen("../data/test_graph20_degree_reorder.txt","r");
    if (p1 == NULL) {
        cout << "input file error" << endl;
        return -1;
    }

    for (int i = 0; i < OMEGA_DEPTH; ++ i) array2n[i] = 1 << i;

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

    memset(tmp_om1_v_id_in,-1,sizeof(tmp_om1_v_id_in));
    memset(tmp_om1_v_value_in,-1,sizeof(tmp_om1_v_id_in));
    memset(tmp_om2_v_id_in,-1,sizeof(tmp_om1_v_id_in));
    memset(tmp_om2_v_value_in,-1,sizeof(tmp_om1_v_id_in));

    memset(vertex_not_read_flag, 0, sizeof(vertex_not_read_flag));
    for (int i = 0; i < v_num; ++ i)
        for (int j = 0; j < 32; ++ j)
            if (i == vertex_not_read[j])
                vertex_not_read_flag[i] = 1;

    p1 = fopen("debug_trace2.txt", "w");
    for (clk = 0; clk < MAX_CLK; clk ++) {
        if (clk % 100000 == 0) cout << clk << endl;
        if (clk < 100) {
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

                    for (int j = 0; j < CACHELINE_LEN; ++ j) {
                        if (edge_valid[i * GROUP_CORE_NUM + j]) {
                            active_v_id[i * GROUP_CORE_NUM + j] = edge_mem[rd_edge_addr + j].first;
                            active_edge[i * GROUP_CORE_NUM + j] = edge_mem[rd_edge_addr + j].second;
                            if (i == 0) fprintf(fp, "%d %d\n", active_v_id[i * GROUP_CORE_NUM + j], rd_edge_addr);
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
                    
                    tmp_om1_v_id_in[0][i] = dst_id;
                    tmp_om1_v_value_in[0][i] = dst_id;
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
                        
                        tmp_om1_v_id_in[0][i] = vertex_not_read_flag[src_id] ? dst_id : src_id;
                        tmp_om1_v_value_in[0][i] = dst_id;

                        schedule_src_id_buffer[i].pop();
                        schedule_dst_id_buffer[i].pop();
                    }
                    else {
                        if (schedule_src_id_buffer_2[i].size() != 0) {
                            int src_id = schedule_src_id_buffer_2[i].front();
                            int dst_id = schedule_dst_id_buffer_2[i].front();
                            
                            tmp_om1_v_id_in[0][i] = vertex_not_read_flag[src_id] ? dst_id : src_id;
                            tmp_om1_v_value_in[0][i] = dst_id;

                            schedule_src_id_buffer_2[i].pop();
                            schedule_dst_id_buffer_2[i].pop();

                            visited_edge_num++;
                        }
                        else {
                            tmp_om1_v_id_in[0][i] = -1;
                            tmp_om1_v_value_in[0][i] = -1;
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
            int omega_empty_1 = Omega_Network_1();

            for (int i = 0; i < CORE_NUM; ++ i){
                tmp_om2_v_id_in[0][i] = tmp_om1_v_id_in[9][i];
                tmp_om2_v_value_in[0][i] = tmp_om1_v_value_in[9][i];
            }
            
            int omega_empty_2 = Omega_Network_2();

            for (int i = 0; i < CORE_NUM; ++ i) {
                if (push_flag) {
                    int dst_id = tmp_om2_v_id_in[9][i];
                    if (dst_id!=-1) {
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
                    int src_id = tmp_om2_v_id_in[9][i];
                    int dst_id = tmp_om2_v_value_in[9][i];
                    if (src_id!=-1) {
                        if (src_id == dst_id) {
                            // power-law case
                            vertex_ram[dst_id] = iteration_id + 1;
                            if (iteration_id == 4) active_v_queue[(iteration_id + 1) % 2][dst_id % CORE_NUM].push(dst_id);
                            vis_bitmap[dst_id] = 1;
                        }
                        //first edge
                        else if (src_id == edge_mem[loffset_bram[dst_id]].second) {
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

            int iteration_end = omega_empty_1 && omega_empty_2;
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
            if ((iteration_end && all_end) || iteration_id >= 6) {
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

int Omega_Network_1() {
    int empty_all = 1;
    for (int i = OMEGA_DEPTH; i >= 1; -- i) {
        for (int j = 0; j < OMEGA_SWITCH_NUM; ++ j) {
            int in_id1 = j;
            int in_id2 = j + OMEGA_SWITCH_NUM;
            int out_id1 = j * 2;
            int out_id2 = j * 2 + 1;
            //int out_id1 = (j < OMEGA_SWITCH_NUM / 2) ? (j * 4) : (j * 4 - CORE_NUM + 1);
            //int out_id2 = (j < OMEGA_SWITCH_NUM / 2) ? (j * 4 + 2) : (j * 4 - CORE_NUM + 3);

            int empty = Omega_Network_1_Single(i - 1, j,
                                   tmp_om1_v_id_in[i - 1][in_id1], tmp_om1_v_value_in[i - 1][in_id1],
                                   tmp_om1_v_id_in[i - 1][in_id2], tmp_om1_v_value_in[i - 1][in_id2],

                                   &tmp_om1_v_id_in[i][out_id1], &tmp_om1_v_value_in[i][out_id1],
                                   &tmp_om1_v_id_in[i][out_id2], &tmp_om1_v_value_in[i][out_id2]);
            empty_all = empty_all & empty;
        }
    }

    return empty_all;
}

int Omega_Network_2() {
    int empty_all = 1;

    for (int i = OMEGA_DEPTH; i >= 1; -- i) {
        for (int j = 0; j < OMEGA_SWITCH_NUM; ++ j) {
            int in_id1 = j;
            int in_id2 = j + OMEGA_SWITCH_NUM;
            int out_id1 = j * 2;
            int out_id2 = j * 2 + 1;
            //int out_id1 = (j < OMEGA_SWITCH_NUM / 2) ? (j * 4) : (j * 4 - CORE_NUM + 1);
            //int out_id2 = (j < OMEGA_SWITCH_NUM / 2) ? (j * 4 + 2) : (j * 4 - CORE_NUM + 3);

            int empty = Omega_Network_2_Single(i - 1, j,
                                   tmp_om2_v_id_in[i - 1][in_id1], tmp_om2_v_value_in[i - 1][in_id1],
                                   tmp_om2_v_id_in[i - 1][in_id2], tmp_om2_v_value_in[i - 1][in_id2],

                                   &tmp_om2_v_id_in[i][out_id1], &tmp_om2_v_value_in[i][out_id1],
                                   &tmp_om2_v_id_in[i][out_id2], &tmp_om2_v_value_in[i][out_id2]);
            empty_all = empty_all & empty;
        }
    }

    return empty_all;
}

int Omega_Network_1_Single(int Omega_Depth_ID, int Omega_Switch_ID,
                            int V_ID_In1, int V_Value_In1,
                            int V_ID_In2, int V_Value_In2,

                            int *Om1_V_ID_Out1, int *Om1_V_Value_Out1,
                            int *Om1_V_ID_Out2, int *Om1_V_Value_Out2) {

    static queue<int> id_buffer_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    static queue<int> id_buffer_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    static queue<int> id_buffer_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    static queue<int> id_buffer_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    static int buffer_empty_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], buffer_empty_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM];
    static int buffer_empty_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], buffer_empty_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM];

    int in1_port_id = Omega_Switch_ID, in2_port_id = Omega_Switch_ID + OMEGA_SWITCH_NUM;
    int in1_src_route = in1_port_id & array2n[OMEGA_DEPTH - 1], in2_src_route = in2_port_id & array2n[OMEGA_DEPTH - 1];
    int in1_dst_route = push_flag ? ((V_ID_In1 << Omega_Depth_ID) & array2n[OMEGA_DEPTH - 1]) : ((V_ID_In1 << Omega_Depth_ID) & array2n[OMEGA_DEPTH - 1]);
    int in2_dst_route = push_flag ? ((V_ID_In2 << Omega_Depth_ID) & array2n[OMEGA_DEPTH - 1]) : ((V_ID_In2 << Omega_Depth_ID) & array2n[OMEGA_DEPTH - 1]);
    int in1_route_flag = (in1_src_route ^ in1_dst_route), in2_route_flag = (in2_src_route ^ in2_dst_route);

    if (rst) {
        /*buffer initilization*/
    }
    else {
        if (V_ID_In1!=-1) {
            if (in1_route_flag == 0) {
                id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].push(V_ID_In1);
                value_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].push(V_Value_In1);
            }
            else {
                id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].push(V_ID_In1);
                value_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].push(V_Value_In1);
            }
        }
        if (V_ID_In2!=-1) {
            if (in2_route_flag == 0) {
                id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].push(V_ID_In2);
                value_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].push(V_Value_In2);
            }
            else {
                id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].push(V_ID_In2);
                value_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].push(V_Value_In2);
            }
        }
    }

    buffer_empty_in1_out1[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].size() == 0);
    buffer_empty_in1_out2[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].size() == 0);
    buffer_empty_in2_out1[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].size() == 0);
    buffer_empty_in2_out2[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].size() == 0);

    if (rst) {
        *Om1_V_ID_Out1 = -1;
        *Om1_V_Value_Out1 = -1;
    }
    else {
        if (id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].size() > id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].size()) { //in1_out1 cannot be empty
            *Om1_V_ID_Out1 = id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om1_V_Value_Out1 = value_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].front();

            id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
            value_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        }
        else {
            if (buffer_empty_in2_out1[Omega_Depth_ID][Omega_Switch_ID]) {
                *Om1_V_ID_Out1 = -1;
                *Om1_V_Value_Out1 = -1;
            }
            else {
                *Om1_V_ID_Out1 = id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om1_V_Value_Out1 = value_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].front();

                id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
                value_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
            }
        }
    }

    if (rst) {
        *Om1_V_ID_Out2 = -1;
        *Om1_V_Value_Out2 = -1;
    }
    else {
        if (id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].size() > id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].size()) { //in1_out1 cannot be empty
            *Om1_V_ID_Out2 = id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om1_V_Value_Out2 = value_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].front();

            id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
            value_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
        }
        else {
            if (buffer_empty_in2_out2[Omega_Depth_ID][Omega_Switch_ID]) {
                *Om1_V_ID_Out2 = -1;
                *Om1_V_Value_Out2 = -1;
            }
            else {
                *Om1_V_ID_Out2 = id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om1_V_Value_Out2 = value_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].front();

                id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
                value_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
            }
        }
    }

    return buffer_empty_in1_out1[Omega_Depth_ID][Omega_Switch_ID] && buffer_empty_in2_out1[Omega_Depth_ID][Omega_Switch_ID] && buffer_empty_in1_out2[Omega_Depth_ID][Omega_Switch_ID] && buffer_empty_in2_out2[Omega_Depth_ID][Omega_Switch_ID];
}

int Omega_Network_2_Single(int Omega_Depth_ID, int Omega_Switch_ID,
                            int V_ID_In1, int V_Value_In1,
                            int V_ID_In2, int V_Value_In2,

                            int *Om2_V_ID_Out1, int *Om2_V_Value_Out1,
                            int *Om2_V_ID_Out2, int *Om2_V_Value_Out2) {

    static queue<int> id_buffer_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    static queue<int> id_buffer_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    static queue<int> id_buffer_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    static queue<int> id_buffer_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], value_buffer_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM]; //原OMEGA_DEPTH+1+1 为 OMEGA_DEPTH+1
    static int buffer_empty_in1_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], buffer_empty_in1_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM];
    static int buffer_empty_in2_out1[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM], buffer_empty_in2_out2[OMEGA_DEPTH+1][OMEGA_SWITCH_NUM];

    int in1_port_id = Omega_Switch_ID, in2_port_id = Omega_Switch_ID + OMEGA_SWITCH_NUM;
    int in1_src_route = in1_port_id & array2n[OMEGA_DEPTH - 1], in2_src_route = in2_port_id & array2n[OMEGA_DEPTH - 1];
    int in1_dst_route = ((V_Value_In1 << Omega_Depth_ID) & array2n[OMEGA_DEPTH - 1]), in2_dst_route =  ((V_Value_In2 << Omega_Depth_ID) & array2n[OMEGA_DEPTH - 1]);
    int in1_route_flag = (in1_src_route ^ in1_dst_route), in2_route_flag = (in2_src_route ^ in2_dst_route);

    if (rst) {
        /*buffer initilization*/
        for (int i = 0; i < OMEGA_DEPTH; ++ i) array2n[i] = 1 << i;
    }
    else {
        if (V_ID_In1!=-1) {
            if (in1_route_flag == 0) {
                id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].push(V_ID_In1);
                value_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].push(V_Value_In1);
            }
            else {
                id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].push(V_ID_In1);
                value_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].push(V_Value_In1);
            }
        }
        if (V_ID_In2!=-1) {
            if (in2_route_flag == 0) {
                id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].push(V_ID_In2);
                value_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].push(V_Value_In2);
            }
            else {
                id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].push(V_ID_In2);
                value_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].push(V_Value_In2);
            }
        }
    }

    buffer_empty_in1_out1[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].size() == 0);
    buffer_empty_in1_out2[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].size() == 0);
    buffer_empty_in2_out1[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].size() == 0);
    buffer_empty_in2_out2[Omega_Depth_ID][Omega_Switch_ID] = (id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].size() == 0);

    if (rst) {
        *Om2_V_ID_Out1 = -1;
        *Om2_V_Value_Out1 = -1;
    }
    else {
        if (id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].size() > id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].size()) { //in1_out1 cannot be empty
            *Om2_V_ID_Out1 = id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om2_V_Value_Out1 = value_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].front();

            id_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
            value_buffer_in1_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
        }
        else {
            if (buffer_empty_in2_out1[Omega_Depth_ID][Omega_Switch_ID]) {
                *Om2_V_ID_Out1 = -1;
                *Om2_V_Value_Out1 = -1;
            }
            else {
                *Om2_V_ID_Out1 = id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om2_V_Value_Out1 = value_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].front();

                id_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
                value_buffer_in2_out1[Omega_Depth_ID][Omega_Switch_ID].pop();
            }
        }
    }

    if (rst) {
        *Om2_V_ID_Out2 = -1;
        *Om2_V_Value_Out2 = -1;
    }
    else {
        if (id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].size() > id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].size()) { //in1_out1 cannot be empty
            *Om2_V_ID_Out2 = id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].front();
            *Om2_V_Value_Out2 = value_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].front();

            id_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
            value_buffer_in1_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
        }
        else {
            if (buffer_empty_in2_out2[Omega_Depth_ID][Omega_Switch_ID]) {
                *Om2_V_ID_Out2 = -1;
                *Om2_V_Value_Out2 = -1;
            }
            else {
                *Om2_V_ID_Out2 = id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].front();
                *Om2_V_Value_Out2 = value_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].front();

                id_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
                value_buffer_in2_out2[Omega_Depth_ID][Omega_Switch_ID].pop();
            }
        }
    }

    return buffer_empty_in1_out1[Omega_Depth_ID][Omega_Switch_ID] && buffer_empty_in2_out1[Omega_Depth_ID][Omega_Switch_ID] && buffer_empty_in1_out2[Omega_Depth_ID][Omega_Switch_ID] && buffer_empty_in2_out2[Omega_Depth_ID][Omega_Switch_ID];
}