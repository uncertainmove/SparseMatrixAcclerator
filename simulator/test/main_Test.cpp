#include "gtest/gtest.h"
#include "Accelerator.h"
#include "debug.h"

#include <iostream>

using namespace std;

#if DEBUG

extern int clk;
extern int rst_rd;
extern int ROOT_ID;
extern int MAX_ITERATION;

extern int Csr_Offset[MAX_VERTEX_NUM + 1];
extern vector<int> Ori_Edge_MEM;
extern int VERTEX_BRAM[CORE_NUM][MAX_VERTEX_NUM / CORE_NUM + 1];
extern int VTX_NUM;
extern int EDGE_NUM;
extern int DEBUG_PUSH_FLAG;
extern int DEBUG_NEXT_PUSH_FLAG;
extern int last_update_cycle;

class main_Test : public testing::Test {
public:
    virtual void SetUp() {
        ASSERT_EQ(DEBUG, 1);
    }
    virtual void TearDown() {
    }
    int ori_bfs(int root_id, vector<int>& bfs_res) {
        bfs_res[root_id] = 0;
        bool vtx_update = true;
        int level = 0;
        // check outlier root id
        if (Csr_Offset[root_id] == Csr_Offset[root_id + 1]) return -1;
        while (vtx_update) {
            vtx_update = false;
            for (int node_id = 0; node_id < VTX_NUM; node_id++) {
                if (bfs_res[node_id] != -1 && bfs_res[node_id] <= level) continue;
                for (int off = Csr_Offset[node_id]; off < Csr_Offset[node_id + 1]; off++) {
                    int src_id = Ori_Edge_MEM[off];
                    if (bfs_res[src_id] == -1) continue;
                    if (bfs_res[node_id] == -1) {
                        bfs_res[node_id] = bfs_res[src_id] + 1;
                        vtx_update = true;
                    } else if (bfs_res[src_id] + 1 < bfs_res[node_id]) {
                        vtx_update = true;
                        bfs_res[node_id] = bfs_res[src_id] + 1;
                    }
                }
            }
            level++;
        }
        return level;
    }
};

// test push mode in small graph
TEST_F(main_Test, test_Push_SmallGraph) {
    // initialize
    DEBUG_PUSH_FLAG = 1;
    DEBUG_NEXT_PUSH_FLAG = 1;
    Initialize_Input_Graph("../data/edge_off_1.txt", "../data/edge_list_1.txt");
    for (int root_id = 0; root_id < 1; root_id++) {
        vector<int> bfs_res(VTX_NUM, -1);
        MAX_ITERATION = ori_bfs(root_id, bfs_res) + 1;
        printf("max_iteration=%d\n", MAX_ITERATION);
        for (int i = 0; i < MAX_ITERATION; i++) {
            int updated = 0;
            for (int j = 0; j < VTX_NUM; j++) {
                if (bfs_res[j] == i) updated++;
            }
            printf("iteration=%d, updated=%d\n", i, updated);
        }
        // call main
        _main("../data/edge_off_1.txt", "../data/edge_list_1.txt", root_id, 30000);
        printf("VTX_NUM=%d, EDGE_NUM=%d\n", VTX_NUM, EDGE_NUM);
        // check
        int error = 0;
        for (int i = 0; i < VTX_NUM; i++) {
            if (VERTEX_BRAM[i % CORE_NUM][i / CORE_NUM] != bfs_res[i]) {
                printf("mismatch: v_id=%d\n", i);
            }
            ASSERT_EQ(VERTEX_BRAM[i % CORE_NUM][i / CORE_NUM], bfs_res[i]);
        }
        printf("last_clk=%d\n", clk);
        printf("last_update_cycle=%d\n", last_update_cycle);
    }
}

// test pull mode in small graph
TEST_F(main_Test, test_Pull_SmallGraph) {
    // initialize
    DEBUG_PUSH_FLAG = 0;
    DEBUG_NEXT_PUSH_FLAG = 0;
    last_update_cycle = 0;
    Initialize_Input_Graph("../data/edge_off_1.txt", "../data/edge_list_1.txt");
    for (int root_id = 0; root_id < 1; root_id++) {
        vector<int> bfs_res(VTX_NUM, -1);
        MAX_ITERATION = ori_bfs(root_id, bfs_res) + 1;
        printf("max_iteration=%d\n", MAX_ITERATION);
        for (int i = 0; i < MAX_ITERATION; i++) {
            int updated = 0;
            for (int j = 0; j < VTX_NUM; j++) {
                if (bfs_res[j] == i) updated++;
            }
            printf("iteration=%d, updated=%d\n", i, updated);
        }
        // call main
        // do rst
        // rst_rd = 1;
        // _main("../data/edge_off_1.txt", "../data/edge_list_1.txt", root_id, 100);
        // rst_rd = 0;
        _main("../data/edge_off_1.txt", "../data/edge_list_1.txt", root_id, 30000);
        printf("VTX_NUM=%d, EDGE_NUM=%d, ROOT=%d\n", VTX_NUM, EDGE_NUM, root_id);
        // check
        for (int i = 0; i < VTX_NUM; i++) {
            if (VERTEX_BRAM[i % CORE_NUM][i / CORE_NUM] != bfs_res[i]) {
                printf("mismatch: v_id=%d\n", i);
            }
            ASSERT_EQ(VERTEX_BRAM[i % CORE_NUM][i / CORE_NUM], bfs_res[i]);
        }
        printf("last_clk=%d\n", clk);
        printf("last_update_cycle=%d\n", last_update_cycle);
    }
}

