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
    int id{};
    vector<pair<int, float> > adj{};
};

struct path {
    vector<int> nodes{};
    float weight{};
    set<int> achieved{};
};

vector<node> G;
vector<int> V_S;
deque<path> tries;
vector<deque<pair<set<int>, float>>> V_d;
path *candidate = nullptr;


void init(const string &input) {
    ifstream fin(input);

    int n;
    fin>>n;
    G.resize(n);
    V_d.resize(n);
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
        G[i] = node { id=id, std::move(adj) };
    }

    fin>>n;
    V_S.resize(n);
    for (int i = 0; i < n; i++) {
        int id;
        fin>>id;
        V_S[i] = id;
    }

    tries.push_back(path { { V_S[0] }, 0, { V_S[0] } });

    V_d[V_S[0]].push_back({ { V_S[0] }, 0});

    fin.close();
}

void write(const string &output) {
    ofstream fout("output.txt");
    if (candidate) {
        fout<<candidate->nodes.size()<<endl;
        for (int each : candidate->nodes)
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
        auto selected_try_it = std::min_element(tries.begin(), tries.end() - 1,
                                     [](const path &p1, const path &p2)
                                     {return p1.weight < p2.weight;});
        auto selected_try = *selected_try_it;
        tries.erase(selected_try_it);

        if (candidate && candidate->weight <= selected_try.weight)
            break;
        else {
            for (auto each : G[selected_try.nodes.back()].adj) {
                if (std::find(selected_try.nodes.begin(), selected_try.nodes.end(), each.first) != selected_try.nodes.end())
                    continue;

                path this_path = path {
                    selected_try.nodes,
                    selected_try.weight + each.second,
                    selected_try.achieved
                };
                this_path.nodes.push_back(each.first);
                if (std::find(V_S.begin(), V_S.end(), each.first) != V_S.end()) {
                    this_path.achieved.insert(each.first);
                }

                if (V_d[each.first].empty())
                    V_d[each.first].emplace_back(this_path.achieved, this_path.weight);
                else {
                    float validation = numeric_limits<float>::max();
                    int same_index = -1;
                    int si = 0;
                    for (auto each_pair : V_d[each.first]) {
                        if (std::includes(each_pair.first.begin(), each_pair.first.end(),
                                          this_path.achieved.begin(), this_path.achieved.end())) {
                            validation = std::min(each_pair.second, validation);
                            if (each_pair.first.size() == this_path.achieved.size())
                                same_index = si;
                        }
                        si++;
                    }
                    if (validation <= this_path.weight)
                        continue;
                    else {
                        if (same_index == -1) {
                            V_d[each.first].emplace_back(this_path.achieved, this_path.weight);
                        }
                        else {
                            V_d[each.first][same_index].second = this_path.weight;
                        }
                    }
                }

                if (this_path.achieved.size() == V_S.size()) {
                    if (!candidate or candidate->weight > this_path.weight) {
                        delete candidate;
                        candidate = new path{this_path};
                    }
                }
                else
                    tries.push_back(this_path);
            }
        }
    }

    flog<<"Result calculation succeeded!"<<endl;

    write(string(argv[1]) + "output.txt");

    flog<<"Output wrote!"<<endl;

    return 0;
}