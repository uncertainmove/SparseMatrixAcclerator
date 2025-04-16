#include <bits/stdc++.h>
#include <fstream>

using namespace std;

typedef struct Edge {
    int source_id;
    int target_id;
} Edge;

FILE* data_fp;
Edge* edge_list;
vector<int> edge_off;

int compare (const void *edge1, const void *edge2) {
    Edge *e1 = (Edge *) edge1;
    Edge *e2 = (Edge *) edge2;
    // -1 代表不交换位置，1 代表交换位置
    if (e1 -> target_id < e2 -> target_id) return -1;
    else if (e1 -> target_id == e2 -> target_id && e1 -> source_id <= e2 -> source_id) return -1;
    else return 1;
}

void LoadData (int has_header, int vertex_num, int edge_num, char *off_file, char *edge_file) {
    FILE *off_fp, *list_fp;
    off_fp = fopen (off_file, "r");
    list_fp = fopen (edge_file, "r");
    if(off_fp == nullptr) {
       cout << "[ERROR] Failed to open " << off_file << endl;
       exit(-1);
    }
    if(list_fp == nullptr) {
        cout << "[ERROR] Failed to open " << edge_file << endl;
        exit(-1);
    }
    cout << "vertex_num: " << vertex_num << ", edge_num: " << edge_num << endl;
    if (has_header) {
        int tmp_v_num, tmp_e_num;
        fscanf (off_fp, "%d", &tmp_v_num);
        fscanf (list_fp, "%d", &tmp_e_num);
        assert (tmp_v_num == vertex_num);
        assert (tmp_e_num == edge_num);
    }
    int off_v = 0;
    for (int i = 0; i < vertex_num + 1; i++) {
        fscanf (off_fp, "%d", &off_v);
        edge_off.push_back (off_v);
    }
    cout << "off_v: " << off_v << ", edge_off_last: " << edge_off[vertex_num] << endl;
    edge_list = (struct Edge *) calloc (edge_off[vertex_num], sizeof(Edge));
    for (int vid = 0; vid < vertex_num; vid++) {
        for (int off = edge_off[vid]; off < edge_off[vid + 1]; off++) {
            int dst_id = 0;
            fscanf (list_fp, "%d", &dst_id);
            edge_list[off].source_id = vid;
            edge_list[off].target_id = dst_id;
            // if (dst_id == 6380) {
                // cout << "src: " << vid << ", dst: " << dst_id << endl;
            // }
        }
    }
    fclose(off_fp);
    fclose(list_fp);
    printf("Initialize Graph Complete\n");
}

int ori_pr_push(int vertex_num, int edge_num, float *res) {
    const double damping = 0.85;
    const double one_over_n = 1.0 / vertex_num;
    const double addedConst = (1.0 - damping) / vertex_num;
    const double epsilon = 1e-7;
    double *vertex_p[2];
    int *degree = new int [vertex_num];
    vertex_p[0] = new double [vertex_num];
    vertex_p[1] = new double [vertex_num];
    for (int i = 0; i < vertex_num; i++) {
        vertex_p[0][i] = one_over_n;
        vertex_p[1][i] = 0;
        degree[i] = edge_off[i + 1] - edge_off[i];
    }
    int iter;
    for (iter = 0; iter < 100; iter++) {
        for (int i = 0; i < vertex_num; i++) {
            for (int off = edge_off[i]; off < edge_off[i + 1]; off++) {
                int dst_id = edge_list[off].target_id;
                vertex_p[(iter + 1) % 2][dst_id] += vertex_p[iter % 2][i] / degree[i];
                // cout << "src_id: " << i << ", dst_id: " << dst_id << ", delta: " << vertex_p[iter % 2][i] / degree[i] <<
                    // ", p: " << vertex_p[(iter + 1) % 2][dst_id] << endl;
            }
        }
        float L1_norm = 0;
        for (int i = 0; i < vertex_num; i++) {
            vertex_p[(iter + 1) % 2][i] = damping * vertex_p[(iter + 1) % 2][i] + addedConst;
            L1_norm += fabs (vertex_p[(iter + 1) % 2][i] - vertex_p[iter % 2][i]);
            vertex_p[iter % 2][i] = 0;
        }
        if (L1_norm < epsilon) {
            cout << "iteration: " << iter + 1 << ", L1_norm: " << L1_norm << endl;
            cout << "Ori PR Push complete iteration: " << iter + 1 << endl;
            iter++;
            break;
        } else {
            cout << "iteration: " << iter + 1 << ", L1_norm: " << L1_norm << endl;
        }
    }
    FILE* check_fp = fopen("./pr_tmp_res.txt", "w");
    for (int i = 0; i < vertex_num; i++) {
        res[i] = vertex_p[iter % 2][i];
        fprintf(check_fp, "%d %.9f\n", i, res[i]);
    }
    fclose(check_fp);
    cout << "Ori PR Push Complete" << endl;

    delete [] degree;
    delete [] vertex_p[0];
    delete [] vertex_p[1];

    return iter;
}

