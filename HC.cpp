#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <fstream>
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

class HCsolution {
private:
    int Fnum, Cnum;         // 工厂数量，顾客数量
    Factility* FList;       // 工厂列表
    Customer* CList;        // 顾客列表
    int** assignmentMap; // 对应工厂和顾客开销的二维表

public:
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

    // 局部搜索法
    int HCrun() {
        initBestSolution();
        srand((int)time(0));
        int flag = 0;
        int bestSolution = evaluate(FList, CList);
        // cout << "当前最好：" << bestSolution << endl;
        Factility* newFList = new Factility[Fnum];
        Customer* newCList = new Customer[Cnum];
        while(flag < 10000) {
            for (int i = 0; i < Fnum; i++)
                newFList[i] = FList[i];
            for (int i = 0; i < Cnum; i++)
                newCList[i] = CList[i];
            // 随机寻找多个个顾客更换其对应的工厂
            int count = random(10);
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
                flag = 0;
            }
            flag++;
        }
        return bestSolution;
    }
};

int main() {
    HCsolution solution;
    for (int i = 1; i <= 71; i++) {
        int start=clock();
        string file = "./Instances/p" + to_string(i);
        fstream f(file);
        solution.input(f);
        int result = solution.HCrun();
        int finish=clock();
        double totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
        cout << "p" << i << ": " << result << "\t"
             << "运行时间: " << totaltime << "秒" << endl;
        solution.clear();
    }
    // solution.HCrun();
}