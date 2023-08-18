#include <bits/stdc++.h>

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

void LoadData(const int vertex_num, const int edge_num, const char* data_file) {
    int v, e;
    if ((data_fp = fopen (data_file, "r")) == NULL) {
        cout << "[ERROR] Failed to open file " << data_file <<endl;
        exit (-1);
    }
    cout << "[INFO] Open file " << data_file << endl;

    edge_list = (struct Edge *) calloc (edge_num, sizeof(Edge));

    // 读入所有边
    int max_vertex_id = 0;
    cout << "[INFO] Read Edge." << endl;
    fscanf (data_fp, "%d %d", &v, &e);
    if (v != vertex_num || e != edge_num) {
        cout << "v & e not matched\n" << endl;
        return;
    }
    for (int i = 0; i < edge_num; i ++) {
        fscanf (data_fp, "%d %d", &(edge_list[i].source_id), &(edge_list[i].target_id));
        max_vertex_id = max (max_vertex_id, edge_list[i].source_id);
        max_vertex_id = max (max_vertex_id, edge_list[i].target_id);
    }
    fclose(data_fp);
    cout << "[INFO] Read Edge Completed." << endl;
    // 按 source_id 和 target_id 排序
    qsort (edge_list, edge_num, sizeof(Edge), compare);
    cout << "[INFO] Sort Edge Completed." << endl;

    // 检查点信息
    if (max_vertex_id >= vertex_num) {
        cout << "[WARNING] max_vertex_id is bigger than vertex_num." << endl;
        exit(-1);
    }

    edge_off.push_back(0);
    int isolate_v_num = 0;
    size_t edge_idx = 0;
    for (int i = 1; i < vertex_num; i++) {
        while (edge_idx < edge_num && edge_list[edge_idx].target_id < i) {
            edge_idx++;
        }
        if (edge_idx == edge_off[edge_off.size() - 1]) isolate_v_num++;
        edge_off.push_back(edge_idx);
    }
    edge_off.push_back(edge_num);
    cout << "[INFO] Generate Offset Completed. isolate_v_num=" << isolate_v_num << endl;
}

