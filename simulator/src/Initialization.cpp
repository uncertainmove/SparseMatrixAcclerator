#include "accelerator.h"
#include "parameter.h"

using namespace std;

extern V_VALUE_TP DELTA1_BRAM[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1];
extern V_VALUE_TP DELTA2_BRAM[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1];
extern int Offset_URAM[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1][2];
extern V_VALUE_TP PR_URAM[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1][2];
extern int VTX_NUM, EDGE_NUM;

int Csr_Offset[MAX_VERTEX_NUM + 1];
vector<int> Ori_Edge_MEM;
vector<int> Edge_MEM[PSEUDO_CHANNEL_NUM];

int VERTEX_DEGREE[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1];

vector<pair<int, int>> no_self_edge_vec;

int ori_pr(vector<V_VALUE_TP>& pr_res) {
    return 0;
}

void Validation_Vertex_bram(int root_id) {
}

void Initialize_Input_Graph(int v_num, int e_num, char *off_file, char *edge_file, int has_header) {
    FILE *off_fp, *list_fp;
    off_fp = fopen(off_file, "r");
    list_fp = fopen(edge_file, "r");
    if(!off_fp) {
       cout << "[ERROR] Failed to open " << off_file << endl;
       exit(-1);
    }
    if(!list_fp) {
        cout << "[ERROR] Failed to open " << edge_file << endl;
        exit(-1);
    }
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        Edge_MEM[i].clear();
    }
    Ori_Edge_MEM.clear();
    no_self_edge_vec.clear();
    if (has_header) {
        int tmp_v_num, tmp_e_num;
        fscanf (off_fp, "%d", &tmp_v_num);
        fscanf (list_fp, "%d", &tmp_e_num);
        assert (tmp_v_num == v_num);
        assert (tmp_e_num == e_num);
    }
    for (int i = 0; i < v_num + 1; i++) {
        fscanf (off_fp, "%d", &Csr_Offset[i]);
    }
    for (int i = 0; i < e_num; i++) {
        int dst_id;
        fscanf (list_fp, "%d", &dst_id);
        assert(dst_id < v_num);
        Ori_Edge_MEM.push_back(dst_id);
    }
    auto compare = [](const int v1, const int v2) -> bool {
        return (Csr_Offset[v1 + 1] - Csr_Offset[v1]) > (Csr_Offset[v2 + 1] - Csr_Offset[v2]);
    };
    for (int i = 0; i < VTX_NUM; i++) {
        sort(&Ori_Edge_MEM[Csr_Offset[i]], &Ori_Edge_MEM[Csr_Offset[i + 1]], compare);
        for (int off = Csr_Offset[i]; off < Csr_Offset[i + 1]; off++) {
            Edge_MEM[(i % CORE_NUM) / GROUP_CORE_NUM].push_back(Ori_Edge_MEM[off]);
        }
    }
    #if STORE_DATA
        // write file
        for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
            FILE* edge_fp;
            string cu_filename = prefix;
            cu_filename += to_string(i);
            cu_filename += ".txt";
            edge_fp = fopen(cu_filename.c_str(), "w");
            printf("channel_%d_size=%d\n", i, Edge_MEM[i].size());
            for (int j = 0; j < Edge_MEM[i].size(); j++) {
                fprintf(edge_fp, "%d\n", Edge_MEM[i][j]);
            }
            fclose(edge_fp);
        }
    #endif

    fclose(off_fp);
    fclose(list_fp);
    printf("Initialize Graph Complete\n");
}

void Initialize_Offset_Uram(int V_Num) {
    for (int i = 0; i < V_Num; i++) {
        if (i % GROUP_CORE_NUM == 0 && i / CORE_NUM == 0) {
            Offset_URAM[i % CORE_NUM][i / CORE_NUM][0] = 0;
            Offset_URAM[i % CORE_NUM][i / CORE_NUM][1] = Csr_Offset[i + 1] - Csr_Offset[i];
        } else if (i % GROUP_CORE_NUM == 0) {
            Offset_URAM[i % CORE_NUM][i / CORE_NUM][0] = Offset_URAM[(i - CORE_NUM + GROUP_CORE_NUM - 1) % CORE_NUM][(i - CORE_NUM + GROUP_CORE_NUM - 1) / CORE_NUM][1];
            Offset_URAM[i % CORE_NUM][i / CORE_NUM][1] = Csr_Offset[i + 1] - Csr_Offset[i] + Offset_URAM[i % CORE_NUM][i / CORE_NUM][0];
        } else {
            Offset_URAM[i % CORE_NUM][i / CORE_NUM][0] = Offset_URAM[(i - 1) % CORE_NUM][(i - 1) / CORE_NUM][1];
            Offset_URAM[i % CORE_NUM][i / CORE_NUM][1] = Csr_Offset[i + 1] - Csr_Offset[i] + Offset_URAM[i % CORE_NUM][i / CORE_NUM][0];
        }
    }
    printf("Initialize Offset Complete\n");
}

void Initialize_Delta_Bram() {
    const V_VALUE_TP ONE_OVER_N = 1.0 / VTX_NUM;
    const V_VALUE_TP ADDED_CONST = (V_VALUE_TP(1.0) - DAMPING) * ONE_OVER_N;
    for(int i = 0; i < CORE_NUM; i++){
        for(int j = 0; j < MAX_VERTEX_NUM / CORE_NUM + 1; j++){
            DELTA1_BRAM[i][j] = V_VALUE_TP(ONE_OVER_N);
            DELTA2_BRAM[i][j] = V_VALUE_TP(ADDED_CONST - ONE_OVER_N, 1);
        }
    }
    printf("Initialize Delta Bram Complete\n");
}

void Initialize_PR_Uram(){
    const V_VALUE_TP ONE_OVER_N = 1.0 / VTX_NUM;
    const V_VALUE_TP ADDED_CONST = (V_VALUE_TP(1.0) - DAMPING) * ONE_OVER_N;
    for(int i = 0; i < CORE_NUM; i++){
        for(int j = 0; j < MAX_VERTEX_NUM / CORE_NUM + 1; j++){
            PR_URAM[i][j][0] = ADDED_CONST;
        }
    }
    printf("Initialize PR Uram Complete\n");
}

void Initialize_Vertex_Degree(int V_Num) {
    for (int i = 0; i < V_Num; i++) {
        int Loff = Offset_URAM[i % CORE_NUM][i / CORE_NUM][0], Roff = Offset_URAM[i % CORE_NUM][i / CORE_NUM][1];
        VERTEX_DEGREE[i % CORE_NUM][i / CORE_NUM] = Roff - Loff;
    }
}