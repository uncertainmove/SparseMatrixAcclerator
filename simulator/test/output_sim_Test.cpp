#include "gtest/gtest.h"
#include "Accelerator.h"

#include <iostream>
#include <chrono>

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

class output_Test : public testing::Test {
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
    void * xmalloc (size_t sz)
    {
        void * out;
        out = malloc (sz);
        if (!out) {
            fprintf(stderr, "Out of memory trying to allocate %zu byte(s)\n", sz);
            abort ();
        }
        return out;
    }
    static int compare_doubles(const void* a, const void* b) {
	    double aa = *(const double*)a;
	    double bb = *(const double*)b;
	    return (aa < bb) ? -1 : (aa == bb) ? 0 : 1;
    }
    enum {s_minimum, s_firstquartile, s_median, s_thirdquartile, s_maximum, s_mean, s_std, s_LAST};
    void get_statistics(const double x[], int n, double r[s_LAST]) {
	    double temp;
	    int i;
	    /* Compute mean. */
	    temp = 0.0;
	    for (i = 0; i < n; ++i) temp += x[i];
	    temp /= n;
	    r[s_mean] = temp;
	    double mean = temp;
	    /* Compute std. dev. */
	    temp = 0;
	    for (i = 0; i < n; ++i) temp += (x[i] - mean) * (x[i] - mean);
	    temp /= n - 1;
	    r[s_std] = sqrt(temp);
	    /* Sort x. */
	    double* xx = (double*)xmalloc(n * sizeof(double));
	    memcpy(xx, x, n * sizeof(double));
	    qsort(xx, n, sizeof(double), compare_doubles);
	    /* Get order statistics. */
	    r[s_minimum] = xx[0];
	    r[s_firstquartile] = (xx[(n - 1) / 4] + xx[n / 4]) * .5;
	    r[s_median] = (xx[(n - 1) / 2] + xx[n / 2]) * .5;
	    r[s_thirdquartile] = (xx[n - 1 - (n - 1) / 4] + xx[n - 1 - n / 4]) * .5;
	    r[s_maximum] = xx[n - 1];
	    /* Clean up. */
	    free(xx);
    }
};

TEST_F(output_Test, test_VertexNotRead_Depth) {
    vector<int> root_vec(64, 0);
    vector<int> cycle_vec(64, 0);
    Initialize_Input_Graph("../data/edge_off.txt", "../data/edge_list.txt");
    Initialize_Offset_Uram(VTX_NUM);
    Initialize_Vertex_bram(ROOT_ID);
    Initialize_Edge_bram(VTX_NUM);
    Initialize_VERTEX_DEGREE(VTX_NUM);
    Initialize_Bitmap(ROOT_ID,VTX_NUM);
    FILE* fp = fopen("../data/graph500_output.txt", "r");
    for (int i = 0; i < 64; i++) {
        fscanf(fp, "%d %d", &root_vec[i], &cycle_vec[i]);
    }
    for (int idx = 0; idx < root_vec.size(); idx++) {
        vector<int> bfs_res(VTX_NUM, -1);
        ori_bfs(root_vec[idx], bfs_res);
        for (int i = 0; i < POWER_LAW_V_NUM; i++) {
            ASSERT_TRUE(bfs_res[VERTEX_NOT_READ[i]] <= 2);
        }
    }
    fclose(fp);
}