void DeltaPR(const int vertex_num, const int edge_num, vector<pair<float, int>> &pr) {
    auto deltapr_ref = [&](vector<pair<float, int>>& pagerank) -> void {
        bool flag = true;
        const double damping = 0.85;
        const double one_over_n = 1.0 / vertex_num;
        // const double one_over_n = 0.85;
        const double e1 = 1e-7;
        const double e2 = 1e-2;
        const double addedConst = (1 - damping) * one_over_n;
        vector<double> delta(vertex_num), nghsum(vertex_num), delta2(vertex_num);
        // vector<int> active_num = {1048576, 1045851, 636190, 511910, 152949, 16752, 4565, 1941, 1026, 544, 463, 37, 15, 13};
        bool bitmap[2][vertex_num];
        cout << "deltapr ref" << endl;
        for (int i = 0; i < vertex_num; i++) {
            pagerank[i].first = 0;
            pagerank[i].second = i;
            delta[i] = one_over_n;
            delta2[i] = addedConst;
            nghsum[i] = 0;
            bitmap[0][i] = true;
            bitmap[1][i] = false;
        }
        int active_edge = edge_num;
        int active_vtx = vertex_num;
        int iter_id = 1;
        while (flag && iter_id < 100) {
            cout << "iter=" << iter_id << ", active_vtx=" << active_vtx << ", active_edge=" << active_edge << endl;
            // if (active_vtx != active_num[iter_id - 1]) break;
            active_vtx = 0;
            active_edge = 0;
            // 1. 获取邻居数据
            for (int i = 0; i < vertex_num; i++) {
                for (int j = edge_off[i]; j < edge_off[i + 1]; j++) {
                    // 数据按照target_id排序
                    int src_id = edge_list[j].source_id;
                    if (bitmap[(iter_id - 1) % 2][src_id]) {
                        delta2[i] += (delta[src_id] / (edge_off[src_id + 1] - edge_off[src_id])) * damping;
                    }
                }
            }
            for (int i = 0; i < vertex_num; i++) {
                if ((iter_id == 1 && fabs(delta2[i] - one_over_n) > e2 * (pagerank[i].first + delta2[i])) || (iter_id != 1 && fabs(delta2[i]) > e2 * pagerank[i].first)) {
                    active_vtx++;
                    active_edge += edge_off[i + 1] - edge_off[i];
                    bitmap[iter_id % 2][i] = true;
                }
                if (iter_id == 1 || fabs(delta2[i]) > e2 * pagerank[i].first) {
                    pagerank[i].first += delta2[i];
                }
                if (iter_id == 1) {
                    delta2[i] -= one_over_n;
                }
            }

            double reduce_delta = 0;
            for (int i = 0; i < vertex_num; i++) {
                delta[i] = delta2[i];
                delta2[i] = 0;
                nghsum[i] = fabs(delta[i]);
                bitmap[(iter_id - 1) % 2][i] = false;
                reduce_delta += nghsum[i];
                nghsum[i] = 0;
            }
            cout << reduce_delta << endl;
            /*
            printf("p[i] = ");
            for (int i = 0; i < 40; i++) {
                printf(" %.9f", pagerank[i].first);
            }
            printf("\n");
            printf("delta[i] = ");
            for (int i = 0; i < 40; i++) {
                printf(" %.9f", delta[i]);
            }
            printf("\n");
            printf("reduce_delta=%.9f\n", reduce_delta);
            */
            if (reduce_delta < e1) {
                break;
            }
            iter_id++;
        }
    };
    // deltapr_ref(pr);
    // sort(pagerank_ref.begin(), pagerank_ref.end(), greater<pair<double, int>>());
    auto deltapr_self = [&](vector<pair<float, int>>& pagerank) -> void {
        bool flag = true;
        const float damping = 0.85;
        const float one_over_n = 1.0 / vertex_num; // 16383:0.00006104  0x3880029a  131073:36ffff80
        // const double one_over_n = 0.85;
        const float e1 = 1e-7;
        const float e2 = 1e-2;
        const float addedConst = (1 - damping) * one_over_n;// 16383:0.000009156  0x37199cb8  131073:3599994d
        // vector<pair<double, int>> p(vertex_num);
        vector<float> delta(vertex_num), nghsum(vertex_num), delta2(vertex_num);
        // vector<pair<double, int>> test_pagerank(vertex_num);
        // vector<int> active_num = {1048576, 1045851, 636190, 511910, 152949, 16752, 4565, 1941, 1026, 544, 463, 37, 15, 13};
        bool bitmap[2][vertex_num];
        for (int i = 0; i < vertex_num; i++) {
            // pagerank[i].first = one_over_n;
            // pagerank[i].first = addedConst;
            pagerank[i].first = addedConst;
            pagerank[i].second = i;
            delta[i] = one_over_n;
            delta2[i] = addedConst - one_over_n;// 16383:-0.00005188  0xb85999ba 131073: -0.0000064849358754 b6d9992d
            nghsum[i] = 0;
            bitmap[0][i] = true;
            // 孤立点第一轮默认激活
            bitmap[1][i] = true;
        }
        int active_edge = edge_num;
        int active_vtx = vertex_num;
        int iter_id = 1;
        int total_travel_edge = 0;
        while (flag && iter_id < 100) {
            vector<float> &cu_delta = iter_id % 2 ? delta : delta2;
            vector<float> &next_delta = iter_id % 2 ? delta2 : delta;
            cout << "iter=" << iter_id << ", active_vtx=" << active_vtx << ", active_edge=" << active_edge << endl;
            total_travel_edge += active_edge;
            // if (active_vtx != active_num[iter_id - 1]) break;
            active_vtx = 0;
            active_edge = 0;
            int ct = 0;
            for (int i = 0; i < vertex_num; i++) {
                if (bitmap[(iter_id - 1) % 2][i]) {
                    for (int j = edge_off[i]; j < edge_off[i + 1]; j++) {
                        int src_id = edge_list[j].target_id;
                        // 1. 获取邻居数据，更新delta值
                        next_delta[src_id] += (cu_delta[i] /
                            (edge_off[i + 1] - edge_off[i])) * damping;
                        // 2. 更新PR值
                        pagerank[src_id].first += (cu_delta[i] /
                            (edge_off[i + 1] - edge_off[i])) * damping;
                        // 3. 生成激活点
                        if (fabs(next_delta[src_id]) > e2 *
                            (pagerank[src_id].first - (iter_id == 1 ? 0 :
                                next_delta[src_id]))) {
                            bitmap[iter_id % 2][src_id] = true;
                        } else {
                            bitmap[iter_id % 2][src_id] = false;
                        }
                        // if (src_id == 686905) {
                        // cout << "iteration_id: " << iter_id << ", i: " << i << ", id: " << src_id << ", accu: " << (cu_delta[i] /
                            // (edge_off[i + 1] - edge_off[i])) * damping << ", delta: " << next_delta[src_id] << ", pr: " << pagerank[src_id].first <<
                            // ", active: " << bitmap[iter_id % 2][src_id] << endl;
                        // }
                    }
                }
            }
            for (int i = 0; i < vertex_num; i++) {
                if (bitmap[iter_id % 2][i]) {
                    active_vtx++;
                    active_edge += edge_off[i + 1] - edge_off[i];
                } else {
                    //cout << i << endl;
                }
            }

            float reduce_delta = 0;
            for (int i = 0; i < vertex_num; i++) {
                // cu_delta[i] = next_delta[i];
                cu_delta[i] = 0; // 作为next_delta时，判断当前轮次和自身记录轮次是否一样
                bitmap[(iter_id - 1) % 2][i] = false; // 第一轮时初始化下一轮次为true，第二轮开始初始化为false
                nghsum[i] = fabs(next_delta[i]);
                reduce_delta += nghsum[i];
                nghsum[i] = 0;
            }
            cout << reduce_delta << endl;
            /*
            printf("p[i] = ");
            for (int i = 0; i < 40; i++) {
                printf(" %.9f", pagerank[i].first);
            }
            printf("\n");
            printf("delta[i] = ");
            for (int i = 0; i < 40; i++) {
                printf(" %.9f", delta[i]);
            }
            printf("\n");
            printf("reduce_delta=%.9f\n", reduce_delta);
            */
            if (reduce_delta < e1) {
                cout << "iteration_num = " << iter_id << endl;
                cout << "total_travel_edge = " << total_travel_edge << endl;
                break;
            }
            iter_id++;
        }
    };
    deltapr_self(pr);
    // sort(pagerank_self.begin(), pagerank_self.end(), greater<pair<double, int>>());
}

