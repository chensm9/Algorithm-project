#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <climits>
#include <cmath>
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

class SAsolution {
private:
    int Fnum, Cnum;         // 工厂数量，顾客数量
    Factility* FList;       // 工厂列表
    Customer* CList;        // 顾客列表
    int** assignmentMap; // 对应工厂和顾客开销的二维表
    // 模拟退火系数
    double T_start;     // 初温
    double T_end;       // 末温
    double DELTA;       // 降温系数
    double ILOOP;       // 内循环次数


public:
    SAsolution() {
        T_start = 100.0;
        T_end = 0.00001;
        DELTA = 0.99;
        ILOOP = 1000;
        FList = nullptr;
        CList = nullptr;
        assignmentMap = nullptr;
    }

    ~SAsolution() {
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
    void Greedyrun() {
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
    }

    int SArun() {
        // initBestSolution();
        Greedyrun();
        srand((int)time(0));
        int bestSolution = evaluate(FList, CList);
        // cout << "当前最好：" << bestSolution << endl;
        Factility* newFList = new Factility[Fnum];
        Customer* newCList = new Customer[Cnum];
        double t = T_start;
        while (t > T_end) {
            for (int kk = 0; kk < ILOOP; kk++) {
                for (int i = 0; i < Fnum; i++)
                    newFList[i] = FList[i];
                for (int i = 0; i < Cnum; i++)
                    newCList[i] = CList[i];
                // 随机寻找多个个顾客更换其对应的工厂
                int count = random(Fnum);
                while(count--) {
                    if (random(10) < 5) {
                        int c_id = random(Cnum);
                        int f_id = random(Fnum);
                        while(newCList[c_id].demand > newFList[f_id].leftCapacity) {
                            f_id = random(Fnum);
                        }
                        newFList[newCList[c_id].belongTo].leftCapacity += newCList[c_id].demand;
                        newCList[c_id].belongTo = f_id;
                        newFList[f_id].leftCapacity -= newCList[c_id].demand;
                    } else {
                        int c_id1 = random(Cnum);
                        int c_id2 = random(Cnum);
                        while(c_id1 == c_id2)
                            c_id2 = random(Cnum);
                        if (newCList[c_id1].demand + newFList[newCList[c_id2].belongTo].leftCapacity >= newCList[c_id2].demand &&
                            newCList[c_id2].demand + newFList[newCList[c_id1].belongTo].leftCapacity >= newCList[c_id1].demand) {
                                newFList[newCList[c_id1].belongTo].leftCapacity += newCList[c_id1].demand;
                                newFList[newCList[c_id1].belongTo].leftCapacity -= newCList[c_id2].demand;
                                newFList[newCList[c_id2].belongTo].leftCapacity += newCList[c_id2].demand;
                                newFList[newCList[c_id2].belongTo].leftCapacity -= newCList[c_id1].demand;
                                swap(newCList[c_id1].belongTo, newCList[c_id2].belongTo);
                            }
                    }
                }
                int newSolution = evaluate(newFList, newCList);
                if (newSolution < bestSolution) {
                    bestSolution = newSolution;
                    swap(CList, newCList);
                    swap(FList, newFList);
                    // cout << "当前最好: " << bestSolution << endl;
                } else {
                    double dE = newSolution - bestSolution;
                    double P_k = exp(dE*-1/t);
                    if (random(1000) * 0.001 < P_k) {
                        bestSolution = newSolution;
                        swap(CList, newCList);
                        swap(FList, newFList);
                    }
                }
            }
            t *= DELTA;
        }
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
    SAsolution solution;
    for (int i = 1; i <= 71; i++) {
        int start=clock();
        string file = "./Instances/p" + to_string(i);
        fstream f(file);
        solution.input(f);
        cout << "p" << i <<  "的运行结果如下：" << endl;
        int result = solution.SArun();
        int finish=clock();
        double totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
        // cout << "|p" << i << "|" << result << "|" << "" << totaltime << "s|" << endl;
        solution.clear();
    }

}