#include "acc.h"
#include <map>

using namespace std;

queue<int> active_v_id_buffer[CORE_NUM],
    active_v_l_offset_buffer[CORE_NUM], active_v_r_offset_buffer[CORE_NUM];

bool vis_bitmap[2][MAX_VERTEX_NUM];
int loffset_bram[MAX_VERTEX_NUM], roffset_bram[MAX_VERTEX_NUM];
pair<int, int> edge_mem[MAX_EDGE_NUM];
float delta_ram[2][MAX_VERTEX_NUM];
float pr_ram[MAX_VERTEX_NUM];

// 模拟原子更新操作
struct Update {
    int _src_id;
    int _dst_id;
    int _cycle;
    float _value;
    bool _valid;
    Update(int src, int dst, int c, float v) :
        _src_id(src), _dst_id(dst), _cycle(c), _value(v) {
            _valid = true;
        }
    Update() {
        _src_id = -1;
        _dst_id = -1;
        _cycle = 0;
        _value = 0;
        _valid = false;
    }
};

struct Update_Q {
    static const int _Q_NUM = 1;
    queue<Update> _q[_Q_NUM];
    Update _q_front_in[_Q_NUM];
    size_t _q_size = 0;
    size_t _max_size = 0;
    bool _flush_flag = 0;
    Update_Q() : _q_size(0) {
        for (int i = 0; i < _Q_NUM; i++) {
            _q_front_in[i] = Update();
        }
        _flush_flag = 0;
    }
    size_t size() {
        return _q_size;
    }
    bool empty() {
        return (_q_size == 0);
    }
    void show() {
        for (int i = 0; i < _Q_NUM; i++) {
            cout << i << ": ";
            int num = _q[i].size();
            for (int j = 0; j < num; j++) {
                cout << _q[i].front()._dst_id << " ";
                _q[i].push(_q[i].front());
                _q[i].pop();
            }
            cout << endl;
        }
    }
    void flush() {
        for (int i = 0; i < _Q_NUM; i++) {
            if (_q_front_in[i]._valid) {
                _q[i].push(_q_front_in[i]);
                _q_front_in[i]._valid = 0;
                _q_size++;
            }
        }
        _flush_flag = 1;
    }
    bool push(Update u) {

        DEBUG_MSG(!u._valid || u._dst_id == -1, "Update_Q push invalid data");

        int id = (u._dst_id / CORE_NUM) % _Q_NUM;

        if (_q_front_in[id]._valid == 0) {
            _q_front_in[id] = u;
        } else if (u._dst_id == _q_front_in[id]._dst_id) {
            _q_front_in[id]._value += u._value;
            // _q[id].push(_q_front_in[id]);
            // _q_front_in[id] = u;
            // _q_size += 1;
        } else {
            _q[id].push(_q_front_in[id]);
            _q_front_in[id] = u;
            _q_size += 1;
        }
        // if (_q_size == FIFO_SIZE) {
            // show();
            // getchar();
        // }
        _max_size = _q_size > _max_size ? _q_size : _max_size;
        return true;
    }
    bool pop() {

        DEBUG_MSG(_q_size == 0, "Update_Q pop when it's empty");

        int max_size = _q[0].size();
        int max_size_id = 0;

        for (int i = 1; i < _Q_NUM; i++) {
            if (_q[i].size() > max_size) {
                max_size = _q[i].size();
                max_size_id = i;
            }
        }
        if (max_size == 0) {
            cout << "update q is empty" << endl;
            exit(-1);
        } else {
            _q[max_size_id].pop();
        }

        _q_size--;
        return true;
    }
    Update front() {
       DEBUG_MSG(_q_size == 0, "Update_Q front when it's empty");

        int max_size = _q[0].size();
        int max_size_id = 0;

        for (int i = 1; i < _Q_NUM; i++) {
            if (_q[i].size() > max_size) {
                max_size = _q[i].size();
                max_size_id = i;
            }
        }
        if (max_size == 0) {
            for (int i = 0; i < _Q_NUM; i++) {
                if (_q_front_in[i]._valid) {
                    return _q_front_in[i];
                }
            }
        }
        return _q[max_size_id].front();
    }
};

