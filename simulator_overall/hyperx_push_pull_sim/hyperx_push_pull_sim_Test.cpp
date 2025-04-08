#include "acc.h"

using namespace std;

// const int v_num = 65535, e_num = 1819290;
const int v_num = 1048576, e_num = 31400738;
const int HY1_FIFO_SIZE = 16;
const int HY1_BLOCK_ROUTING = 1;
const int HY2_FIFO_SIZE = 16;
const int HY2_BLOCK_ROUTING = 1;
const int USING_HY1 = 1;
const int USING_HY2 = 1;

int hyperx_1_src_id[CORE_NUM], hyperx_1_dst_id[CORE_NUM];
int hyperx_1_src_id_out[CORE_NUM], hyperx_1_dst_id_out[CORE_NUM];
int row_buffer_empty_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], column_buffer_empty_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], queue_buffer_empty_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
int column_buffer_empty_time_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
static queue<int> row_router_buffer_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], row_router_buffer2_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
static queue<int> column_router_buffer_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], column_router_buffer2_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
static queue<int> queue_buffer_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], queue_buffer2_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
static queue<int> hyper_out_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], hyper_out2_n1[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];

int hyperx_2_src_id[CORE_NUM], hyperx_2_dst_id[CORE_NUM];
int hyperx_2_src_id_out[CORE_NUM], hyperx_2_dst_id_out[CORE_NUM];
int row_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], column_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], queue_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
int column_buffer_empty_time[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
static queue<int> row_router_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], row_router_buffer2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
static queue<int> column_router_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], column_router_buffer2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
static queue<int> queue_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], queue_buffer2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
static queue<int> hyper_out[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], hyper_out2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];

int crossbar_1_src_id[CORE_NUM], crossbar_1_dst_id[CORE_NUM];
int crossbar_1_src_id_out[CORE_NUM], crossbar_1_dst_id_out[CORE_NUM];
int crossbar_in_buffer_empty_1[CORE_NUM];
static queue<int> crossbar_in_buffer_1[CORE_NUM], crossbar_in_buffer2_1[CORE_NUM];
static queue<int> crossbar_out_buffer_1[CORE_NUM], crossbar_out_buffer2_1[CORE_NUM];

int crossbar_2_src_id[CORE_NUM], crossbar_2_dst_id[CORE_NUM];
int crossbar_2_src_id_out[CORE_NUM], crossbar_2_dst_id_out[CORE_NUM];
int crossbar_in_buffer_empty_2[CORE_NUM];
static queue<int> crossbar_in_buffer_2[CORE_NUM], crossbar_in_buffer2_2[CORE_NUM];
static queue<int> crossbar_out_buffer_2[CORE_NUM], crossbar_out_buffer2_2[CORE_NUM];

static queue<pair<int, int>> scheduler_q[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];

int edge_off[v_num + 1];
int edge_list[e_num];

int send_edge = 0;
int clk;
FILE *p3;
int row_same_id_sum[PSEUDO_CHANNEL_NUM];
int row_same_pe_sum[PSEUDO_CHANNEL_NUM];
int col_same_id_sum[GROUP_CORE_NUM];
int col_same_pe_sum[GROUP_CORE_NUM];
int crossbar_same_id_sum[CORE_NUM];
int crossbar_same_pe_sum[CORE_NUM];
vector<pair<int, int>> degree_vec;

int hyperx_network_1();
int hyperx_network_2();
int crossbar_network_1();
int crossbar_network_2();