// test pull mode in small graph
TEST_F(main_Test, test_Pull_LargeGraph) {
    // initialize
    DEBUG_PUSH_FLAG = 0;
    DEBUG_NEXT_PUSH_FLAG = 0;
    last_update_cycle = 0;
    Initialize_Input_Graph("../data/edge_off.txt", "../data/edge_list.txt");
    for (int root_id = 0; root_id < 1; root_id++) {
        vector<int> bfs_res(VTX_NUM, -1);
        MAX_ITERATION = ori_bfs(root_id, bfs_res) + 1;
        printf("max_iteration=%d\n", MAX_ITERATION);
        // call main
        // do rst
        // rst_rd = 1;
        // _main("../data/edge_off_1.txt", "../data/edge_list_1.txt", root_id, 100);
        // rst_rd = 0;
        _main("../data/edge_off.txt", "../data/edge_list.txt", root_id, 500000);
        printf("VTX_NUM=%d, EDGE_NUM=%d, ROOT=%d\n", VTX_NUM, EDGE_NUM, root_id);
        // check
        for (int i = 0; i < VTX_NUM; i++) {
            if (VERTEX_BRAM[i % CORE_NUM][i / CORE_NUM] != bfs_res[i]) {
                printf("mismatch: v_id=%d\n", i);
            }
            ASSERT_EQ(VERTEX_BRAM[i % CORE_NUM][i / CORE_NUM], bfs_res[i]);
        }
        printf("last_clk=%d\n", clk);
        printf("last_update_cycle=%d\n", last_update_cycle);
    }
}

// test pull mode in small graph
TEST_F(main_Test, test_PushPull_SmallGraph) {
    // initialize
    last_update_cycle = 0;
    Initialize_Input_Graph("../data/edge_off_1.txt", "../data/edge_list_1.txt");
    for (int root_id = 0; root_id < 1; root_id++) {
        vector<int> bfs_res(VTX_NUM, -1);
        MAX_ITERATION = ori_bfs(root_id, bfs_res) + 1;
        printf("max_iteration=%d\n", MAX_ITERATION);
        for (int i = 0; i < MAX_ITERATION + 1; i++) {
            int updated = 0;
            for (int j = 0; j < VTX_NUM; j++) {
                if (bfs_res[j] <= i && bfs_res[j] != -1) updated++;
            }
            printf("iteration=%d, updated=%d\n", i, updated);
        }
        // call main
        // do rst
        // rst_rd = 1;
        // _main("../data/edge_off_1.txt", "../data/edge_list_1.txt", root_id, 100);
        // rst_rd = 0;
        _main("../data/edge_off_1.txt", "../data/edge_list_1.txt", root_id, 30000);
        printf("VTX_NUM=%d, EDGE_NUM=%d, ROOT=%d\n", VTX_NUM, EDGE_NUM, root_id);
        // check
        for (int i = 0; i < VTX_NUM; i++) {
            if (VERTEX_BRAM[i % CORE_NUM][i / CORE_NUM] != bfs_res[i]) {
                printf("mismatch: v_id=%d\n", i);
            }
            ASSERT_EQ(VERTEX_BRAM[i % CORE_NUM][i / CORE_NUM], bfs_res[i]);
        }
        printf("last_clk=%d\n", clk);
        printf("last_update_cycle=%d\n", last_update_cycle);
    }
}

