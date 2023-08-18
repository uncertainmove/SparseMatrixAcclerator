#include "acc.h"

using namespace std;

// const int v_num = 65535, e_num = 1819290;
const int v_num = 1048576, e_num = 31400738;

int hyperx_1_src_id[CORE_NUM], hyperx_1_dst_id[CORE_NUM];
int hyperx_1_src_id_out[CORE_NUM], hyperx_1_dst_id_out[CORE_NUM];
int row_buffer_empty_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], column_buffer_empty_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], queue_buffer_empty_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
int column_buffer_empty_time_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
static queue<int> row_router_buffer_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], row_router_buffer2_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
static queue<int> column_router_buffer_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], column_router_buffer2_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
static queue<int> queue_buffer_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], queue_buffer2_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
static queue<int> hyper_out_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], hyper_out2_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];

static queue<int> row_router_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], row_router_buffer2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
static queue<int> column_router_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], column_router_buffer2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
static queue<int> queue_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], queue_buffer2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
static queue<int> hyper_out[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], hyper_out2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
int hyperx_2_src_id[CORE_NUM], hyperx_2_dst_id[CORE_NUM];
int hyperx_2_src_id_out[CORE_NUM], hyperx_2_dst_id_out[CORE_NUM];
int row_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], column_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], queue_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
int column_buffer_empty_time[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];

int edge_off[v_num + 1];
int edge_list[e_num];

int pe_load[CORE_NUM]; // recv task load
int pe_load2[CORE_NUM]; // exec task load

int send_edge = 0;
bool p_flag = true, q_flag = true, z_flag = true;
int clk;
FILE *p3;

int hyperx_network_1();
int hyperx_network_2();

void degree_check() {
    vector<pair<int, int>> degree_vec;
    int v_new_num = 0;
    for (int i = 0; i < v_num; i++) {
        degree_vec.push_back(pair<int, int>(edge_off[i + 1] - edge_off[i], i));
        if (edge_off[i + 1] > edge_off[i]) v_new_num++;
    }
    cout << "v_new_num " << v_new_num << endl;
    sort(degree_vec.begin(), degree_vec.end(), greater<pair<int, int>>());
    FILE *fp;
    fp = fopen("./degree_sort.txt", "w");
    int sum_ct[100];
    for (int i = 0; i < 100; i++) {
        sum_ct[i] = 0;
    }
    int cu_percent = 1;
    for (int i = 0; i < v_num; i++) {
        if (degree_vec[i].first == 0) break;
        fprintf(fp, "%d %d\n", degree_vec[i].second, degree_vec[i].first);
        if (i < 0.01 * cu_percent * v_new_num) {
            sum_ct[cu_percent - 1] += degree_vec[i].first;
        } else {
            cu_percent += 1;
            if (cu_percent - 1 >= 100) {
                cout << "error cu_percent" << endl;
                exit(-1);
            }
            sum_ct[cu_percent - 1] += degree_vec[i].first;
        }
    }
    for (int i = 0; i < 100; i++) {
        cout << sum_ct[i] << endl;
    }
    fclose(fp);
    int _pe_load[CORE_NUM];
    vector<int> _pe_belong[CORE_NUM];
    for (int i = 0; i < CORE_NUM; i++) {
        _pe_load[i] = 0;
    }
    for (int i = 0; i < 700; i++) {
        int core_id = degree_vec[i].second % CORE_NUM;
        _pe_load[core_id] += degree_vec[i].first;
        _pe_belong[core_id].push_back(degree_vec[i].second);
    }
    for (int i = 0; i < CORE_NUM; i++) {
        // cout << _pe_load[i] << " " << i << ":";
        // cout << _pe_load[i] << endl;
        for (int j = 0; j < _pe_belong[i].size(); j++) {
            // cout << " " << _pe_belong[i][j];
        }
        // cout << endl;
    }
}

