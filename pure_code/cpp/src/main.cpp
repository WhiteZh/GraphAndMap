//
// Created by John Bonjour on 7/14/23.
//

#include <fstream>
#include <vector>
#include <deque>
#include <optional>
#include <utility>
#include <limits>
#include <set>

using namespace std;

struct node {
    int id;
    vector<pair<int, float> > adj;

    node(int id, vector<pair<int, float> > adj) : id(id), adj(std::move(adj)) {}
};

vector<node> G;
set<int> V_S;

struct path {
    vector<int> nodes_v;
    set<int> nodes_s;
    float weight;
    set<int> achieved;

    path(vector<int> nv, set<int> ns, float w, set<int> as)
    : nodes_v(std::move(nv)), nodes_s(std::move(ns)), weight(w), achieved(std::move(as)) {}

    void add_node(int node, float weight) {
        this->nodes_v.push_back(node);
        this->nodes_s.insert(node);
        this->weight += weight;
        if (V_S.contains(node)) {
            this->achieved.insert(node);
        }
    }
};

deque<path*> tries;
//vector<deque<pair<set<int>, float>>> V_d;
path *candidate = nullptr;

template<typename Tp>
inline bool issubset(set<Tp>& s1, set<Tp>& s2) {
    for (auto &each : s1) {
        if (!s2.contains(each))
            return false;
    }
    return true;
}


void init(const string &input) {
    ifstream fin(input);

    int n;
    fin>>n;
    for (int i = 0; i < n; i++) {
        int id;
        fin>>id;
        int m;
        fin>>m;
        vector<pair<int, float> > adj;
        for (int j = 0; j < m; j++) {
            int adj_id;
            float weight;
            fin>>adj_id>>weight;
            adj.emplace_back(adj_id, weight);
        }
        G.emplace_back(id, std::move(adj));
//        V_d.emplace_back();
    }

    fin>>n;
    int start_node;
    fin>>start_node;
    V_S.insert(start_node);
    for (int i = 1; i < n; i++) {
        int id;
        fin>>id;
        V_S.insert(id);
    }

    tries.emplace_back(new path(vector<int>({ start_node }), set<int>({ start_node }), 0.0f, set<int>({ start_node })));

//    V_d[start_node].emplace_back(set<int>({ start_node }), 0.0f);

    fin.close();
}

void write(const string &output) {
    ofstream fout(output);
    if (candidate) {
        fout << candidate->nodes_v.size() << endl;
        for (int each : candidate->nodes_v)
            fout<<each<<"\n";
        fout<<candidate->weight<<endl;
    }
    else {
        fout<<"None"<<endl;
    }
    fout.close();
}

int main(int argc, char **argv) {
    ofstream flog(string(argv[1]) + "log.txt");

    flog<<"Program started!"<<endl;

    init(string(argv[1]) + "input.txt");

    flog<<"Initialization sucessful!"<<endl;

    while (!tries.empty()) {
        auto selected_try_it = std::min_element(tries.begin(), tries.end(),
                                     [](const path *p1, const path *p2)
                                     {return p1->weight < p2->weight;});
        auto selected_try = *selected_try_it;
        tries.erase(selected_try_it);

        if (candidate && candidate->weight <= selected_try->weight)
            break;
        else {
            for (auto &each : G[selected_try->nodes_v.back()].adj) {
                if (selected_try->nodes_s.contains(each.first))
                    continue;

                path* this_path = new path(selected_try->nodes_v, selected_try->nodes_s, selected_try->weight, selected_try->achieved);
                this_path->add_node(each.first, each.second);

//                if (V_d[each.first].empty())
//                    V_d[each.first].emplace_back(this_path->achieved, this_path->weight);
//                else {
//                    pair<set<int>, float> *same_index = nullptr;
//                    bool good_flag = true;
//                    for (auto &each_pair : V_d[each.first]) {
//                        if (this_path->achieved.size() <= each_pair.first.size()) {
//                            if (this_path->achieved.size() == each_pair.first.size())
//                                same_index = &each_pair;
//                            if (issubset(this_path->achieved, each_pair.first)) {
//                                if (this_path->weight > each_pair.second) {
//                                    good_flag = false;
//                                    break;
//                                }
//                            }
//                        }
//                        else if (issubset(each_pair.first, this_path->achieved)) {
//                            each_pair.second = std::min(each_pair.second, this_path->weight);
//                        }
//                    }
//                    if (good_flag) {
//                        if (same_index == nullptr) {
//                            V_d[each.first].emplace_back(this_path->achieved, this_path->weight);
//                        }
//                        else {
//                            (*same_index).second = this_path->weight;
//                        }
//                    }
//                    else
//                        continue;
//                }

                if (this_path->achieved.size() == V_S.size()) {
                    if (!candidate or candidate->weight > this_path->weight) {
                        delete candidate;
                        candidate = this_path;
                    }
                }
                else
                    tries.push_back(this_path);
            }
        }

        delete selected_try;
    }

    flog<<"Result calculation succeeded!"<<endl;

    write(string(argv[1]) + "output.txt");

    flog<<"Output wrote!"<<endl;

    return 0;
}