Update_Q update_q[CORE_NUM];
Update prev_q[CORE_NUM][DELAY_CYCLE];
Update acc_reg[CORE_NUM][ACC_REG_NUM];

int active_v_id[CORE_NUM], active_edge[CORE_NUM], edge_valid[CORE_NUM];
int bitmap_line_id[CORE_NUM];

vector<int> ngh[MAX_VERTEX_NUM];

int hyperx_2_src_id[CORE_NUM], hyperx_2_dst_id[CORE_NUM];

int hyperx_2_src_id_out[CORE_NUM], hyperx_2_dst_id_out[CORE_NUM];

pair<int, int> hyperx_network_2();
pair<int, int> hyperx_network_2_voq();
pair<int, int> hyperx_network_2_dp();

int clk;

int main()
{
    int v_num, e_num;
    int rst, push_flag, iteration_id = 1;
    int visited_edge_num = 0;

    for (int i = 0; i < CORE_NUM; i++) {
        for (int j = 0; j < DELAY_CYCLE; j++) {
            prev_q[i][j] = Update(-1, -1, 0, 0);
        }
        for (int j = 0; j < ACC_REG_NUM; j++) {
            acc_reg[i][j] = Update(-1, -1, 0, 0);
        }
    }

    //initialization
    FILE *p1;
    int src, dst;

    p1 = fopen("../../simulator/data/test_graph20_degree_reorder.txt","r");
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

    const float damping = 0.85;
    const float one_over_n = 1.0 / v_num;
    const float e1 = 1e-7;
    const float e2 = 1e-2;
    const float addedConst = (1 - damping) * one_over_n;

    //original bfs
    int now_offset = 0;
    for (int i = 0; i < v_num; ++ i) {
        loffset_bram[i] = now_offset;
        roffset_bram[i] = now_offset + ngh[i].size();

        vector<int>::iterator it;
        for (it = ngh[i].begin(); it != ngh[i].end(); ++ it) {
            edge_mem[now_offset] = make_pair(i, *it);
            now_offset ++;
        }
        delta_ram[0][i] = one_over_n;
        delta_ram[1][i] = addedConst - one_over_n;
        pr_ram[i] = addedConst;
        vis_bitmap[0][i] = true;
        vis_bitmap[1][i] = true;
    }

    p1 = fopen("debug_res.txt", "w");

    int hyperx2_empty_clk = 0;
    int update_q_empty_clk[CORE_NUM];
    int wait_time[CORE_NUM];
    int wait_ct[DELAY_CYCLE];
    int pe_empty_time[CORE_NUM];
    int pe_load[CORE_NUM];
    map<int, int> v_delay_ct[CORE_NUM];
    int test_delay = 0;
    for (int i = 0; i < DELAY_CYCLE; i++) {
        wait_ct[i] = 0;
    }
    for (int i = 0; i < CORE_NUM; i++) {
        wait_time[i] = 0;
        pe_empty_time[i] = 0;
        pe_load[i] = 0;
    }
    for (clk = 0; clk < MAX_CLK && iteration_id <= 14; clk ++) {
        if (clk < 10) {
            rst = 1;
            push_flag = 1;
            iteration_id = 1;
            for (int i = 0; i < CORE_NUM; ++ i) bitmap_line_id[i] = 0;
        }
        else {
            rst = 0;
        }

        if (!rst) {

            if (iteration_id <= 5) {
                push_flag = 1;
            }
            else {
                // pull轮次依然采用push执行
                push_flag = 1;
            }

            if (push_flag) {
                //schedule active vertex
                for (int i = 0; i < CORE_NUM; ++ i) {
                    int now_bitmap_line_id, max_bitmap_line_id = (bitmap_line_id[i] / 32 + 1) * 32;
                    for (now_bitmap_line_id = bitmap_line_id[i]; now_bitmap_line_id < max_bitmap_line_id; ++now_bitmap_line_id){
                        int v_id = now_bitmap_line_id * CORE_NUM + i;
                        if (v_id < v_num) {
                            if (vis_bitmap[(iteration_id - 1) % 2][v_id]) {
                                active_v_id_buffer[i].push(v_id);
                                active_v_l_offset_buffer[i].push(loffset_bram[v_id]);
                                active_v_r_offset_buffer[i].push(roffset_bram[v_id]);

                                bitmap_line_id[i] = now_bitmap_line_id + 1;
                                break;
                            }
                            bitmap_line_id[i] = now_bitmap_line_id + 1;
                        }
                    }
                }
            } else {
                // schedule all vertex
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
                            // vis_bitmap[iteration_id % 2][active_v_id_buffer[front_core_id].front()] = 0;
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
                    // fprintf(channel_trace[i], "0x%08X READ %d\n", rd_edge_addr * 4, clk * 10);

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

                    // visited_edge_num += 16;
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
                    int src_id = active_v_id[i];
                    int dst_id = active_edge[i];

                    hyperx_2_src_id[i] = src_id;
                    hyperx_2_dst_id[i] = dst_id;
                } else {
                    int src_id = active_edge[i];
                    int dst_id = active_v_id[i];

                    hyperx_2_src_id[i] = src_id;
                    hyperx_2_dst_id[i] = dst_id;
                }
                if (active_v_id[i] != -1) visited_edge_num++;
            }
            pair<int, int> hyperx_rq_num = hyperx_network_2_dp();
            if (clk % 500000 == 0) cout << clk << " hyperx in " << hyperx_rq_num.first << " hyperx out " << hyperx_rq_num.second << " q size " << update_q[0].size() << endl;
            if (hyperx_rq_num.first - hyperx_rq_num.second > 0) {
                hyperx2_empty_clk = clk;
            }

            for (int i = 0; i < CORE_NUM; ++ i) {
                if (!update_q[i].empty() && !update_q[i]._flush_flag) {
                    update_q_empty_clk[i] = clk;
                }
                if (hyperx_rq_num.first == hyperx_rq_num.second && hyperx_rq_num.first > 0 && update_q[i].empty()) {
                    update_q[i].flush();
                }
                int src_id = hyperx_2_src_id_out[i];
                int dst_id = hyperx_2_dst_id_out[i];
                if (dst_id != -1) {
                    update_q[i].push(Update(src_id, dst_id, 0, delta_ram[(iteration_id - 1) % 2][src_id] /
                        (roffset_bram[src_id] - loffset_bram[src_id]) * damping));
                }
                if (prev_q[i][DELAY_CYCLE - 1]._valid) {
                    src_id = prev_q[i][DELAY_CYCLE - 1]._src_id;
                    dst_id = prev_q[i][DELAY_CYCLE - 1]._dst_id;
                    delta_ram[iteration_id % 2][dst_id] += prev_q[i][DELAY_CYCLE - 1]._value;
                    // delta_ram[iteration_id % 2][dst_id] +=
                        // delta_ram[(iteration_id - 1) % 2][src_id] /
                        // (roffset_bram[src_id] - loffset_bram[src_id]) * damping;
                    pr_ram[dst_id] +=  prev_q[i][DELAY_CYCLE - 1]._value;
                    // pr_ram[dst_id] += 
                        // delta_ram[(iteration_id - 1) % 2][src_id] /
                        // (roffset_bram[src_id] - loffset_bram[src_id]) * damping;
                    if (fabs(delta_ram[iteration_id % 2][dst_id]) > e2 *
                            (pr_ram[dst_id] - (iteration_id == 1 ? 0 :
                            delta_ram[iteration_id % 2][dst_id]))) {
                        vis_bitmap[iteration_id % 2][dst_id] = true;
                    } else {
                        vis_bitmap[iteration_id % 2][dst_id] = false;
                    }
                }
                for (int k = DELAY_CYCLE - 1; k > 0; k--) {
                    prev_q[i][k] = prev_q[i][k - 1];
                }
                bool flag = true;
                for (int k = 0; k < DELAY_CYCLE && !update_q[i].empty(); k++) {
                    if (update_q[i].front()._dst_id == prev_q[i][k]._dst_id) {
                        flag = false;
                        break;
                    }
                }
                if (!update_q[i].empty() && flag) {
                    prev_q[i][0] = update_q[i].front();
                    if (wait_time[i] > DELAY_CYCLE) {
                        cout << "wait time = " << wait_time[i] << endl;
                        exit(-1);
                    }
                    if (i == 25) {
                        wait_ct[wait_time[i]] += 1;
                        test_delay += wait_time[i] + 1;
                    }
                    /*
                    if (v_delay_ct[i].find(update_q[i].front()._dst_id) == v_delay_ct[i].end()) {
                        v_delay_ct[i][update_q[i].front()._dst_id] = wait_time[i] + 1;
                    } else {
                        v_delay_ct[i][update_q[i].front()._dst_id] += wait_time[i] + 1;
                    }
                    */
                    wait_time[i] = 0;
                    update_q[i].pop();
                } else {
                    prev_q[i][0] = Update(-1, -1, 0, 0);
                    if (!update_q[i].empty()) {
                        wait_time[i] += 1;
                    } else {
                        pe_empty_time[i] += 1;
                    }
                }
            }
            int iteration_end = (hyperx_rq_num.first == hyperx_rq_num.second);
            for (int i = 0; i < CORE_NUM; ++ i) {
                iteration_end &= (bitmap_line_id[i] * CORE_NUM + i >= v_num);
                iteration_end &= update_q[i].empty();
            }
            /*
            for (int i = 0; i < CORE_NUM; ++ i) {
                int src_id = hyperx_2_src_id_out[i];
                int dst_id = hyperx_2_dst_id_out[i];
                if (dst_id != -1) {
                    update_q[i].push(Update(src_id, dst_id, 0));
                } else if (iteration_end) {
                    for (int j = 0; j < ACC_REG_NUM; j++) {
                        if (acc_reg[i][j].dst_id != -1) {
                            delta_ram[iteration_id % 2][acc_reg[i][j].dst_id] += acc_reg[i][j].value;
                            pr_ram[acc_reg[i][j].dst_id] += acc_reg[i][j].value;
                            if (fabs(delta_ram[iteration_id % 2][acc_reg[i][j].dst_id]) > e2 *
                                    (pr_ram[acc_reg[i][j].dst_id] - (iteration_id == 1 ? 0 :
                                    delta_ram[iteration_id % 2][acc_reg[i][j].dst_id]))) {
                                vis_bitmap[iteration_id % 2][acc_reg[i][j].dst_id] = true;
                            } else {
                                vis_bitmap[iteration_id % 2][acc_reg[i][j].dst_id] = false;
                            }
                        }
                    }
                }
                if (!update_q[i].empty()) {
                    int new_src_id = update_q[i].front().src_id;
                    int new_dst_id = update_q[i].front().dst_id;
                    int acc_reg_id = (new_dst_id / CORE_NUM) % ACC_REG_NUM;
                    if (acc_reg[i][acc_reg_id].dst_id != -1) {
                        delta_ram[iteration_id % 2][acc_reg[i][acc_reg_id].dst_id] += acc_reg[i][acc_reg_id].value;
                        pr_ram[acc_reg[i][acc_reg_id].dst_id] += acc_reg[i][acc_reg_id].value;
                        if (fabs(delta_ram[iteration_id % 2][acc_reg[i][acc_reg_id].dst_id]) > e2 *
                                (pr_ram[acc_reg[i][acc_reg_id].dst_id] - (iteration_id == 1 ? 0 :
                                delta_ram[iteration_id % 2][acc_reg[i][acc_reg_id].dst_id]))) {
                            vis_bitmap[iteration_id % 2][acc_reg[i][acc_reg_id].dst_id] = true;
                        } else {
                            vis_bitmap[iteration_id % 2][acc_reg[i][acc_reg_id].dst_id] = false;
                        }
                    }
                    acc_reg[i][acc_reg_id] = Update(new_src_id, new_dst_id, 0);
                    acc_reg[i][acc_reg_id].value = delta_ram[(iteration_id - 1) % 2][new_src_id] /
                        (roffset_bram[new_src_id] - loffset_bram[new_src_id]) * damping;
                    update_q[i].pop();
                } else {
                    prev_q[i][0] = Update(-1, -1, 0);
                }
            }
            */

            if (iteration_end) {
                int updated_vertex_num = 0;
                for (int i = 0; i < v_num; ++ i) {
                    vis_bitmap[(iteration_id - 1) % 2][i] = false;
                    delta_ram[(iteration_id - 1) % 2][i] = 0;
                    if (vis_bitmap[iteration_id % 2][i])
                        updated_vertex_num ++;
                }
                for (int i = 0; i < DELAY_CYCLE; i++) {
                    cout << i << " " << wait_ct[i] << endl;
                }
                cout << "update q empty clk " << clk << endl;
                for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
                    for (int j = 0; j < GROUP_CORE_NUM; j++) {
                        cout << update_q_empty_clk[i * GROUP_CORE_NUM + j] << " ";
                    }
                    cout << endl;
                }
                cout << "hyperx empty at clk " << hyperx2_empty_clk << endl;
                cout << "iteration " << iteration_id << " end at clk " << clk << " active vertice " << updated_vertex_num << " visited edges " <<  visited_edge_num << endl;
                cout << "max update q size " << endl;
                for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
                    for (int j = 0; j < GROUP_CORE_NUM; j++) {
                        cout << update_q[i * GROUP_CORE_NUM + j]._max_size << " ";
                    }
                    cout << endl;
                }
                visited_edge_num = 0;
                iteration_id ++;

                for (int i = 0; i < CORE_NUM; ++ i) bitmap_line_id[i] = 0;
            }

            if (iteration_end && iteration_id > 14) {
                cout << e_num << " " << visited_edge_num << " " << 1.0 * e_num * 14 / (2 * 5 * clk) << "GTEPS" << endl;
                break;
            }
        }
    }
    for (int i = 0; i < v_num; i++) {
        // fprintf(p1, "%d %.9f\n", i, pr_ram[i]);
    }
    fclose(p1);
}