int main()
{
    int rst;

    //initialization
    FILE *p1, *p2;
    int src, dst;

    p1 = fopen("../simulator/data/edge_off.txt","r");
    p2 = fopen("../simulator/data/edge_list.txt","r");
    p3 = fopen("./pe25_trace.txt", "w");
    if (p1 == NULL || p2 == NULL || p3 == NULL) {
        cout << "input file error" << endl;
        return -1;
    }

    for (int i = 0; i <= v_num; ++ i) {
        fscanf(p1, "%d", &edge_off[i]);
    }
    for (int i = 0; i < e_num; ++ i) {
        if (i % 1000000 == 0) cout << "now: " <<  i << endl;
        fscanf(p2, "%d", &edge_list[i]);
        //if (i < 10) cout << src << " " << dst << endl;
    }
    fclose(p1);
    fclose(p2);

    // degree_check();
    // return 0;

    int edge = 0;
    vector<pair<int, int>> tasks[PSEUDO_CHANNEL_NUM];
    for (int i = 0; i < v_num; i++) {
        int channel_id = (i % CORE_NUM) / GROUP_CORE_NUM;
        if (i == v_num - 1) {
            cout << "loff " << edge_off[i] << " roff " << edge_off[i + 1] << endl;
        }
        for (int off = edge_off[i]; off < edge_off[i + 1]; off++) {
            tasks[channel_id].push_back(pair<int, int>(i, edge_list[off]));
            if (i == 0) {
                cout << i << " " << edge_list[off] << endl;
            }
            edge++;
        }
    }
    int tasks_index[PSEUDO_CHANNEL_NUM];
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        tasks_index[i] = 0;
    }

    /*
    vector<pair<int, int>> tasks[CORE_NUM];
    for (int i = 0; i < v_num; i++) {
        for (int off = edge_off[i]; off < edge_off[i + 1]; off++) {
            int pe_id = edge_list[off] % CORE_NUM;
            tasks[pe_id].push_back(pair<int, int>(i, edge_list[off]));
            edge++;
        }
    }
    int tasks_index[CORE_NUM];
    for (int i = 0; i < CORE_NUM; i++) {
        tasks_index[i] = 0;
    }
    */
    cout << "read edge " << edge << endl;

    for (int i = 0; i < CORE_NUM; i++) {
        hyperx_2_src_id[i] = -1;
        hyperx_2_dst_id[i] = -1;
        hyperx_1_src_id[i] = -1;
        hyperx_1_dst_id[i] = -1;

        pe_load[i] = 0;
        pe_load2[i] = 0;
    }
    int send_task = 0;
    for (clk = 1; clk < 10000000; clk++) {
        int n1_empty = hyperx_network_1();
        int n2_empty = hyperx_network_2();
        int send = 0;
        for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
            for (int j = 0; j < GROUP_CORE_NUM; j++) {
                int core_id = i * GROUP_CORE_NUM + j;
                if (tasks_index[i] < tasks[i].size()) {
                    hyperx_1_src_id[core_id] = tasks[i][tasks_index[i]].second;
                    hyperx_1_dst_id[core_id] = tasks[i][tasks_index[i]].first;
                    /*
                    if (hyperx_2_src_id[core_id] % CORE_NUM != core_id) {
                        cout << "hyperx 2 send task error" << endl;
                        exit(-1);
                    }
                    */
                    // pe_load[core_id] += 1;
                    // pe_load2[tasks[i][tasks_index[i]].second % CORE_NUM] += 1;

                    tasks_index[i] += 1;
                    send = 1;
                    send_task += 1;
                } else {
                    hyperx_1_src_id[core_id] = -1;
                    hyperx_1_dst_id[core_id] = -1;
                }
                hyperx_2_src_id[core_id] = hyperx_1_src_id_out[core_id];
                hyperx_2_dst_id[core_id] = hyperx_1_dst_id_out[core_id];
                if (hyperx_1_src_id_out[core_id] != -1 && hyperx_1_src_id_out[core_id] % CORE_NUM != core_id) {
                    cout << "hyperx 1 routing error" << endl;
                    cout << hyperx_1_src_id_out[core_id] << " " << hyperx_1_dst_id_out[core_id] << " " << core_id << endl;
                    exit(-1);
                }
                if (hyperx_2_dst_id_out[core_id] != -1 && hyperx_2_dst_id_out[core_id] % CORE_NUM != core_id) {
                    cout << "hyperx 2 routing error" << endl;
                    exit(-1);
                }
                if (hyperx_2_dst_id_out[core_id] == 798169) {
                    cout << "catch 798169 in core " << core_id << endl;
                    exit(-1);
                }
                if (core_id == 25 && hyperx_2_dst_id_out[core_id] != -1) {
                    fprintf(p3, "%d %d\n", clk, hyperx_2_dst_id_out[core_id]);
                }
            }
        }
        if (send == 0 && p_flag) {
            p_flag = false;
            cout << "clk " << clk << " send edge " << send_task << " over" << endl;
        }
        if (send == 0) {
            int empty = 1, empty2 = 1, empty3 = 1;
            for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
                for (int j = 0; j < GROUP_CORE_NUM; j++) {
                    empty &= queue_buffer_empty[i][j] & n1_empty;
                    empty2 &= row_buffer_empty[i][j];
                    empty3 &= column_buffer_empty[i][j];
                }
            }
            if (clk % 100000 == 0) {
                cout << clk << " send edge " << send_edge << endl;
            }
            if (empty && q_flag) {
                cout << "clk " << clk << " row routing over" << endl;
                // cout << "row buffer size" << endl;
                int max_same_sum = 0;
                int max_same_sum_ct[18];
                int rq_sum = 0;
                int same_798169_sum = 0;
                int rq_798169_sum = 0;
                int _clk = 0;
                for (int i = 0; i < 18; i++) {
                    max_same_sum_ct[i] = 0;
                }
                while (true) {
                    _clk += 1;
                    bool ex_flag = false;
                    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
                        int max_same = 0;
                        int same_798169 = 0;
                        int _ch_num = 19905;
                        bool _ch = false;
                        for (int j = 0; j < GROUP_CORE_NUM; j++) {
                            if (row_router_buffer[i][j].size() > 0) ex_flag = true;
                            if (row_router_buffer[i][j].size() == 0) continue;
                            if (row_router_buffer[i][j].front() == 798169) same_798169 += 1;
                            int same = 0;
                            for (int k = 0; k < GROUP_CORE_NUM; k++) {
                                if (row_router_buffer[i][k].size() > 0 && row_router_buffer[i][j].front() == row_router_buffer[i][k].front()) {
                                    same += 1;
                                }
                            }
                            max_same = max_same > same ? max_same : same;
                            // cout << row_router_buffer[i][j].size() << " ";
                            if (row_router_buffer[i][j].front() == _ch_num) {
                                _ch = true;
                            }
                        }
                        if (_ch) {
                            cout << _ch_num << " " << max_same << endl;
                            _ch = false;
                        }
                        if (max_same > 0) {
                            max_same_sum += max_same;
                            max_same_sum_ct[max_same] += 1;
                            rq_sum += 16;
                            if (same_798169 > 0) {
                                same_798169_sum += same_798169;
                                rq_798169_sum += 16;
                            }
                            for (int j = 0; j < GROUP_CORE_NUM; j++) {
                                if (row_router_buffer[i][j].size() > 0) {
                                    row_router_buffer[i][j].pop();
                                }
                            }
                        }
                    }
                    if (!ex_flag) break;
                }
                cout << max_same_sum << " " << rq_sum << endl;
                cout << same_798169_sum << " " << rq_798169_sum << endl;
                for (int i = 0; i < 18; i++) {
                    cout << max_same_sum_ct[i] << endl;
                }
                q_flag = false;
                fclose(p3);
                exit(0);
            }
            if (empty && empty2 && z_flag) {
                cout << "clk " << clk << " col routing over" << endl;
                cout << "column buffer size" << endl;
                for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
                    for (int j = 0; j < GROUP_CORE_NUM; j++) {
                        cout << column_router_buffer[i][j].size() << " ";
                    }
                    cout << endl;
                }
                z_flag = false;
            }
            if (empty && empty2 && empty3) {
                cout << "clk " << clk << " out over" << endl;
                break;
            }
        }
    }
    cout << "send edge " << send_edge << endl;
    cout << "pe load" << endl;
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        for (int j = 0; j < GROUP_CORE_NUM; j++) {
            cout << pe_load[i * GROUP_CORE_NUM + j] << " ";
        }
        cout << endl;
    }
    cout << "pe load2" << endl;
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        for (int j = 0; j < GROUP_CORE_NUM; j++) {
            cout << pe_load2[i * GROUP_CORE_NUM + j] << endl;
        }
        // cout << endl;
    }
    cout << "column empty time" << endl;
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        for (int j = 0; j < GROUP_CORE_NUM; j++) {
            cout << column_buffer_empty_time[i][j] << " ";
        }
        cout << endl;
    }
    return 0;
}

