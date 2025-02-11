#include <iostream>
#include <stdlib.h>
#include <iomanip>
#include <string.h>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stack>
#include <iomanip>
#include <chrono>

using namespace std;
using namespace std::chrono;

/////////////////// publisher ///////////////////
class publisher{
public:
    string name;
    float na_sales;
    float eu_sales;
    float others_sales;
};

/////////////////// Node ///////////////////
class Node{
public:
    publisher key;
    int color; // "Red"=1 or "Black"=0
    Node *parent, *left, *right;

    Node(publisher);
    ~Node(){}
    int get_color(){return color;}
    void set_color(int c){color = c;}
};

Node::Node(publisher key){
    this->key = key;
    this->parent = NULL;
    this->left = NULL;
    this->right = NULL;
}

/////////////////// BST-Tree ///////////////////
class BST_tree{
private:
    Node* root;
public:
    publisher* best_seller[3];
    stack<string> tree_deep_stack;

    Node* get_root();

    Node* BST_insert(Node* root, Node* ptr);
    void insertValue(const publisher &pub);
    void find_best_seller();

    BST_tree();
    ~BST_tree();
    Node* searchBST(Node* root, const string &name);
    publisher* searchPublisher(const string &name);
};

BST_tree::BST_tree(){
    this->root = NULL;
    this->best_seller[0] = NULL;
    this->best_seller[1] = NULL;
    this->best_seller[2] = NULL;
}

BST_tree::~BST_tree(){
}

Node* BST_tree::get_root(){
    return this->root;
}

Node* BST_tree::BST_insert(Node* root, Node* ptr){
    if(root == NULL){
        return ptr;
    }

    if(ptr->key.name == root->key.name){
        root->key.na_sales += ptr->key.na_sales;
        root->key.eu_sales += ptr->key.eu_sales;
        root->key.others_sales += ptr->key.others_sales;
        delete ptr;
        return root;
    }

    if(ptr->key.name < root->key.name){
        root->left = BST_insert(root->left, ptr);
        if(root->left != NULL) root->left->parent = root;
    }else{
        root->right = BST_insert(root->right, ptr);
        if(root->right != NULL) root->right->parent = root;
    }

    return root;
}

void BST_tree::insertValue(const publisher &pub){
    Node* newNode = new Node(pub);
    root = BST_insert(root, newNode);
}

void BST_tree::find_best_seller(){
    publisher* bestNA = new publisher{"None",0,0,0};
    publisher* bestEU = new publisher{"None",0,0,0};
    publisher* bestOther = new publisher{"None",0,0,0};

    stack<Node*> nodeStack;
    if(root)nodeStack.push(root);

    while(!nodeStack.empty()){
        Node* cur = nodeStack.top();
        nodeStack.pop();

        if(cur->key.na_sales > bestNA->na_sales){
            *bestNA = cur->key;
        }
        if(cur->key.eu_sales > bestEU->eu_sales){
            *bestEU = cur->key;
        }
        if(cur->key.others_sales > bestOther->others_sales){
            *bestOther = cur->key;
        }

        if (cur->right) nodeStack.push(cur->right);
        if (cur->left) nodeStack.push(cur->left);
    }

    this->best_seller[0] = bestNA;
    this->best_seller[1] = bestEU;
    this->best_seller[2] = bestOther;
}

Node* BST_tree::searchBST(Node* root, const string &name){
    if(!root) return NULL;
    if(root->key.name == name) return root;
    if(name < root->key.name) return searchBST(root->left, name);
    return searchBST(root->right,name);
}

publisher* BST_tree::searchPublisher(const string &name){
    Node* res = searchBST(this->root, name);
    if(res) return &res->key;
    return NULL;
}

void print_best_sellers(int year, publisher* temp_publisher[3]){
    cout<<fixed<<setprecision(2); 
    cout<<"End of the "<<to_string(year)<<" Year"<<endl;
    cout<<"Best seller in North America: "<<temp_publisher[0]->name <<" - "<<temp_publisher[0]->na_sales<<" million"<<endl;
    cout<<"Best seller in Europe: "<<temp_publisher[1]->name<<" - "<<temp_publisher[1]->eu_sales<<" million"<<endl;
    cout<<"Best seller rest of the World: "<<temp_publisher[2]->name << " - "<<temp_publisher[2]->others_sales<<" million"<<endl<<endl;
}

vector<vector<string>> read_csv(const string &filename){
    ifstream file(filename);
    vector<vector<string>> entries;

    string line;
    getline(file, line);

    while(getline(file, line)){
        vector<string> row;
        size_t start = 0;
        while(true){
            size_t pos = line.find(',', start);
            if(pos == string::npos) {
                row.push_back(line.substr(start));
                break;
            }else{
                row.push_back(line.substr(start, pos - start));
                start = pos + 1;
            }
        }
        if(row.size() == 7){
            entries.push_back(row);
        } 
    }

    file.close();
    return entries;
}

BST_tree generate_BST_tree_from_csv(string fname, int year){
    BST_tree temp_BSTtree;

    auto entries = read_csv(fname);
    for(const auto &row : entries){
       
        try{
            int row_year = stoi(row[2]);
            if(row_year <= year){ 
                publisher pub;
                pub.name = row[3];
                pub.na_sales = stof(row[4]);
                pub.eu_sales = stof(row[5]);
                pub.others_sales = stof(row[6]);

                temp_BSTtree.insertValue(pub);
            }
        }catch (const exception &e){
            cerr<<"error was encountered while processing the row."<<e.what()<<endl;
        }
    }

    return temp_BSTtree;
}

int main(int argc, char* argv[]){
    if(argc < 2){
        cerr<<"provide CSV filename"<<endl;
        return EXIT_FAILURE;
    }

    string fname = argv[1];

    vector<int> milestones = {1990, 2000, 2010, 2020};
    BST_tree BSTtree; 

    auto total_insertion_start = high_resolution_clock::now();

    for(int year : milestones){
        BSTtree = generate_BST_tree_from_csv(fname, year); 
        BSTtree.find_best_seller();
        print_best_sellers(year, BSTtree.best_seller);
    }

    auto total_insertion_end = high_resolution_clock::now();
    auto total_insertion_duration = duration_cast<microseconds>(total_insertion_end - total_insertion_start).count();

    cout<<"Time taken to insert all data into BST: "<<total_insertion_duration<<" µs"<<endl;

    vector<string> publishers;
    auto entries = read_csv(fname);
    for(const auto &row : entries){
        publishers.push_back(row[3]);
    }

    sort(publishers.begin(), publishers.end(), [](const string &a, const string &b){
        return a.length() == b.length() ? a < b : a.length() < b.length();
    });

    publishers.erase(unique(publishers.begin(), publishers.end()), publishers.end());

    srand(time(0));

    int scount = 50;
    long long bst_tot_stime = 0;
    int i = 0;

    do{
        string search_name = publishers[rand() % publishers.size()];
        auto bst_sstart = high_resolution_clock::now();
        BSTtree.searchPublisher(search_name);
        auto bst_srend = high_resolution_clock::now();
        bst_tot_stime += duration_cast<nanoseconds>( bst_srend- bst_sstart).count();
        i++;
    }while (i < scount);

    double bst_avg_stime = (double)bst_tot_stime / scount;

    cout<<"Average time for 50 random searches: "<<bst_avg_stime<<" ns"<<endl;

    return EXIT_SUCCESS;
}