pair<int, int> hyperx_network_2_voq () {
    static queue<pair<int, int>> queue_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static queue<pair<int, int>> queue_voq_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM][PSEUDO_CHANNEL_NUM];
    static queue<pair<int, int>> row_router_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static queue<pair<int, int>> row_voq_router_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM][GROUP_CORE_NUM];
    static queue<pair<int, int>> column_router_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static pair<int, int> rq_num;

    int row_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], column_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], queue_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    int row_voq_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM][GROUP_CORE_NUM];
    int queue_voq_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM][PSEUDO_CHANNEL_NUM];

    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            row_buffer_empty[i][j] = (row_router_buffer[i][j].size() == 0);
            column_buffer_empty[i][j] = (column_router_buffer[i][j].size() == 0);
            queue_buffer_empty[i][j] = (queue_buffer[i][j].size() == 0);
            for (int k = 0; k < GROUP_CORE_NUM; k++) {
                row_voq_buffer_empty[i][j][k] = (row_voq_router_buffer[i][j][k].size() == 0);
            }
            for (int k = 0; k < PSEUDO_CHANNEL_NUM; k++) {
                queue_voq_buffer_empty[i][j][k] = (queue_voq_buffer[i][j][k].size() == 0);
            }
        }
    }

    for (int i = 0; i < CORE_NUM; ++ i) {
        if (hyperx_2_src_id[i] != -1) {
            int row_id = (hyperx_2_dst_id[i] % CORE_NUM) / GROUP_CORE_NUM;
            int col_id = hyperx_2_dst_id[i] % GROUP_CORE_NUM;
            int channel_id = i / GROUP_CORE_NUM;
            int group_core_id = i % GROUP_CORE_NUM;

            queue_buffer[channel_id][group_core_id].push(pair<int, int>(hyperx_2_src_id[i], hyperx_2_dst_id[i]));
            queue_voq_buffer[channel_id][group_core_id][row_id].push(pair<int, int>(hyperx_2_src_id[i], hyperx_2_dst_id[i]));
        }
    }

    // row routing
    for (int i = 0; i < GROUP_CORE_NUM; ++i) { // 遍历列，表示列ID
        for (int j = 0; j < PSEUDO_CHANNEL_NUM; ++j) { // 遍历第i列PE的第j组voq，表示voq ID，同时表示去往的行ID
            for (int k = 0; k < PSEUDO_CHANNEL_NUM; ++k) { // 从该列的第j组voq中选择输出，表示行ID
                if (!queue_voq_buffer_empty[k][i][j]) {
                    int dst_id = queue_voq_buffer[k][i][j].front().second;
                    int another_id = queue_voq_buffer[k][i][j].front().first;

                    int test_row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                    int row_id = j;
                    int col_id = dst_id % GROUP_CORE_NUM;
                    if (test_row_id != row_id) {
                        cout << "voq row route failed" << endl;
                        exit(-1);
                    }

                    if (row_router_buffer[row_id][i].size() < FIFO_SIZE) {
                        row_router_buffer[row_id][i].push(pair<int, int>(another_id, dst_id));
                        row_voq_router_buffer[row_id][i][col_id].push(pair<int, int>(another_id, dst_id));
                        rq_num.first++;

                        queue_buffer[k][i].pop();
                        queue_voq_buffer[k][i][j].pop();
                    }
                    break; // 一组voq仅允许1个输出，当目的buffer满时可以跳过后续转发判定
                }
            }
        }
    }

    // column routing
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++i) { // 遍历行，表示行ID
        for (int j = 0; j < GROUP_CORE_NUM; ++j) { // 遍历第i行PE的第j组voq，表示voq ID，同时表示去往的列ID
            for (int k = 0; k < GROUP_CORE_NUM; ++k) { // 从该行的第j组voq中选择输出，表示列ID
                if (!row_voq_buffer_empty[i][k][j]) {
                    int dst_id = row_voq_router_buffer[i][k][j].front().second;
                    int another_id = row_voq_router_buffer[i][k][j].front().first;

                    int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                    int test_col_id = dst_id % GROUP_CORE_NUM;
                    int col_id = j;

                    if (test_col_id != j) {
                        cout << "voq col route failed." << endl;
                        exit(-1);
                    }

                    if (column_router_buffer[i][col_id].size() < FIFO_SIZE) {
                        column_router_buffer[i][col_id].push(pair<int, int>(another_id, dst_id));

                        row_router_buffer[i][k].pop();
                        row_voq_router_buffer[i][k][j].pop();
                    }
                    break;
                }
            }
        }
    }

    // output
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++j) {
            if (!column_buffer_empty[i][j] && update_q[i * GROUP_CORE_NUM + j].size() < 64) {
            // if (!column_buffer_empty[i][j]) {
                int dst_id = column_router_buffer[i][j].front().second;
                int another_id = column_router_buffer[i][j].front().first;

                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;
                if (row_id != i || col_id != j) {
                    cout << "failed in hyperx out" << endl;
                    exit(-1);
                }

                hyperx_2_src_id_out[i * GROUP_CORE_NUM + j] = another_id;
                hyperx_2_dst_id_out[i * GROUP_CORE_NUM + j] = dst_id;

                column_router_buffer[i][j].pop();
                rq_num.second++;
            } else {
                hyperx_2_src_id_out[i * GROUP_CORE_NUM + j] = -1;
                hyperx_2_dst_id_out[i * GROUP_CORE_NUM + j] = -1;
            }
        }
    }

    return rq_num;
}