// test pull mode in small graph
TEST_F(main_Test, test_PushPull_LargeGraph) {
    // initialize
    last_update_cycle = 0;
    Initialize_Input_Graph("../data/edge_off.txt", "../data/edge_list.txt");
    // for (int root_id = 220217; root_id < 220218; root_id++) {
    for (int root_id = 30160; root_id < 30161; root_id++) {
        vector<int> bfs_res(VTX_NUM, -1);
        MAX_ITERATION = ori_bfs(root_id, bfs_res) + 1;
        printf("root_id=%d, max_iteration=%d\n", root_id, MAX_ITERATION);
        bool skip = false;
        if (MAX_ITERATION == 0) {
            printf("Skip: Outlier root id\n");
            skip = true;
        }
        for (int i = 0; i < POWER_LAW_V_NUM; i++) {
            if (bfs_res[VERTEX_NOT_READ[i]] > 2) {
                printf("Skip: level > 2\n");
                skip = true;
                break;
            }
        }
        if (skip) continue;
        for (int i = 0; i < MAX_ITERATION + 1; i++) {
            int updated = 0;
            for (int j = 0; j < VTX_NUM; j++) {
                if (bfs_res[j] <= i && bfs_res[j] != -1) updated++;
            }
            printf("iteration=%d, updated=%d\n", i, updated);
        }
        // call main
        // do rst
        // rst_rd = 1;
        // _main("../data/edge_off_1.txt", "../data/edge_list_1.txt", root_id, 100);
        // rst_rd = 0;
        _main("../data/edge_off.txt", "../data/edge_list.txt", root_id, 50000);
        printf("VTX_NUM=%d, EDGE_NUM=%d, ROOT=%d\n", VTX_NUM, EDGE_NUM, root_id);
        // check
        for (int i = 0; i < VTX_NUM; i++) {
            if (VERTEX_BRAM[i % CORE_NUM][i / CORE_NUM] != bfs_res[i]) {
                printf("mismatch: v_id=%d\n", i);
            }
            ASSERT_EQ(VERTEX_BRAM[i % CORE_NUM][i / CORE_NUM], bfs_res[i]);
        }
        printf("last_clk=%d\n", clk);
        printf("last_update_cycle=%d\n", last_update_cycle);
    }
}


// test pull mode in small graph
TEST_F(main_Test, test_DegreeSort) {
    char* off_file = "../data/edge_off_1.txt";
    char* edge_file = "../data/edge_list_1.txt";
    ifstream in_off,in_list;
    in_off.open(off_file);
    in_list.open(edge_file);
    if(!in_off) {
       cout << "[ERROR] Failed to open " << off_file << endl;
       exit(-1);
    }
    if(!in_list) {
        cout << "[ERROR] Failed to open " << edge_file << endl;
        cout<<"error"<<endl;
        exit(-1);
    }
    VTX_NUM = -1;
    for (int i = 0; i < PSEUDO_CHANNEL_NUM; i++) {
        Ori_Edge_MEM.clear();
    }
    int t1;
    int*p1=&Csr_Offset[0];
    //int*p2=&Edge_MEM[0][0];
    while(in_off>>t1)             //遇到空白符结束
    {
        *p1=t1;
        p1++;
        VTX_NUM++;
    }
    int count = 0;
    int l, r;
    EDGE_NUM = 0;
    vector<pair<int, int>> degree;
    while (count < VTX_NUM) {
        l = Csr_Offset[count], r = Csr_Offset[count + 1];
        degree.push_back(pair<int, int>(r - l, count));
        for (int i = l; i < r; i++) {
            in_list >> t1;
            EDGE_NUM++;
            Ori_Edge_MEM.push_back(t1);
        }
        count++;
    }
    sort(degree.begin(), degree.end(), greater<pair<int, int>>());
    ofstream ofs("./degree_sort.txt");
    for (int i = 0; i < VTX_NUM; i++) {
        ofs << degree[i].first << " " << degree[i].second << endl;
    }
    /*
    vector<pair<int, int>> neig_degree;
    for (int i = Csr_Offset[degree[0].second]; i < Csr_Offset[degree[0].second + 1]; i++) {
        neig_degree.push_back(pair<int, int>(Csr_Offset[Ori_Edge_MEM[i] + 1] - Csr_Offset[Ori_Edge_MEM[i]], Ori_Edge_MEM[i]));
    }
    ofstream ofs("./neigh_degree_sort.txt");
    for (int i = 0; i < neig_degree.size(); i++) {
        ofs << neig_degree[i].first << " " << neig_degree[i].second << endl;
    }
    */
    ofs.close();
    in_off.close();
    in_list.close();
}

// test push mode in large graph
TEST_F(main_Test, test_Push_LargeGraph) {
    // initialize
    DEBUG_PUSH_FLAG = 1;
    DEBUG_NEXT_PUSH_FLAG = 1;
    last_update_cycle = 0;
    Initialize_Input_Graph("../data/edge_off.txt", "../data/edge_list.txt");
    for (int root_id = 0; root_id < 2; root_id++) {
        vector<int> bfs_res(VTX_NUM, -1);
        MAX_ITERATION = ori_bfs(root_id, bfs_res) + 1;
        printf("max_iteration=%d\n", MAX_ITERATION);
        // call main
        _main("../data/edge_off.txt", "../data/edge_list.txt", root_id, 300000);
        printf("VTX_NUM=%d, EDGE_NUM=%d\n", VTX_NUM, EDGE_NUM);
        // check
        for (int i = 0; i < VTX_NUM; i++) {
            ASSERT_EQ(VERTEX_BRAM[i % CORE_NUM][i / CORE_NUM], bfs_res[i]);
        }
        printf("last_clk=%d\n", clk);
        printf("last_update_cycle=%d\n", last_update_cycle);
    }
}

int main(int argc,char* argv[])
{ 
    //testing::GTEST_FLAG(output) = "xml:"; //若要生成xml结果文件
    testing::InitGoogleTest(&argc,argv);
    RUN_ALL_TESTS();
    return 0;
}

#endif