// test push mode in small graph
TEST_F(output_Test, test_Graph500_Output) {
    // initialize
    vector<int> root_vec(64, 0);
    vector<int> cycle_vec(64, 0);
    double bfs_times[64] = {0};
    double edge_counts[64] = {0};
    double validate_times[64] = {0};
    double construction_time = 0;
    double graph_generation = 9.168952;
    auto start = std::chrono::steady_clock::now();
    Initialize_Input_Graph("../data/edge_off.txt", "../data/edge_list.txt");
    Initialize_Offset_Uram(VTX_NUM);
    Initialize_Vertex_bram(ROOT_ID);
    Initialize_Edge_bram(VTX_NUM);
    Initialize_VERTEX_DEGREE(VTX_NUM);
    Initialize_Bitmap(ROOT_ID,VTX_NUM);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_second = end - start;
    double transform_time = 0.175108;
    construction_time = elapsed_second.count() + 0.175108;
    FILE* fp = fopen("../data/graph500_output.txt", "r");
    FILE* upload_file = fopen("../data/graph500_upload.txt", "w");
    // auto upload_file = stdout;
    for (int i = 0; i < 64; i++) {
        fscanf(fp, "%d %d", &root_vec[i], &cycle_vec[i]);
        bfs_times[i] = (double)cycle_vec[i] * 100 / 20 / 1000000000;
    }
    fprintf(upload_file, "graph_generation:               %.6f s\n", graph_generation);
    fprintf(upload_file, "construction_time:              %.6f s\n", construction_time);
    for (int idx = 0; idx < root_vec.size(); idx++) {
        // run simulator
        fprintf(upload_file, "Running BFS %d\n", idx);
        vector<int> bfs_res(VTX_NUM, -1);
        ori_bfs(root_vec[idx], bfs_res);
        edge_counts[idx] = 0;
        for (int i = 0; i < Ori_Edge_MEM.size(); i++) {
            if (bfs_res[Ori_Edge_MEM[i]] >= 0) {
                edge_counts[idx] += 1;
            }
        }
        ASSERT_EQ((int)edge_counts[idx] % 2, 0);
        edge_counts[idx] /= 2;
        // printf("root=%d, size=%d, %f\n", root_vec[idx], Ori_Edge_MEM.size(), edge_counts[idx]);
        fprintf(upload_file, "Time for BFS %d is %.5e\n", idx, bfs_times[idx]);
        fprintf(upload_file, "TEPS for BFS %d is %.5e\n", idx, edge_counts[idx] / bfs_times[idx]);
	    printf("bfs  time:                  %.6e\n", bfs_times[idx]);

        // validation
        fprintf(upload_file, "Validating BFS %d\n", idx);
        auto start = std::chrono::steady_clock::now();
        vector<int> bfs_res_2(VTX_NUM, -1);
        ori_bfs(root_vec[idx], bfs_res_2);
        for (int i = 0; i < VTX_NUM; i++) {
            ASSERT_EQ(bfs_res_2[i], bfs_res[i]);
        }
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_second = end - start;
        validate_times[idx] = elapsed_second.count();
        fprintf(upload_file, "Validate time for BFS %d is %f\n", idx, validate_times[idx]);
    }
    fprintf(upload_file, "SCALE:                          %d\n", 20);
    fprintf(upload_file, "edgefactor:                     %d\n", 16);
    fprintf(upload_file, "NBFS:                           %d\n", 64);
    fprintf(upload_file, "graph_generation:               %.6f\n", graph_generation);
    // fprintf(upload_file, "num_mpi_processes:              %d\n", 1);
    fprintf(upload_file, "construction_time:              %.6f\n", construction_time);

    double stats[s_LAST];
	get_statistics(bfs_times, 64, stats);
	fprintf(upload_file, "bfs  min_time:                  %.5e\n", stats[s_minimum]);
	fprintf(upload_file, "bfs  firstquartile_time:        %.5e\n", stats[s_firstquartile]);
	fprintf(upload_file, "bfs  median_time:               %.5e\n", stats[s_median]);
	fprintf(upload_file, "bfs  thirdquartile_time:        %.5e\n", stats[s_thirdquartile]);
	fprintf(upload_file, "bfs  max_time:                  %.5e\n", stats[s_maximum]);
	fprintf(upload_file, "bfs  mean_time:                 %.5e\n", stats[s_mean]);
	fprintf(upload_file, "bfs  stddev_time:               %.5e\n", stats[s_std]);

    get_statistics(edge_counts, 64, stats);
	fprintf(upload_file, "min_nedge:                      %.11g\n", stats[s_minimum]);
	fprintf(upload_file, "firstquartile_nedge:            %.11g\n", stats[s_firstquartile]);
	fprintf(upload_file, "median_nedge:                   %.11g\n", stats[s_median]);
	fprintf(upload_file, "thirdquartile_nedge:            %.11g\n", stats[s_thirdquartile]);
	fprintf(upload_file, "max_nedge:                      %.11g\n", stats[s_maximum]);
	fprintf(upload_file, "mean_nedge:                     %.11g\n", stats[s_mean]);
	fprintf(upload_file, "stddev_nedge:                   %.11g\n", stats[s_std]);

    double* secs_per_edge = (double*)xmalloc(64 * sizeof(double));
	for (int i = 0; i < 64; ++i) secs_per_edge[i] = bfs_times[i] / edge_counts[i];
	get_statistics(secs_per_edge, 64, stats);
	fprintf(upload_file, "bfs  min_TEPS:                  %g\n", 1. / stats[s_maximum]);
	fprintf(upload_file, "bfs  firstquartile_TEPS:        %g\n", 1. / stats[s_thirdquartile]);
	fprintf(upload_file, "bfs  median_TEPS:               %g\n", 1. / stats[s_median]);
	fprintf(upload_file, "bfs  thirdquartile_TEPS:        %g\n", 1. / stats[s_firstquartile]);
	fprintf(upload_file, "bfs  max_TEPS:                  %g\n", 1. / stats[s_minimum]);
	fprintf(upload_file, "bfs  harmonic_mean_TEPS:     !  %g\n", 1. / stats[s_mean]);
	fprintf(upload_file, "bfs  harmonic_stddev_TEPS:      %g\n", stats[s_std] / (stats[s_mean] * stats[s_mean] * sqrt(64 - 1)));

	get_statistics(validate_times, 64, stats);
	fprintf(upload_file, "bfs  min_validate:              %g\n", stats[s_minimum]);
	fprintf(upload_file, "bfs  firstquartile_validate:    %g\n", stats[s_firstquartile]);
	fprintf(upload_file, "bfs  median_validate:           %g\n", stats[s_median]);
	fprintf(upload_file, "bfs  thirdquartile_validate:    %g\n", stats[s_thirdquartile]);
	fprintf(upload_file, "bfs  max_validate:              %g\n", stats[s_maximum]);
	fprintf(upload_file, "bfs  mean_validate:             %g\n", stats[s_mean]);
	fprintf(upload_file, "bfs  stddev_validate:           %g\n", stats[s_std]);

    fclose(fp);
    fclose(upload_file);
}

int main(int argc,char* argv[])
{ 
    //testing::GTEST_FLAG(output) = "xml:"; //若要生成xml结果文件
    testing::InitGoogleTest(&argc,argv);
    RUN_ALL_TESTS();
    return 0;
}

#endif