int hyperx_network_1() {

    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            row_buffer_empty_n1[i][j] = (row_router_buffer_n1[i][j].size() == 0);
            column_buffer_empty_n1[i][j] = (column_router_buffer_n1[i][j].size() == 0);
            queue_buffer_empty_n1[i][j] = (queue_buffer_n1[i][j].size() == 0);
        }
    }

    for (int i = 0; i < CORE_NUM; ++ i) {
        if (hyperx_1_src_id[i] != -1) {
            queue_buffer_n1[i / GROUP_CORE_NUM][i % GROUP_CORE_NUM].push(hyperx_1_src_id[i]);
            queue_buffer2_n1[i / GROUP_CORE_NUM][i % GROUP_CORE_NUM].push(hyperx_1_dst_id[i]);
        }
    }

    //row routing
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        // crossbar schedule

        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (!queue_buffer_empty_n1[i][j]) {
                int dst_id = queue_buffer_n1[i][j].front();
                int another_id = queue_buffer2_n1[i][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;

                row_router_buffer_n1[row_id][j].push(dst_id);
                row_router_buffer2_n1[row_id][j].push(another_id);

                if (dst_id == 0 && another_id == 0) {
                    cout << "catch error 1" << endl;
                    exit(-1);
                }

                queue_buffer_n1[i][j].pop();
                queue_buffer2_n1[i][j].pop();
            }
        }
    }

    //column routing
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (!row_buffer_empty_n1[i][j]) {
                int dst_id = row_router_buffer_n1[i][j].front();
                int another_id = row_router_buffer2_n1[i][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;

                if (row_id != i) {
                    cout << "failed in row routing" << endl;
                    exit(-1);
                }
                column_router_buffer_n1[i][col_id].push(dst_id);
                column_router_buffer2_n1[i][col_id].push(another_id);
                if (dst_id == 0 && another_id == 0) {
                    cout << "catch error 2" << endl;
                    exit(-1);
                }


                row_router_buffer_n1[i][j].pop();
                row_router_buffer2_n1[i][j].pop();
            }
        }
    }

    //out
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (!column_buffer_empty_n1[i][j]) {
                int dst_id = column_router_buffer_n1[i][j].front();
                int another_id = column_router_buffer2_n1[i][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;

                if (row_id != i || col_id != j) {
                    cout << "failed in hyperx out" << endl;
                    exit(-1);
                }
                hyper_out_n1[i][j].push(dst_id);
                hyper_out2_n1[i][j].push(another_id);
                if (dst_id == 0 && another_id == 0) {
                    cout << "catch error 3" << endl;
                    exit(-1);
                }

                column_router_buffer_n1[i][j].pop();
                column_router_buffer2_n1[i][j].pop();
            }
        }
    }

    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (hyper_out_n1[i][j].size() != 0) {
                int dst_id = hyper_out_n1[i][j].front();
                int another_id = hyper_out2_n1[i][j].front();

                //if ((dst_id % CORE_NUM) / GROUP_CORE_NUM != i || dst_id % GROUP_CORE_NUM != j) {
                //    cout << "error " << dst_id << " " << i << " " << j;
                //    return -1;
                //}
                hyperx_1_src_id_out[i * GROUP_CORE_NUM + j] = dst_id;
                hyperx_1_dst_id_out[i * GROUP_CORE_NUM + j] = another_id;
                
                send_edge += 1;

                hyper_out_n1[i][j].pop();
                hyper_out2_n1[i][j].pop();
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
            all_empty &= (queue_buffer_n1[i][j].size() == 0);
            all_empty &= (row_router_buffer_n1[i][j].size() == 0);
            all_empty &= (column_router_buffer_n1[i][j].size() == 0);
            all_empty &= (hyper_out_n1[i][j].size() == 0);
        }
    }
    return all_empty;
}