pair<int, int> hyperx_network_2() {
    static queue<int> row_router_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], row_router_buffer2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static queue<int> column_router_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], column_router_buffer2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static queue<int> queue_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], queue_buffer2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static queue<int> hyper_out[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], hyper_out2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static pair<int, int> rq_num;

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
    for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
        // crossbar schedule
        int row_recv[PSEUDO_CHANNEL_NUM];
        for (int l = 0; l < PSEUDO_CHANNEL_NUM; l++) {
            row_recv[l] = -1;
        }

        for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
            if (!queue_buffer_empty[i][j]) {
                int dst_id = queue_buffer[i][j].front();
                int another_id = queue_buffer2[i][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;
                if (row_recv[row_id] != -1) {
                    //if (row_recv[row_id] == dst_id) {
                        //queue_buffer[i][j].pop();
                        //queue_buffer2[i][j].pop();
                    //}
                    continue;
                }

                if (row_router_buffer[row_id][j].size() < FIFO_SIZE) {
                    row_router_buffer[row_id][j].push(dst_id);
                    row_router_buffer2[row_id][j].push(another_id);
                    row_recv[row_id] = dst_id;

                    rq_num.first++;
                    queue_buffer[i][j].pop();
                    queue_buffer2[i][j].pop();
                }
            }
        }
    }

    //row routing
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        int col_recv[GROUP_CORE_NUM];
        for (int l = 0; l < GROUP_CORE_NUM; l++) {
            col_recv[l] = -1;
        }
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (!row_buffer_empty[i][j]) {
                int dst_id = row_router_buffer[i][j].front();
                int another_id = row_router_buffer2[i][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;

                if (col_recv[col_id] != -1) {
                    //if (col_recv[col_id] == dst_id) {
                        //row_router_buffer[i][j].pop();
                        //row_router_buffer2[i][j].pop();
                    //}
                    continue;
                }

                if (row_id != i) {
                    cout << "failed in row routing" << endl;
                    exit(-1);
                }
                if (column_router_buffer[i][col_id].size() < FIFO_SIZE) {
                    column_router_buffer[i][col_id].push(dst_id);
                    column_router_buffer2[i][col_id].push(another_id);
                    col_recv[col_id] = dst_id;

                    row_router_buffer[i][j].pop();
                    row_router_buffer2[i][j].pop();
                }
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

                if (row_id != i || col_id != j) {
                    cout << "failed in hyperx out" << endl;
                    exit(-1);
                }
                if (hyper_out[i][j].size() < FIFO_SIZE) {
                    hyper_out[i][j].push(dst_id);
                    hyper_out2[i][j].push(another_id);

                    column_router_buffer[i][j].pop();
                    column_router_buffer2[i][j].pop();
                }
            }
        }
    }

    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (hyper_out[i][j].size() != 0 && update_q[i * GROUP_CORE_NUM + j].size() < 16) {
            // if (hyper_out[i][j].size() != 0) {
                int dst_id = hyper_out[i][j].front();
                int another_id = hyper_out2[i][j].front();

                hyperx_2_src_id_out[i * GROUP_CORE_NUM + j] = another_id;
                hyperx_2_dst_id_out[i * GROUP_CORE_NUM + j] = dst_id;

                hyper_out[i][j].pop();
                hyper_out2[i][j].pop();
                rq_num.second++;
            }
            else {
                hyperx_2_src_id_out[i * GROUP_CORE_NUM + j] = -1;
                hyperx_2_dst_id_out[i * GROUP_CORE_NUM + j] = -1;
            }
        }
    }

    return rq_num;
}

