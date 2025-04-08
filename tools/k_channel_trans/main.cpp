#include <bits/stdc++.h>

using namespace std;

int *Edge_Off_Mem_Array;
int *Edge_Mem_Array;
int *Degree_Array;
int Vertex_Num;
int Edge_Num;

void mem_init (const char *offset_filename, const char *edge_info_filename, int has_header) {
    FILE *p1, *p2;

    if ((p1 = fopen(offset_filename, "r")) == NULL) {
        cout << "[ERROR] Failed to open file " << offset_filename << endl;
        exit(-1);
    }
    cout << "[INFO] Open file " << offset_filename << endl;

    if ((p2 = fopen(edge_info_filename, "r")) == NULL) {
        cout << "[ERROR] Failed to open file " << edge_info_filename << endl;
        exit(-1);
    }
    cout << "[INFO] Open file " << edge_info_filename << endl;

    Edge_Off_Mem_Array  = (int *) calloc (Vertex_Num + 1, sizeof(int));
    Edge_Mem_Array      = (int *) calloc (Edge_Num, sizeof(int));

    cout << "[INFO] Vertex_num = " << Vertex_Num << ", Edge_num = " << Edge_Num << endl;
    if (has_header) {
        int tmp_v_num, tmp_e_num;
        fscanf (p1, "%d", &tmp_v_num);
        fscanf (p2, "%d", &tmp_e_num);
        assert (tmp_v_num == Vertex_Num);
        assert (tmp_e_num == Edge_Num);
    }

    cout << "[INFO] Reading offset." << endl;
    for (int i = 0; i < Vertex_Num + 1; i ++) {
        fscanf(p1, "%d", &Edge_Off_Mem_Array[i]);
    }
    cout << "[INFO] Reading offset complete." << endl;

    cout << "[INFO] Reading edge list." << endl;
    for (int i = 0; i < Edge_Num; i ++) {
        fscanf(p2, "%d", &Edge_Mem_Array[i]);
    }
    cout << "[INFO] Reading edge list complete." << endl;

    fclose(p1);
    fclose(p2);
}

void k_channel_trans (const char *offset_filename, const char *info_filename, const char *result_folder, int channel, int has_header) {
    FILE *offset_k_cn_fp, **info_k_cn_fp;
    char fname[200];

    info_k_cn_fp = (FILE **) calloc (channel, sizeof(FILE *));

    mem_init(offset_filename, info_filename, has_header);

    cout << "[INFO] Create k channel offset file." << endl;
    sprintf(fname, "%s/off.txt", result_folder);
    if ((offset_k_cn_fp = fopen(fname, "w")) == NULL) {
        cout << "[ERROR] Failed to create k channel offest file " << fname << endl;
        exit (-1);
    }
    cout << "[INFO] Create k channel offset file complete." << endl;

    cout << "[INFO] Create k channel info file." << endl;
    for (int i = 0; i < channel; i ++) {
        sprintf(fname, "%s/edge_%d.txt", result_folder, i);
        if ((info_k_cn_fp[i] = fopen(fname, "w")) == NULL) {
            cout << "[ERROR] Failed to create k channel edge file " << fname << endl;
            exit (-1);
        }
    }
    cout << "[INFO] Create k channel info file complete." << endl;

    int cu_off[channel];
    for (int i = 0; i < channel; i++) {
        cu_off[i] = 0;
    }
    for (int i = 0; i < Vertex_Num; i++) {
        int channel_id = i % 32 < 16 ? 0 : 1;
        // 节点的在所属channel的起始off为cu_off, 结束off为cu_off + ed_off - st_off
        fprintf(offset_k_cn_fp, "%d\n", cu_off[channel_id]);
        cu_off[channel_id] = cu_off[channel_id] + Edge_Off_Mem_Array[i + 1] - Edge_Off_Mem_Array[i];
        fprintf(offset_k_cn_fp, "%d\n", cu_off[channel_id]);
        for (int j = Edge_Off_Mem_Array[i]; j < Edge_Off_Mem_Array[i + 1]; j++) {
            fprintf(info_k_cn_fp[channel_id], "%d\n", Edge_Mem_Array[j]);
        }
    }

    fclose(offset_k_cn_fp);
    for (int i = 0; i < channel; i ++) {
        fclose(info_k_cn_fp[i]);
    }

    cout << "[INFO] Generate channel file complete." << endl;

}

int main(int argc, char **argv) {
    int channel;

    if (argc != 8) {
        cout << "[INFO] Parameter (offset_file, info_file, result_folder, channel_num, vertex_num, edge_num, has_header)." << endl;
        exit (-1);
    }
    channel = atoi (argv[4]);
    Vertex_Num = atoi (argv[5]);
    Edge_Num = atoi (argv[6]);
    int has_header = atoi (argv[7]);
    cout << "[INFO] channel: " << channel << ", vertex_num: " << Vertex_Num << ", edge_num: " << Edge_Num << endl;

    k_channel_trans (argv[1], argv[2], argv[3], channel, has_header);

}