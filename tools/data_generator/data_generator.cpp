#include <bits/stdc++.h>
// 生成文件所需的图需要满足如下条件：
// 1. 节点需连续
// 2. 所有节点均需存在入边

using namespace std;

typedef struct Edge {
    int source_id;
    int target_id;
} Edge;

static int is_from_snap = 0; // default is not from snap.

int compare (const void *edge1, const void *edge2) {
    Edge *e1 = (Edge *) edge1;
    Edge *e2 = (Edge *) edge2;
    // -1 代表不交换位置，1 代表交换位置
    if (e1 -> target_id < e2 -> target_id) return -1;
    else if (e1 -> target_id == e2 -> target_id && e1 -> source_id <= e2 -> source_id) return -1;
    else return 1;
}

void data_generator(const char *filename, const char *offset_filename, const char *edge_info_filename, const int vertex_num_tmp, const int edge_num_tmp, const int directed) {

    FILE *data_fp, *offset_fp, *info_fp;
    struct Edge *edge_list;
    int next_edge_num;
    int vertex_num = vertex_num_tmp;
    int edge_num = edge_num_tmp;

    if ((data_fp = fopen (filename, "r")) == NULL) {
        cout << "[ERROR] Failed to open file " << filename <<endl;
        exit (-1);
    }
    cout << "[INFO] Open file " << filename << endl;

    if (directed) edge_list = (struct Edge *) calloc (vertex_num + edge_num, sizeof(Edge));
    else edge_list = (struct Edge *) calloc (vertex_num + edge_num * 2, sizeof(Edge));

    // 读入所有边
    if (is_from_snap) {
        char c_tmp[200];
        cout << "[INFO] SNAP Graph PreProcess." << endl;
        for (int i = 0; i < 4; i ++) {
            fscanf (data_fp, "%[^\n]%*c", c_tmp);
            cout << "[INFO] Delete Line: " << c_tmp << endl;
        }
        cout << "[INFO] SNAP Graph PreProcess Completed." << endl;
    }
    int max_vertex_id = 0;
    cout << "[INFO] Read Edge." << endl;
    for (int i = 0; i < edge_num; i ++) {
        fscanf (data_fp, "%d %d", &(edge_list[i].source_id), &(edge_list[i].target_id));
        max_vertex_id = max (max_vertex_id, edge_list[i].source_id);
        max_vertex_id = max (max_vertex_id, edge_list[i].target_id);
    }
    cout << "[INFO] Read Edge Completed." << endl;
    next_edge_num = edge_num;
    if (!directed) {
        cout << "[INFO] Undirected Graph, Expand Edge." << endl;
        for (int i = 0; i < edge_num; i ++) {
            edge_list[i + edge_num].source_id = edge_list[i].target_id;
            edge_list[i + edge_num].target_id = edge_list[i].source_id;
        }
        next_edge_num += edge_num;
        cout << "[INFO] Expand Edge Completed." << endl;
    }

    // 检查点信息
    if (max_vertex_id >= vertex_num) {
        cout << "[WARNING] max_vertex_id is bigger than vertex_num." << endl;
        vertex_num = max_vertex_id + 1;
        cout << "[INFO] Change vertex_num to " << vertex_num << endl;
    }
    
    // 为每个点增加额外入边
    for (int i = 0; i < vertex_num; i ++) {
        edge_list[next_edge_num].source_id = i;
        edge_list[next_edge_num].target_id = i;
        next_edge_num ++;
    }
    cout << "[INFO] Next_Edge_num = " << next_edge_num << endl;

    // 按 source_id 和 target_id 排序
    qsort (edge_list, next_edge_num, sizeof(Edge), compare);
    // sort (&edge_list[0], &edge_list[edge_num - 1], compare);

    if ((offset_fp = fopen (offset_filename, "w")) == NULL) {
        cout << "[ERROR] Failed to open file " << offset_filename << endl;
        exit (-1);
    }
    cout << "[INFO] Open file " << offset_filename << endl;

    if ((info_fp = fopen (edge_info_filename, "w")) == NULL) {
        cout << "[ERROR] Failed to open file " << edge_info_filename << endl;
        exit (-1);
    }
    cout << "[INFO] Open file " << edge_info_filename << endl;

    fprintf (offset_fp, "%d\n", vertex_num);
    fprintf (offset_fp, "%d\n", 0);

    fprintf (info_fp, "%d\n", next_edge_num);
    fprintf (info_fp, "%d\n", edge_list[0].source_id);

    int write_ptr = 1;
    for (int i = 1; i < next_edge_num; i ++) {
        if (edge_list[i].source_id < 0 && edge_list[i].source_id > vertex_num) {
            cout << "[ERROR] Source_Id = " << edge_list[i].source_id << endl;
            exit (-1);
        }
        if (edge_list[i].target_id < 0 && edge_list[i].target_id > vertex_num) {
            cout << "[ERROR] Target_Id = " << edge_list[i].target_id << endl;
            exit (-1);
        }

        fprintf (info_fp, "%d\n", edge_list[i].source_id);
        if (edge_list[i].target_id != edge_list[i - 1].target_id) {
            if (edge_list[i].target_id - edge_list[i - 1].target_id > 1) {
                cout << "[ERROR] Gap error between " << edge_list[i].target_id << " and " << edge_list[i - 1].target_id << endl;
                exit (-1);
            }

            if (write_ptr != edge_list[i].target_id) {
                cout << "[ERROR] Write_Ptr = " << write_ptr << ", Target_Id = " << edge_list[i].target_id << endl;
                exit (-1);
            }

            fprintf (offset_fp, "%d\n", i);
            write_ptr ++;
        }
    }
    // 写入最后节点的结束 offset
    fprintf (offset_fp, "%d\n", next_edge_num);

    fclose (offset_fp);
    fclose (info_fp);

}

int main(int argc, char **argv) {
    int vertex_num, edge_num, directed;

    if (argc != 8) {
        cout << "Usage: Parameter (data_file, offset_file, info_file, vertex_num, ori_edge_num, directed, is_from_snap)." << endl;
        cout << "Info: directed=1 means directed, directed=0 means undirected." << endl;
        exit(-1);
    }
    vertex_num = atoi (argv[4]);
    edge_num = atoi (argv[5]);
    directed = atoi (argv[6]);
    is_from_snap = atoi (argv[7]);

    cout << "Vertex_Num = " << vertex_num << ", Edge_num = " << edge_num << ", Directed = " << directed << endl;

    data_generator(argv[1], argv[2], argv[3], vertex_num, edge_num, directed);

    return 0;
}