pair<int, int> hyperx_network_2_dp() {
    static queue<int> row_router_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], row_router_buffer2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static queue<int> column_router_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], column_router_buffer2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static queue<int> queue_buffer[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], queue_buffer2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static queue<int> hyper_out[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], hyper_out2[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];
    static pair<int, int> rq_num;
    int dynamic_priority_start_row;
    int dynamic_priority_start_column;

    int row_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], column_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM], queue_buffer_empty[PSEUDO_CHANNEL_NUM][GROUP_CORE_NUM];

    // set dynamic priority
    dynamic_priority_start_column = clk % GROUP_CORE_NUM;
    dynamic_priority_start_row = clk % PSEUDO_CHANNEL_NUM;
    // dynamic_priority_start_column = 0;
    // dynamic_priority_start_row = 0;

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
    for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
        // crossbar schedule
        int row_recv[PSEUDO_CHANNEL_NUM];
        for (int l = 0; l < PSEUDO_CHANNEL_NUM; l++) {
            row_recv[l] = -1;
        }

        for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
            int send_pe_row = (dynamic_priority_start_row + i) % PSEUDO_CHANNEL_NUM;
            if (!queue_buffer_empty[send_pe_row][j]) {
                int dst_id = queue_buffer[send_pe_row][j].front();
                int another_id = queue_buffer2[send_pe_row][j].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;
                if (row_recv[row_id] != -1) {
                    //if (row_recv[row_id] == dst_id) {
                        //queue_buffer[i][j].pop();
                        //queue_buffer2[i][j].pop();
                    //}
                    continue;
                }

                if (row_router_buffer[row_id][j].size() < FIFO_SIZE) {
                    row_router_buffer[row_id][j].push(dst_id);
                    row_router_buffer2[row_id][j].push(another_id);
                    row_recv[row_id] = dst_id;

                    rq_num.first++;
                    queue_buffer[send_pe_row][j].pop();
                    queue_buffer2[send_pe_row][j].pop();
                }
            }
        }
    }

    //row routing
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        int col_recv[GROUP_CORE_NUM];
        for (int l = 0; l < GROUP_CORE_NUM; l++) {
            col_recv[l] = -1;
        }
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            int send_pe_column = (dynamic_priority_start_column + j) % GROUP_CORE_NUM;
            if (!row_buffer_empty[i][send_pe_column]) {
                int dst_id = row_router_buffer[i][send_pe_column].front();
                int another_id = row_router_buffer2[i][send_pe_column].front();
                int row_id = (dst_id % CORE_NUM) / GROUP_CORE_NUM;
                int col_id = dst_id % GROUP_CORE_NUM;

                if (col_recv[col_id] != -1) {
                    //if (col_recv[col_id] == dst_id) {
                        //row_router_buffer[i][j].pop();
                        //row_router_buffer2[i][j].pop();
                    //}
                    continue;
                }

                if (row_id != i) {
                    cout << "failed in row routing" << endl;
                    exit(-1);
                }
                if (column_router_buffer[i][col_id].size() < FIFO_SIZE) {
                    column_router_buffer[i][col_id].push(dst_id);
                    column_router_buffer2[i][col_id].push(another_id);
                    col_recv[col_id] = dst_id;

                    row_router_buffer[i][send_pe_column].pop();
                    row_router_buffer2[i][send_pe_column].pop();
                }
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

                if (row_id != i || col_id != j) {
                    cout << "failed in hyperx out" << endl;
                    exit(-1);
                }
                if (hyper_out[i][j].size() < FIFO_SIZE) {
                    hyper_out[i][j].push(dst_id);
                    hyper_out2[i][j].push(another_id);

                    column_router_buffer[i][j].pop();
                    column_router_buffer2[i][j].pop();
                }
            }
        }
    }

    for (int i = 0; i < PSEUDO_CHANNEL_NUM; ++ i) {
        for (int j = 0; j < GROUP_CORE_NUM; ++ j) {
            if (hyper_out[i][j].size() != 0 && update_q[i * GROUP_CORE_NUM + j].size() < 16) {
            // if (hyper_out[i][j].size() != 0) {
                int dst_id = hyper_out[i][j].front();
                int another_id = hyper_out2[i][j].front();

                hyperx_2_src_id_out[i * GROUP_CORE_NUM + j] = another_id;
                hyperx_2_dst_id_out[i * GROUP_CORE_NUM + j] = dst_id;

                hyper_out[i][j].pop();
                hyper_out2[i][j].pop();
                rq_num.second++;
            }
            else {
                hyperx_2_src_id_out[i * GROUP_CORE_NUM + j] = -1;
                hyperx_2_dst_id_out[i * GROUP_CORE_NUM + j] = -1;
            }
        }
    }

    return rq_num;
}