void DeltaPR2(const int vertex_num, const int edge_num, vector<pair<double, int>> &pagerank_self) {
    double ALPHA = 0.85; // damping factor
    double TOLERANCE = 1.0e-2;
    int MAX_ITER = 1000;

    vector<pair<double, int>> pagerank_ref(vertex_num);
    vector<double> delta_ref(vertex_num);
    // Track incremental contributions from incoming neighbors
    vector<double> residual_ref(vertex_num);

    // vector<pair<double, int>> pagerank_self(vertex_num);
    vector<double> delta_self(vertex_num);
    // Track incremental contributions from incoming neighbors
    vector<double> residual_self(vertex_num);

    auto deltapr_ref_f = [&](vector<pair<double, int>> &pagerank, vector<double> &delta, vector<double> &residual) -> void {
        // Initialize the data structures
        for (int i = 0; i < vertex_num; i++) {
        pagerank[i].first = 0;
        pagerank[i].second = i;
        delta[i] = 0;
        residual[i] = 1.0 / vertex_num; // Own contribution in the first iteration
        }

        int iterations = 0;

        while (true) {
            bool changed = false;

            for (int src = 0; src < vertex_num; src++) {
                delta[src] = 0;
                if (residual[src] > TOLERANCE) {
                    double oldResidual = residual[src];
                    pagerank[src].first += oldResidual;
                    residual[src] = 0.0;
                    int outdegree = edge_off[src + 1] - edge_off[src];
                    if (outdegree > 0) {
                        delta[src] = oldResidual * ALPHA / outdegree;
                        changed = true;
                    }
                }
            }

            for (int dst = 0; dst < vertex_num; dst++) {
                double sum = 0;
                for (int i = edge_off[dst]; i < edge_off[dst + 1]; i++) {
                    int src = edge_list[i].source_id;
                    if (delta[src] > 0) {
                        sum += delta[src];
                    }
                }
                if (sum > 0) {
                    residual[dst] = sum;
                }
            }

            iterations++;
            if (iterations >= MAX_ITER || !changed) { // termination condition
                break;
            }

        } // end while (true)

        printf("iteration=%d, p[i] = ", iterations);
        for (int i = 0; i < 40; i++) {
            printf(" %.9f", pagerank[i].first);
        }
        printf("\n");
    };
    // deltapr_ref_f(pagerank_ref, delta_ref, residual_ref);
    // sort(pagerank_ref.begin(), pagerank_ref.end(), greater<pair<double, int>>());
    auto deltapr_self_f = [&](vector<pair<double, int>> &pagerank, vector<double> &delta, vector<double> &residual) -> void {
        // Initialize the data structures
        for (int i = 0; i < vertex_num; i++) {
        pagerank[i].first = 0;
        pagerank[i].second = i;
        delta[i] = 0;
        residual[i] = 1.0 / vertex_num; // Own contribution in the first iteration
        }

        int iterations = 0;

        while (true) {
            bool changed = false;

            for (int src = 0; src < vertex_num; src++) {
                delta[src] = 0;
                if (residual[src] > TOLERANCE * pagerank[src].first) {
                    double oldResidual = residual[src];
                    pagerank[src].first += oldResidual;
                    residual[src] = 0.0;
                    int outdegree = edge_off[src + 1] - edge_off[src];
                    if (outdegree > 0) {
                        delta[src] = oldResidual * ALPHA / outdegree;
                        changed = true;
                    }
                }
            }

            for (int dst = 0; dst < vertex_num; dst++) {
                double sum = 0;
                for (int i = edge_off[dst]; i < edge_off[dst + 1]; i++) {
                    int src = edge_list[i].source_id;
                    if (delta[src] > 0) {
                        sum += delta[src];
                    }
                }
                if (sum > 0) {
                    residual[dst] = sum - (iterations == 0 ? 1.0 / vertex_num : 0);
                }
            }

            iterations++;
            if (iterations >= MAX_ITER || !changed) { // termination condition
                break;
            }

        } // end while (true)

        printf("iteration=%d, p[i] = ", iterations);
        for (int i = 0; i < 40; i++) {
            printf(" %.9f", pagerank[i].first);
        }
        printf("\n");
    };
    deltapr_self_f(pagerank_self, delta_self, residual_self);
    // sort(pagerank_self.begin(), pagerank_self.end(), greater<pair<double, int>>());
    FILE* check_fp = fopen("./tmp_check_2.txt", "w");
    for (int i = 0; i < vertex_num; i++) {
        fprintf(check_fp, "%d %.9f\n", pagerank_self[i].second, pagerank_self[i].first);
    }
    fclose(check_fp);
}

