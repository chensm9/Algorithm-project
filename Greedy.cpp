#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <climits>
using namespace std;

#define random(x) (rand()%x)

struct Factility {
    int Capacity;        // 工厂容量
    int OpenCost;        // 打开工厂的开销
    int leftCapacity;    // 工厂剩余容量
};

struct Customer {
    int belongTo;
    int demand;
};

class GreedySolution {
private:
    int Fnum, Cnum;         // 工厂数量，顾客数量
    Factility* FList;       // 工厂列表
    Customer* CList;        // 顾客列表
    int** assignmentMap; // 对应工厂和顾客开销的二维表


public:
    GreedySolution() {
        FList = nullptr;
        CList = nullptr;
        assignmentMap = nullptr;
    }

    ~GreedySolution() {
        clear();
    }

    void input(istream& in) {
        in >> Fnum >> Cnum;
        FList = new Factility[Fnum];
        CList = new Customer[Cnum];
        // 输入相关信息
        for (int i = 0; i < Fnum; i++) {
            double Capacity, OpenCost;
            in >> Capacity >> OpenCost;
            FList[i].Capacity = Capacity;
            FList[i].OpenCost = OpenCost;
            FList[i].leftCapacity = Capacity;
        }

        for (int i = 0; i < Cnum; i++) {
            double demand;
            in >> demand;
            CList[i].demand = demand;
            CList[i].belongTo = -1;
        }

        assignmentMap = new int*[Fnum];
        for (int i = 0; i < Fnum; i++) {
            assignmentMap[i] = new int[Cnum];
            for (int j = 0; j < Cnum; j++) {
                double t;
                in >> t;
                assignmentMap[i][j] = t;
            }
        }

    }

    void clear() {
        if (FList != nullptr) {
            delete []FList;
            FList = nullptr;
        }
        if (CList != nullptr) {
            delete []CList;
            CList = nullptr;
        }
        if (assignmentMap != nullptr) {
            for (int i = 0; i < Fnum; i++)
                delete [] assignmentMap[i];
            delete [] assignmentMap;
            assignmentMap = nullptr;
        }
    }

    int evaluate(Factility* flist, Customer* clist) {
        int result = 0;
        for (int i = 0; i < Fnum; i++) {
            if (flist[i].leftCapacity < flist[i].Capacity)
                result += flist[i].OpenCost;
        }
        for (int i = 0; i < Cnum; i++) {
            result += assignmentMap[clist[i].belongTo][i];
        }
        return result;
    }

    void initBestSolution() {
        srand((int)time(0));
        for (int i = 0; i < Cnum; i++) {
            int k = random(Fnum);
            while(CList[i].demand > FList[k].leftCapacity) {
                k = random(Fnum);
            }
            CList[i].belongTo = k;
            FList[k].leftCapacity -= CList[i].demand;
        }
    }

    // 贪心算法流程
    int Greedyrun() {
        for (int i = 0; i < Cnum; i++) {
            int greedyVal = INT_MAX;
            int c_id = -1, f_id = -1;
            for (int j = 0; j < Cnum; j++) {
                if (CList[j].belongTo != -1)
                    continue;
                for (int k = 0; k < Fnum; k++) {
                    if (CList[j].demand <= FList[k].leftCapacity) {
                        int tempVal = assignmentMap[k][j] / CList[j].demand;
                        if (FList[k].leftCapacity == FList[k].Capacity)
                            tempVal += FList[k].OpenCost / CList[j].demand;
                        if (greedyVal > tempVal) {
                            greedyVal = tempVal;
                            c_id = j;
                            f_id = k;
                        }
                    }
                }
            }
            FList[f_id].leftCapacity -= CList[c_id].demand;
            CList[c_id].belongTo = f_id;
        }
        int bestSolution = evaluate(FList, CList);
        cout << bestSolution << endl;
        for (int i = 0; i < Fnum; i++) {
            if (FList[i].leftCapacity == FList[i].Capacity)
                cout << "0 ";
            else
                cout << "1 ";
        }
        cout << endl;
        for (int i = 0; i < Cnum; i++) {
            cout << CList[i].belongTo+1 << " ";
        }
        cout << endl;
        return bestSolution;
    }
};

int main() {
    GreedySolution solution;
    for (int i = 1; i <= 71; i++) {
        int start=clock();
        string file = "./Instances/p" + to_string(i);
        fstream f(file);
        solution.input(f);
        cout << "p" << i <<  "的运行结果如下：" << endl;
        int result = solution.Greedyrun();
        int finish=clock();
        double totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
        // cout << "|p" << i << "|" << result << "|" << "" << totaltime << "|" << endl;
        solution.clear();
    }
    // fstream f("./Instances/p1");
    // solution.input(f);
    // solution.input(cin);
    // int result =  solution.Greedyrun();
    // cout << result << endl;
}