#include <bits/stdc++.h>

using namespace std;

/*
typedef struct Node {
    int id;
    float value;
} Node ;

int compare (const void *node1, const void *node2) {
    Node *n1 = (Node *) node1;
    Node *n2 = (Node *) node2;
    // -1 代表不交换位置，1 代表交换位置
    if (n1 -> value < n2 -> value) return 1;
    else return -1;
}
*/

/*
void check_degree (int edge_degree) {
    edge_degree &= 0xffffe000;
    float *a = (float *)(&edge_degree);
    int b = (edge_degree >> 13) & 0x00003fff;
    b |= ((edge_degree >> 16) & 0x0000c000);
    if (*a - float16_to_float32(b) > 2) {
        printf ("32_Degree = %x-%f, 16_Degree = %x-%f\n", edge_degree, *a, b, float16_to_float32(b));
        exit (-1);
    }
}
*/

int main(int argc, char **argv) {
    FILE *p2, *p3;
    int vertex_num;
    double *vertex_p;
    
    if (argc != 4) {
        cout << "Usage: Parameter (vertex_num, ori_pr_file, result_file)." << endl;
        exit (-1);
    }

	if ((p2 = fopen(argv[2], "r")) == NULL) {
        cout << "[ERROR] Failed to open file " << argv[2] << endl;
        exit(-1);
    }
    cout << "[INFO] Open file " << argv[2] << endl;

    if ((p3 = fopen(argv[3], "r")) == NULL) {
        cout << "[ERROR] Failed to open file " << argv[3] << endl;
        exit(-1);
    }
    cout << "[INFO] Open file " << argv[3] << endl;

    vertex_num = atoi (argv[1]);

    cout << "[INFO] Vertex_num = " << vertex_num << endl;

    vertex_p = (double *) calloc (vertex_num, sizeof(double));

    for (int i = 0; i < vertex_num; ++ i) {
        int id;
        fscanf (p2, "%d %lf", &id, &vertex_p[i]);
    }
	fclose(p2);

    // check
    cout << "Start check." << endl;

    double *pr_result = (double *) calloc (vertex_num, sizeof(double));
    for (int i = 0; i < vertex_num; i ++) {
        int id;
        double va;
        fscanf (p3, "%d %lf", &id, &va);
		if(id < 0 || id > vertex_num) {
            cout << "[ERROR] Vertex id overflow, MAX_ID = " << vertex_num << ", MIN_ID = 0, Current = " << id << endl;
			exit(-1);
		}
        pr_result[id] = va;
    }
    fclose (p3);
/*
    float *pr_result = (float *) calloc (vertex_num, sizeof(float));
    for (int i = 0; i < vertex_num; i ++) {
        int id;
        int va;
        fscanf (p3, "%d %d", &id, &va);
		if(id < 0 || id > vertex_num) {
            cout << "[ERROR] Vertex id overflow, MAX_ID = " << vertex_num << ", MIN_ID = 0, Current = " << id << endl;
			exit(-1);
		}
        pr_result[id] = float16_to_float32(va);
    }
    fclose (p3);
*/

    int acc_bigger = 0;
    int ori_bigger = 0;

/*
    Node *ori_node = (Node *) calloc (vertex_num, sizeof(Node));
    Node *acc_node = (Node *) calloc (vertex_num, sizeof(Node));
    for (int i = 0; i < vertex_num; i ++) {
        ori_node[i].id = i;
        ori_node[i].value = vertex_p[i];
        acc_node[i].id = i;
        acc_node[i].value = pr_result[i];
    }

    // 从大到小排列
    qsort (ori_node, vertex_num, sizeof(Node), compare);
    qsort (acc_node, vertex_num, sizeof(Node), compare);

    int err = 0;
    for (int i = 0; i < vertex_num; i ++) {
        // if (ori_node[i].id != acc_node[i].id && ori_node[i].value - acc_node[i].value > 0.1) {
        if (ori_node[i].id != acc_node[i].id) {
            cout << "Ori_Id = " << ori_node[i].id << ", Ori_Value = " << ori_node[i].value << ", Acc_Id = " << acc_node[i].id << ", Acc_Value = " << acc_node[i].value << endl;
            err ++;
        }
    }
    cout << "Err = " << err << endl;
*/

    double sum_dif = 0;
    double max_dif = 0;
    double max_deviation = 0;
    int max_deviation_id = 0;
    int max_dif_id = 0;
    int err_count = 0;
    double sum = 0;
	for (int i = 0; i < vertex_num; ++ i) {
        double dif;
        sum += vertex_p[i];
        if (vertex_p[i] > pr_result[i]) {
            dif = vertex_p[i] - pr_result[i];
            max_dif_id = max_dif > dif ? max_dif_id : i;
            max_dif = max_dif > dif ? max_dif : dif;
            sum_dif += dif;
        } else {
            dif = pr_result[i] - vertex_p[i];
            max_dif_id = max_dif > dif ? max_dif_id : i;
            max_dif = max_dif > dif ? max_dif : dif;
            sum_dif += dif;
        }
        double deviation = dif / vertex_p[i];
        max_deviation = max_deviation > deviation ? max_deviation : deviation;
		if (deviation > 0.02) {
            err_count ++;
            if (err_count < 10) {
                cout << "Vertex_Id = " << i << ", ACC = " << pr_result[i] << ", RT = " << vertex_p[i] << endl;
            }
		}
		if (vertex_p[i] - pr_result[i] > 0) {
            ori_bigger ++;
        } else acc_bigger ++;
	}
    cout << "Deviation = " << sum_dif / sum << endl;
    cout << "Err = " << err_count << endl;
    cout << "Sum_Dif = " << sum_dif << endl;
    cout << "Max_Id = " << max_dif_id << ", H_Value = " << vertex_p[max_dif_id] << ", ACC_Value = " << pr_result[max_dif_id] << ", Max_Dif = " << max_dif << endl;
    cout << "Max_Deviation_Id = " << max_deviation_id << ", Max_Deviation = " << max_deviation << endl;
    cout << "Ori_Bigger = " << ori_bigger << ", Acc_Bigger = " << acc_bigger << endl;
    cout << "End check." << endl;
}