void DeltaPR(const int vertex_num, const int edge_num, vector<pair<double, int>> &pr) {
    vector<pair<double, int>> pagerank_ref(vertex_num);
    auto deltapr_ref = [&](vector<pair<double, int>>& pagerank) -> void {
        bool flag = true;
        const double damping = 0.85;
        const double one_over_n = 1.0 / vertex_num;
        // const double one_over_n = 0.85;
        const double e1 = 1e-7;
        const double e2 = 1e-2;
        const double addedConst = (1 - damping) * one_over_n;
        // vector<pair<double, int>> p(vertex_num);
        vector<double> delta(vertex_num), nghsum(vertex_num), delta2(vertex_num);
        vector<int> active_num = {1048576, 1045851, 636190, 511910, 152949, 16752, 4565, 1941, 1026, 544, 463, 37, 15, 13};
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
            bool pull_flag = active_vtx + active_edge > edge_num / 20;
            if (pull_flag) {
                cout << "do pull" << endl;
            } else {
                cout << "do push" << endl;
            }
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
    // deltapr_ref(pagerank_ref);
    // sort(pagerank_ref.begin(), pagerank_ref.end(), greater<pair<double, int>>());
    vector<pair<float, int>> pagerank_self(vertex_num);
    auto deltapr_self = [&](vector<pair<float, int>>& pagerank) -> void {
        bool flag = true;
        const float damping = 0.85;
        const float one_over_n = 1.0 / vertex_num;
        // const double one_over_n = 0.85;
        const float e1 = 1e-7;
        const float e2 = 1e-2;
        const float addedConst = (1 - damping) * one_over_n;
        // vector<pair<double, int>> p(vertex_num);
        vector<float> delta(vertex_num), nghsum(vertex_num), delta2(vertex_num);
        // vector<pair<double, int>> test_pagerank(vertex_num);
        vector<int> active_num = {1048576, 1045851, 636190, 511910, 152949, 16752, 4565, 1941, 1026, 544, 463, 37, 15, 13};
        bool bitmap[2][vertex_num];
        for (int i = 0; i < vertex_num; i++) {
            // pagerank[i].first = one_over_n;
            // pagerank[i].first = addedConst;
            pagerank[i].first = addedConst;
            pagerank[i].second = i;
            delta[i] = one_over_n;
            delta2[i] = addedConst - one_over_n;
            nghsum[i] = 0;
            bitmap[0][i] = true;
            // 孤立点第一轮默认激活
            bitmap[1][i] = true;
        }
        int active_edge = edge_num;
        int active_vtx = vertex_num;
        int iter_id = 1;
        while (flag && iter_id < 100) {
            vector<float> &cu_delta = iter_id % 2 ? delta : delta2;
            vector<float> &next_delta = iter_id % 2 ? delta2 : delta;
            cout << "iter=" << iter_id << ", active_vtx=" << active_vtx << ", active_edge=" << active_edge << endl;
            // if (active_vtx != active_num[iter_id - 1]) break;
            bool pull_flag = active_vtx + active_edge > edge_num / 20;
            if (pull_flag) {
                cout << "do pull" << endl;
            } else {
                cout << "do push" << endl;
            }
            active_vtx = 0;
            active_edge = 0;
            int ct = 0;
            for (int i = 0; i < vertex_num; i++) {
                if (bitmap[(iter_id - 1) % 2][i]) {
                    for (int j = edge_off[i]; j < edge_off[i + 1]; j++) {
                        int src_id = edge_list[j].source_id;
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
                        if (src_id == 0) {
                            cout << "next_add " << (cu_delta[i] /
                                (edge_off[i + 1] - edge_off[i])) * damping << endl;
                            cout << "v_id 0" << " delta " << next_delta[src_id] <<
                                " pr " << pagerank[src_id].first << endl;
                        }
                    }
                }
            }
            for (int i = 0; i < vertex_num; i++) {
                if (bitmap[iter_id % 2][i]) {
                    active_vtx++;
                    active_edge += edge_off[i + 1] - edge_off[i];
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
                break;
            }
            iter_id++;
        }
    };
    deltapr_self(pagerank_self);
    // sort(pagerank_self.begin(), pagerank_self.end(), greater<pair<double, int>>());
    FILE* check_fp = fopen("./tmp_check.txt", "w");
    for (int i = 0; i < vertex_num; i++) {
        fprintf(check_fp, "%d %.9f\n", pagerank_self[i].second, pagerank_self[i].first);
    }
    fclose(check_fp);
}

int main(int argc, char **argv) {
    int vertex_num, edge_num, mem_num;

    if (argc != 4) {
        cout << "Usage: Parameter (vertex_num, edge_num, data_file)." << endl;
        exit(-1);
    }
    vertex_num = atoi (argv[1]);
    edge_num = atoi (argv[2]);

    cout << "Vertex_num = " << vertex_num << ", Edge_num = " << edge_num << endl;

    vector<pair<double, int>> pagerank(vertex_num);
    vector<pair<double, int>> pagerank2(vertex_num);
    LoadData(vertex_num, edge_num, argv[3]);
    DeltaPR(vertex_num, edge_num, pagerank);
    // DeltaPR2(vertex_num, edge_num, pagerank2);
    /*
    FILE* check_fp = fopen("./tmp_check.txt", "w");
    for (int i = 0; i < vertex_num; i++) {
        if (pagerank[i].second != pagerank2[i].second) {
            printf("Check Failed! pr = %f, id = %d,  pr2 = %f, id2 = %d\n", pagerank[i].first, pagerank[i].second, pagerank2[i].first, pagerank2[i].second);
            break;
        }
        fprintf(check_fp, "%f %d\n", pagerank[i].first, pagerank[i].second);
    }
    fclose(check_fp);
    */
    return 0;
}