void degree_check() {
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

    p1 = fopen("../../simulator/data/edge_off.txt","r");
    p2 = fopen("../../simulator/data/edge_list.txt","r");
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

    for (int i = 0; i < GROUP_CORE_NUM; i++) {
        row_same_id_sum[i] = 0;
        row_same_pe_sum[i] = 0;
    }
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        col_same_id_sum[i] = 0;
        col_same_pe_sum[i] = 0;
    }
    for (int i = 0; i < CORE_NUM; i++) {
        crossbar_same_id_sum[i] = 0;
        crossbar_same_pe_sum[i] = 0;
    }

    degree_check();
    return 0;

    int edge = 0;
    vector<pair<int, int>> tasks[PSEUDO_CHANNEL_NUM];
    for (int i = 0; i < v_num; i++) {
        // int v_id = degree_vec[i].second;
        int v_id = i;
        int channel_id = (v_id % CORE_NUM) / GROUP_CORE_NUM;

        for (int off = edge_off[v_id]; off < edge_off[v_id + 1]; off++) {
            tasks[channel_id].push_back(pair<int, int>(v_id, edge_list[off]));
            edge++;
        }
    }
    int tasks_index[PSEUDO_CHANNEL_NUM];
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        tasks_index[i] = 0;
    }

    cout << "read edge " << edge << endl;

    for (int i = 0; i < CORE_NUM; i++) {
        hyperx_2_src_id[i] = -1;
        hyperx_2_dst_id[i] = -1;
        hyperx_1_src_id[i] = -1;
        hyperx_1_dst_id[i] = -1;
        crossbar_1_src_id[i] = -1;
        crossbar_1_dst_id[i] = -1;
        crossbar_2_src_id[i] = -1;
        crossbar_2_dst_id[i] = -1;
    }
    int send_task = 0;
    int n1_empty_clk = 0;
    int n2_empty_clk = 0;
    for (clk = 1; clk < 10000000; clk++) {
        int n1_empty;
        if (USING_HY1) n1_empty = hyperx_network_1();
        else n1_empty = crossbar_network_1();
        int n2_empty;
        if (USING_HY2) n2_empty = hyperx_network_2();
        else n2_empty = crossbar_network_2();
        int send = 0;
        for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
            int tmp_scheduler_full = false;
            for (int j = 0; j < GROUP_CORE_NUM; j++) {
                if (scheduler_q[i][j].size() > HY1_FIFO_SIZE) {
                    tmp_scheduler_full = true;
                    break;
                }
            }
            for (int j = 0; j < GROUP_CORE_NUM; j++) {
                int core_id = i * GROUP_CORE_NUM + j;
                int tmp_idx = i;
                if (tasks_index[tmp_idx] < tasks[tmp_idx].size() && !tmp_scheduler_full) {
                    scheduler_q[i][j].push(tasks[tmp_idx][tasks_index[tmp_idx]]);
                    tasks_index[tmp_idx] += 1;
                }
                if (scheduler_q[i][j].size() > 0 && queue_buffer_n1[i][j].size() < HY1_FIFO_SIZE) {
                    if (USING_HY1) {
                        hyperx_1_src_id[core_id] = scheduler_q[i][j].front().second;
                        hyperx_1_dst_id[core_id] = scheduler_q[i][j].front().first;
                    } else {
                        crossbar_1_src_id[core_id] = scheduler_q[i][j].front().second;
                        crossbar_1_dst_id[core_id] = scheduler_q[i][j].front().first;
                    }
                    scheduler_q[i][j].pop();
                    send = 1;
                    send_task += 1;
                } else {
                    hyperx_1_src_id[core_id] = -1;
                    hyperx_1_dst_id[core_id] = -1;
                    crossbar_1_src_id[core_id] = -1;
                    crossbar_1_dst_id[core_id] = -1;
                }
                if (USING_HY2) {
                    if (USING_HY1) {
                        hyperx_2_src_id[core_id] = hyperx_1_src_id_out[core_id];
                        hyperx_2_dst_id[core_id] = hyperx_1_dst_id_out[core_id];
                    } else {
                        hyperx_2_src_id[core_id] = crossbar_1_src_id_out[core_id];
                        hyperx_2_dst_id[core_id] = crossbar_1_dst_id_out[core_id];
                    }
                } else {
                    if (USING_HY1) {
                        crossbar_2_src_id[core_id] = hyperx_1_src_id_out[core_id];
                        crossbar_2_dst_id[core_id] = hyperx_1_dst_id_out[core_id];
                    } else {
                        crossbar_2_src_id[core_id] = crossbar_1_src_id_out[core_id];
                        crossbar_2_dst_id[core_id] = crossbar_1_dst_id_out[core_id];
                    }
                }
                if (USING_HY1 && hyperx_1_src_id_out[core_id] != -1 && hyperx_1_src_id_out[core_id] % CORE_NUM != core_id) {
                    cout << "hyperx 1 routing error" << endl;
                    cout << hyperx_1_src_id_out[core_id] << " " << hyperx_1_dst_id_out[core_id] << " " << core_id << endl;
                    exit(-1);
                }
                if (USING_HY2 && hyperx_2_dst_id_out[core_id] != -1 && hyperx_2_dst_id_out[core_id] % CORE_NUM != core_id) {
                    cout << "hyperx 2 routing error" << endl;
                    exit(-1);
                }
                if (!USING_HY1 && crossbar_1_src_id_out[core_id] != -1 && crossbar_1_src_id_out[core_id] % CORE_NUM != core_id) {
                    cout << "crossbar 1 routing error" << endl;
                    cout << crossbar_1_src_id_out[core_id] << " " << crossbar_1_dst_id_out[core_id] << " " << core_id << endl;
                    exit(-1);
                }
                if (!USING_HY2 && crossbar_2_dst_id_out[core_id] != -1 && crossbar_2_dst_id_out[core_id] % CORE_NUM != core_id) {
                    cout << "crossbar 2 routing error" << endl;
                    exit(-1);
                }
            }
        }
        if (clk % 100000 == 0) {
            cout << "clk " << clk << " send task " << send_task << " do task " << send_edge << " n1_empty " << n1_empty << " n2_empty " << n2_empty << endl;
        }
        if (!n1_empty) {
            n1_empty_clk = clk;
        }
        if (!n2_empty) {
            n2_empty_clk = clk;
        }
        if (send == 0 && n1_empty && n2_empty && send_edge == edge) {
            cout << "n1 routing over at clk " << n1_empty_clk << endl;
            cout << "n2 routing over at clk " << n2_empty_clk << endl;
            cout << "routing over at clk " << clk << endl;
            break;
        }
    }
    if (USING_HY2) {
        cout << "print row_same_id_sum" << endl;
        for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
            cout << row_same_id_sum[i] << endl;
        }
        cout << "print row_same_pe_sum" << endl;
        for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
            cout << row_same_pe_sum[i] << endl;
        }
        cout << "print col_same_id_sum" << endl;
        for (int i = 0; i < GROUP_CORE_NUM; i++) {
            cout << col_same_id_sum[i] << endl;
        }
        cout << "print col_same_pe_sum" << endl;
        for (int i = 0; i < GROUP_CORE_NUM; i++) {
            cout << col_same_pe_sum[i] << endl;
        }
    } else {
        cout << "print crossbar_same_id_sum" << endl;
        for (int i = 0; i < CORE_NUM; i++) {
            cout << crossbar_same_id_sum[i] << endl;
        }
        cout << "print crossbar_same_pe_sum" << endl;
        for (int i = 0; i < CORE_NUM; i++) {
            cout << crossbar_same_pe_sum[i] << endl;
        }
    }
    return 0;
}