int hyperx_network_2() {

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

    //row routing
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        // crossbar schedule

        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (!queue_buffer_empty[i][j]) {
                int dst_id = queue_buffer[i][j].front();
                int another_id = queue_buffer2[i][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;

                row_router_buffer[row_id][j].push(dst_id);
                row_router_buffer2[row_id][j].push(another_id);

                if (dst_id == 798169) {
                    fprintf(p3, "%d %d %d %d %d\n", clk, i * GROUP_CORE_NUM + j, dst_id, another_id, another_id % CORE_NUM);
                }

                queue_buffer[i][j].pop();
                queue_buffer2[i][j].pop();
            }
        }
    }

    //column routing
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (!row_buffer_empty[i][j] && !q_flag) {
                int dst_id = row_router_buffer[i][j].front();
                int another_id = row_router_buffer2[i][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;

                if (col_id != j) {
                    cout << "failed in row routing" << endl;
                    exit(-1);
                }
                column_router_buffer[i][col_id].push(dst_id);
                column_router_buffer2[i][col_id].push(another_id);

                row_router_buffer[i][j].pop();
                row_router_buffer2[i][j].pop();
            }
        }
    }

    //out
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (!column_buffer_empty[i][j]) {
                int dst_id = column_router_buffer[i][j].front();
                int another_id = column_router_buffer2[i][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;

                if (row_id != i || col_id != j) {
                    cout << "failed in hyperx out" << endl;
                    exit(-1);
                }
                hyper_out[i][j].push(dst_id);
                hyper_out2[i][j].push(another_id);

                column_router_buffer[i][j].pop();
                column_router_buffer2[i][j].pop();
                column_buffer_empty_time[i][j] = clk;
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