int main(int argc, char **argv) {
    int vertex_num, edge_num, mem_num;

    if (argc != 7) {
        cout << "Usage: Parameter (vertex_num, edge_num, off_file, list_file, res_file, has_header)." << endl;
        exit(-1);
    }
    vertex_num = atoi (argv[1]);
    edge_num = atoi (argv[2]);
    int has_header = atoi(argv[6]);

    LoadData (has_header, vertex_num, edge_num, argv[3], argv[4]);

    vector<pair<float, int>> pagerank(vertex_num);
    DeltaPR(vertex_num, edge_num, pagerank);

    FILE* check_fp = fopen(argv[5], "w");
    if (check_fp == nullptr) {
       cout << "[ERROR] Failed to open " << argv[5] << endl;
       exit(-1);
    }
    for (int i = 0; i < vertex_num; i++) {
        fprintf(check_fp, "%d %x\n", pagerank[i].second, *(int *)&pagerank[i].first);
    }
    fclose(check_fp);
    cout << "Do Origin PR" << endl;
    float ori_res[vertex_num];
    ori_pr_push (vertex_num, edge_num, ori_res);
    int error_counter = 0;
    for (int i = 0; i < vertex_num; i++) {
        float debug_pr_v = ori_res[i];
        float ret_pr_v  = pagerank[i].first;
        float divation = ret_pr_v - debug_pr_v > 0 ? ret_pr_v - debug_pr_v : debug_pr_v - ret_pr_v;
      if (divation / debug_pr_v < 0 || divation / debug_pr_v > 0.2) {
        error_counter++;
        printf("id: %d, ori_pr_res: %.9f, delta_pr_res: %.9f, err: %.9f.\n", i, debug_pr_v, ret_pr_v, divation / debug_pr_v);
      }
      if (error_counter > 5) {
        cout << "Too many errors found. Exiting check of result." << endl;
        break;
      }
    }
    cout << "Check Complete" << endl;
    return 0;
}