int hyperx_network_1() {

    if (!USING_HY1) return 1;

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
    for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
        // crossbar schedule
        bool row_send[PSEUDO_CHANNEL_NUM];
        for (int l = 0; l < PSEUDO_CHANNEL_NUM; l++) {
            row_send[l] = 0;
        }
        for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
            if (!queue_buffer_empty_n1[i][j]) {
                int dst_id = queue_buffer_n1[i][j].front();
                int another_id = queue_buffer2_n1[i][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;

                if (row_send[row_id]) continue;

                if (row_router_buffer_n1[row_id][j].size() < HY1_FIFO_SIZE) {
                    row_router_buffer_n1[row_id][j].push(dst_id);
                    row_router_buffer2_n1[row_id][j].push(another_id);

                    if (HY1_BLOCK_ROUTING) row_send[row_id] = true;

                    queue_buffer_n1[i][j].pop();
                    queue_buffer2_n1[i][j].pop();
                }
            }
        }
    }

    //column routing
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        bool col_send[GROUP_CORE_NUM];
        for (int l = 0; l < GROUP_CORE_NUM; l++) {
            col_send[l] = 0;
        }
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (!row_buffer_empty_n1[i][j]) {
                int dst_id = row_router_buffer_n1[i][j].front();
                int another_id = row_router_buffer2_n1[i][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;

                if (col_send[col_id]) continue;

                if (row_id != i) {
                    cout << "hyperx1: failed in row routing" << endl;
                    exit(-1);
                }
                if (column_router_buffer_n1[i][col_id].size() < HY1_FIFO_SIZE) {
                    column_router_buffer_n1[i][col_id].push(dst_id);
                    column_router_buffer2_n1[i][col_id].push(another_id);

                    if (HY1_BLOCK_ROUTING) col_send[col_id] = true;

                    row_router_buffer_n1[i][j].pop();
                    row_router_buffer2_n1[i][j].pop();
                }
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
                    cout << "hyperx1: failed in hyperx out" << endl;
                    exit(-1);
                }

                if (hyper_out_n1[i][j].size() < HY1_FIFO_SIZE) {
                    hyper_out_n1[i][j].push(dst_id);
                    hyper_out2_n1[i][j].push(another_id);

                    column_router_buffer_n1[i][j].pop();
                    column_router_buffer2_n1[i][j].pop();
                }
            }
        }
    }

    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (hyper_out_n1[i][j].size() != 0 && queue_buffer[i][j].size() < HY2_FIFO_SIZE) {
            // if (hyper_out_n1[i][j].size() != 0) {
                int dst_id = hyper_out_n1[i][j].front();
                int another_id = hyper_out2_n1[i][j].front();

                //if ((dst_id % CORE_NUM) / GROUP_CORE_NUM != i || dst_id % GROUP_CORE_NUM != j) {
                //    cout << "error " << dst_id << " " << i << " " << j;
                //    return -1;
                //}
                hyperx_1_src_id_out[i * GROUP_CORE_NUM + j] = dst_id;
                hyperx_1_dst_id_out[i * GROUP_CORE_NUM + j] = another_id;

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

    if (!USING_HY2) return 1;

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
    for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
        // crossbar schedule
        int max_same = 0;
        int max_pe_same = 0;
        bool row_send[PSEUDO_CHANNEL_NUM];
        for (int l = 0; l < PSEUDO_CHANNEL_NUM; l++) {
            row_send[l] = 0;
        }
        for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
            if (!queue_buffer_empty[i][j]) {
                int dst_id = queue_buffer[i][j].front();
                int another_id = queue_buffer2[i][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;

                if (row_send[row_id]) continue;

                int cu_same = 0;
                int cu_pe_same = 0;
                for (int k = 0; k < PSEUDO_CHANNEL_NUM; ++k) {
                    if (!queue_buffer_empty[k][j] && queue_buffer[k][j].front() == queue_buffer[i][j].front()) {
                        cu_same += 1;
                    }
                    if (!queue_buffer_empty[k][j] && ((queue_buffer[k][j].front() % CORE_NUM) / GROUP_CORE_NUM) == ((queue_buffer[i][j].front() % CORE_NUM) / GROUP_CORE_NUM)) {
                        cu_pe_same += 1;
                    }
                }
                max_same = max_same > cu_same ? max_same : cu_same;
                max_pe_same = max_pe_same > cu_pe_same ? max_pe_same : cu_pe_same;

                if (row_router_buffer[row_id][j].size() < HY2_FIFO_SIZE) {
                    row_router_buffer[row_id][j].push(dst_id);
                    row_router_buffer2[row_id][j].push(another_id);

                    if (HY2_BLOCK_ROUTING) row_send[row_id] = true;

                    queue_buffer[i][j].pop();
                    queue_buffer2[i][j].pop();
                }
            }
        }
        if (max_same > 0) {
            row_same_id_sum[max_same - 1] += 1;
        }
        if (max_pe_same > 0) {
            row_same_pe_sum[max_pe_same - 1] += 1;
        }
    }

    //column routing
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++i) {
        int max_same = 0;
        int max_pe_same = 0;
        bool col_send[GROUP_CORE_NUM];
        for (int l = 0; l < GROUP_CORE_NUM; l++) {
            col_send[l] = 0;
        }
        for (int j = 0; j < GROUP_CORE_NUM; ++j) {
            if (!row_buffer_empty[i][j]) {
                int dst_id = row_router_buffer[i][j].front();
                int another_id = row_router_buffer2[i][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;

                if (col_send[col_id]) continue;

                int cu_same = 0;
                int cu_pe_same = 0;
                for (int k = 0; k < GROUP_CORE_NUM; ++k) {
                    if (!row_buffer_empty[i][k] && row_router_buffer[i][k].front() == row_router_buffer[i][j].front()) {
                        cu_same += 1;
                    }
                    if (!row_buffer_empty[i][k] && (row_router_buffer[i][k].front() % GROUP_CORE_NUM) == (row_router_buffer[i][j].front() % GROUP_CORE_NUM)) {
                        cu_pe_same += 1;
                    }
                }
                max_same = max_same > cu_same ? max_same : cu_same;
                max_pe_same = max_pe_same > cu_pe_same ? max_pe_same : cu_pe_same;

                if (row_id != i) {
                    cout << "hyperx2: failed in row routing" << endl;
                    exit(-1);
                }
                if (column_router_buffer[i][col_id].size() < HY2_FIFO_SIZE) {
                    column_router_buffer[i][col_id].push(dst_id);
                    column_router_buffer2[i][col_id].push(another_id);

                    if (HY2_BLOCK_ROUTING) col_send[col_id] = true;

                    fprintf(p3, "clk %d (%d, %d) pop %d %d\n", clk, i, j, another_id, dst_id);

                    row_router_buffer[i][j].pop();
                    row_router_buffer2[i][j].pop();
                }
            }
        }
        if (max_same > 0) {
            col_same_id_sum[max_same - 1] += 1;
        }
        if (max_pe_same > 0) {
            col_same_pe_sum[max_pe_same - 1] += 1;
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
                    cout << "hyperx2: failed in hyperx out" << endl;
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

                hyperx_2_src_id_out[i * GROUP_CORE_NUM + j] = another_id;
                hyperx_2_dst_id_out[i * GROUP_CORE_NUM + j] = dst_id;
                
                send_edge += 1;

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

int crossbar_network_1() {

    if (USING_HY1) return 1;

    for (int i = 0; i < CORE_NUM; ++ i) {
        crossbar_in_buffer_empty_1[i] = (crossbar_in_buffer_1[i].size() == 0);
        if (crossbar_1_src_id[i] != -1) {
            crossbar_in_buffer_1[i].push(crossbar_1_src_id[i]);
            crossbar_in_buffer2_1[i].push(crossbar_1_dst_id[i]);
        }
    }

    //routing
    bool col_send[CORE_NUM];
    for (int l = 0; l < CORE_NUM; l++) {
        col_send[l] = 0;
    }
    for (int i = 0; i < CORE_NUM; ++ i) {
        if (!crossbar_in_buffer_empty_1[i]) {
            int dst_id = crossbar_in_buffer_1[i].front();
            int another_id = crossbar_in_buffer2_1[i].front();
            int pe_id = dst_id % CORE_NUM;

            if (col_send[pe_id]) continue;

            if (crossbar_out_buffer_1[pe_id].size() < HY1_FIFO_SIZE) {
                crossbar_out_buffer_1[pe_id].push(dst_id);
                crossbar_out_buffer2_1[pe_id].push(another_id);

                if (HY1_BLOCK_ROUTING) col_send[pe_id] = true;

                crossbar_in_buffer_1[i].pop();
                crossbar_in_buffer2_1[i].pop();
            }
        }
    }

    //out
    for (int i = 0; i < CORE_NUM; ++ i) {
        if (crossbar_out_buffer_1[i].size() != 0) {
            int dst_id = crossbar_out_buffer_1[i].front();
            int another_id = crossbar_out_buffer2_1[i].front();

            crossbar_1_src_id_out[i] = dst_id;
            crossbar_1_dst_id_out[i] = another_id;

            crossbar_out_buffer_1[i].pop();
            crossbar_out_buffer2_1[i].pop();
        }
        else {
            crossbar_1_src_id_out[i] = -1;
            crossbar_1_dst_id_out[i] = -1;
        }
    }

    int all_empty = 1;
    for (int i = 0; i < CORE_NUM; ++ i) {
            all_empty &= (crossbar_in_buffer_1[i].size() == 0);
            all_empty &= (crossbar_out_buffer_1[i].size() == 0);
    }
    return all_empty;
}

int crossbar_network_2() {

    if (USING_HY2) return 1;

    for (int i = 0; i < CORE_NUM; ++ i) {
        crossbar_in_buffer_empty_2[i] = (crossbar_in_buffer_2[i].size() == 0);
    }

    for (int i = 0; i < CORE_NUM; ++ i) {
        if (crossbar_2_src_id[i] != -1) {
            crossbar_in_buffer_2[i].push(crossbar_2_dst_id[i]);
            crossbar_in_buffer2_2[i].push(crossbar_2_src_id[i]);
            // crossbar_in_buffer_2[crossbar_2_src_id[i] % CORE_NUM].push(crossbar_2_dst_id[i]);
            // crossbar_in_buffer2_2[crossbar_2_src_id[i] % CORE_NUM].push(crossbar_2_src_id[i]);
        }
    }

    //routing
    int max_same = 0;
    int max_pe_same = 0;
    bool col_send[CORE_NUM];
    for (int l = 0; l < CORE_NUM; l++) {
        col_send[l] = 0;
    }
    for (int i = 0; i < CORE_NUM; ++i) {
        if (!crossbar_in_buffer_empty_2[i]) {
            int dst_id = crossbar_in_buffer_2[i].front();
            int another_id = crossbar_in_buffer2_2[i].front();
            int pe_id = dst_id % CORE_NUM;

            if (col_send[pe_id]) continue;

            int cu_same = 0;
            int cu_pe_same = 0;
            for (int k = 0; k < CORE_NUM; ++k) {
                if (!crossbar_in_buffer_empty_2[k] && crossbar_in_buffer_2[k].front() == crossbar_in_buffer_2[i].front()) {
                    cu_same += 1;
                }
                if (!crossbar_in_buffer_empty_2[k] && (crossbar_in_buffer_2[k].front() % CORE_NUM) == (crossbar_in_buffer_2[i].front() % CORE_NUM)) {
                    cu_pe_same += 1;
                }
            }
            max_same = max_same > cu_same ? max_same : cu_same;
            max_pe_same = max_pe_same > cu_pe_same ? max_pe_same : cu_pe_same;

            if (crossbar_out_buffer_2[pe_id].size() < HY2_FIFO_SIZE) {
                crossbar_out_buffer_2[pe_id].push(dst_id);
                crossbar_out_buffer2_2[pe_id].push(another_id);

                if (HY2_BLOCK_ROUTING) col_send[pe_id] = true;

                crossbar_in_buffer_2[i].pop();
                crossbar_in_buffer2_2[i].pop();
            }
        }
   }
    if (max_same > 0) {
        crossbar_same_id_sum[max_same - 1] += 1;
    }
    if (max_pe_same > 0) {
        crossbar_same_pe_sum[max_pe_same - 1] += 1;
    }

    //out
    for (int i = 0; i < CORE_NUM; ++ i) {
        if (crossbar_out_buffer_2[i].size() != 0) {
            int dst_id = crossbar_out_buffer_2[i].front();
            int another_id = crossbar_out_buffer2_2[i].front();

            crossbar_2_src_id_out[i] = another_id;
            crossbar_2_dst_id_out[i] = dst_id;
                
            send_edge += 1;

            crossbar_out_buffer_2[i].pop();
            crossbar_out_buffer2_2[i].pop();
        }
        else {
            crossbar_2_src_id_out[i] = -1;
            crossbar_2_dst_id_out[i] = -1;
        }
    }

    int all_empty = 1;
    for (int i = 0; i < CORE_NUM; ++ i) {
        all_empty &= (crossbar_in_buffer_2[i].size() == 0);
        all_empty &= (crossbar_out_buffer_2[i].size() == 0);
    }
    return all_empty;
}