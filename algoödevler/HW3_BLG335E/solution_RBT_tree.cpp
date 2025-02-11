#include <iostream>
#include <stdlib.h>
#include <iomanip>
#include <string.h>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stack>
#include <chrono>
#include <random>

using namespace std;
using namespace std::chrono;

/////////////////// Publisher ///////////////////
class publisher 
{
public:
    string name;
    float na_sales;
    float eu_sales;
    float others_sales;
};

/////////////////// Node ///////////////////
class Node 
{
public:
    publisher key;
    int color; // "Red"=1 or "Black"=0
    Node *parent, *left, *right;

    Node(publisher);
    ~Node(){}
    int get_color(){return color;}
    void set_color(int c){color = c;}
};

Node::Node(publisher key) 
{
    this->key = key;
    this->color = 1; // "RED"; 
    this->parent = NULL;
    this->left = NULL;
    this->right = NULL;
}

/////////////////// RB-Tree ///////////////////
class RB_tree 
{
private:
    Node* root;
    void collect_nodes(Node* node, vector<Node*>& nodes);

public:    
    publisher* best_seller[3];    

    RB_tree();
    ~RB_tree();

    Node* get_root();
    Node* RB_insert(Node* root, Node* ptr);
    void insertValue(const publisher &pub);
    void RB_left_rotate(Node*);
    void RB_right_rotate(Node*);
    void RB_insert_fixup(Node*);
    void preorder();
    void preorder_helper(Node* node, int depth);  
    void find_best_seller();

    Node* searchRB(Node* root, const string &name);
    publisher* searchPublisher(const string &name);
};

void RB_tree::collect_nodes(Node* node, vector<Node*>& nodes){
    if(!node) return;
    collect_nodes(node->left, nodes);
    nodes.push_back(node);
    collect_nodes(node->right, nodes);
}

void RB_tree::find_best_seller(){
    vector<Node*> nodes;
    collect_nodes(this->root, nodes);

    publisher* bestNA = new publisher{"None", 0, 0, 0};
    publisher* bestEU = new publisher{"None", 0, 0, 0};
    publisher* bestOther = new publisher{"None", 0, 0, 0};

    for(const auto& node : nodes){
        if(node->key.na_sales > bestNA->na_sales){
            *bestNA = node->key;
        }
        if(node->key.eu_sales > bestEU->eu_sales){
            *bestEU = node->key;
        }
        if(node->key.others_sales > bestOther->others_sales){
            *bestOther = node->key;
        }
    }

    this->best_seller[0] = bestNA;
    this->best_seller[1] = bestEU;
    this->best_seller[2] = bestOther;
}

void print_best_sellers(int year, publisher* temp_publisher[3]){
    cout<<fixed << setprecision(2); 
    cout<<"End of the "<<to_string(year)<<" Year"<<endl;
    cout<<"Best seller in North America: "<<temp_publisher[0]->name<<" - "<<temp_publisher[0]->na_sales<<" million"<<endl;
    cout<<"Best seller in Europe: "<<temp_publisher[1]->name<<" - "<<temp_publisher[1]->eu_sales<<" million"<<endl;
    cout<<"Best seller rest of the World: "<<temp_publisher[2]->name<<" - "<<temp_publisher[2]->others_sales<<" million"<<endl<<endl;
}

RB_tree::RB_tree(){
    this->root = NULL;
    this->best_seller[0] = NULL;
    this->best_seller[1] = NULL;
    this->best_seller[2] = NULL;
}

RB_tree::~RB_tree() 
{
}

Node* RB_tree::get_root() 
{
    return this->root;
}

Node* RB_tree::RB_insert(Node* root, Node* ptr) 
{
    if(root == NULL) 
    {
        return ptr;
    }

    if(ptr->key.name == root->key.name) 
    {
        root->key.na_sales += ptr->key.na_sales;
        root->key.eu_sales += ptr->key.eu_sales;
        root->key.others_sales += ptr->key.others_sales;
        delete ptr;
        return NULL; 
    }

    if(ptr->key.name < root->key.name) 
    {
        Node* leftRes = RB_insert(root->left, ptr);
        if(leftRes == NULL) {
            return root; 
        }
        root->left = leftRes;
        root->left->parent = root;
    }else 
    {
        Node* rightRes = RB_insert(root->right, ptr);
        if(rightRes == NULL) {
            return root; 
        }
        root->right = rightRes;
        root->right->parent = root;
    }

    return root; 
}

void RB_tree::RB_left_rotate(Node *x) 
{
    Node* y = x->right;
    x->right = y->left;
    if(y->left != NULL)
        y->left->parent = x;

    y->parent = x->parent;
    if(x->parent == NULL){
        this->root = y;
    }else if(x == x->parent->left){
        x->parent->left = y;
    }else{
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
}

void RB_tree::RB_right_rotate(Node *y) 
{
    Node* x = y->left;
    y->left = x->right;
    if(x->right != NULL)
        x->right->parent = y;

    x->parent = y->parent;
    if(y->parent == NULL){
        this->root = x;
    }else if(y == y->parent->right){
        y->parent->right = x;
    }else{
        y->parent->left = x;
    }

    x->right = y;
    y->parent = x;
}

void RB_tree::RB_insert_fixup(Node *z) 
{
    while(z->parent != NULL && z->parent->color == 1) 
    {
        Node* gp = z->parent->parent;
        if(z->parent == gp->left) 
        {
            Node* y = gp->right; 
            if(y != NULL && y->color == 1) 
            {
                z->parent->color = 0;
                y->color = 0;
                gp->color = 1;
                z = gp;
            }else 
            {
                if(z == z->parent->right) 
                {
                    z = z->parent;
                    RB_left_rotate(z);
                }
                z->parent->color = 0;
                gp->color = 1;
                RB_right_rotate(gp);
            }
        }else 
        {
            Node* y = gp->left; 
            if(y != NULL && y->color == 1){
                z->parent->color = 0;
                y->color = 0;
                gp->color = 1;
                z = gp;
            } 
            else{
                if(z == z->parent->left){
                    z = z->parent;
                    RB_right_rotate(z);
                }
                z->parent->color = 0;
                gp->color = 1;
                RB_left_rotate(gp);
            }
        }
    }

    this->root->color = 0;
}

void RB_tree::insertValue(const publisher &pub){
    Node* newNode = new Node(pub);
    Node* insertedNode = RB_insert(this->root, newNode);

    if(insertedNode == NULL) {return;}

    if(this->root == NULL) 
    {
        this->root = insertedNode;
        this->root->color = 0;
        return;
    }

    if(newNode == this->root) 
    {
        newNode->color = 0; 
        return;
    }

    RB_insert_fixup(newNode);
}

void RB_tree::preorder_helper(Node* node, int depth) 
{
    if(!node) return;
    string color_str = (node->color == 1) ? "RED" : "BLACK";
    cout<<string(depth, '-')<<"("<<color_str<<") "<<node->key.name<<"\n";

    preorder_helper(node->left, depth+1);
    preorder_helper(node->right, depth+1);
}

void RB_tree::preorder() 
{
    preorder_helper(this->root, 0);
}

Node* RB_tree::searchRB(Node* root, const string &name) 
{
    if(!root) return NULL;
    if(root->key.name == name) return root;
    if(name < root->key.name) return searchRB(root->left, name);
    return searchRB(root->right, name);
}

publisher* RB_tree::searchPublisher(const string &name) 
{
    Node* n = searchRB(this->root, name);
    if(n) return &n->key;
    return NULL;
}

vector<vector<string>> read_csv(const string &filename) 
{
    ifstream file(filename);
    vector<vector<string>> entries;
    if(!file.is_open()){
        cerr<<"cant open the file: "<<filename<<endl;
        return entries;
    }

    string line;
    getline(file, line);

    while(getline(file, line)) 
    {
        vector<string> row;
        size_t start = 0;
        do{
            size_t pos = line.find(',', start);
            if(pos == string::npos){
                row.push_back(line.substr(start));
                break;
            }
             else{
                row.push_back(line.substr(start, pos - start));
                start = pos + 1;
            }
        }while (true);

        if(row.size() == 7) 
        {
            entries.push_back(row);
        }
    }
    file.close();
    return entries;
}

RB_tree generate_RBT_tree_from_csv(const string &file_name){
    RB_tree temp_RBtree;
    auto entries = read_csv(file_name);

    size_t i = 0;
    while(i < entries.size()){
        const auto &row = entries[i];
        if(row.size() < 7) continue;

        publisher pub;
        pub.name = row[3];
        pub.na_sales = stof(row[4]);
        pub.eu_sales = stof(row[5]);
        pub.others_sales = stof(row[6]);

        temp_RBtree.insertValue(pub);
        ++i;
    }

    return temp_RBtree;
}

RB_tree generate_RBT_tree_from_csv_year(const string &file_name, int year){
    RB_tree temp_RBtree;

    auto entries = read_csv(file_name);
    if(entries.empty()){
        cerr<<"no data found to process from CSV"<<endl;
        return temp_RBtree;
    }

    size_t i = 0;
    while(i < entries.size()){
        const auto &row = entries[i];
        if(row.size() < 7){
            ++i;
            continue;
        }

        try{
            int row_year = stoi(row[2]);
            if(row_year <= year){ 
                publisher pub;
                pub.name = row[3];
                pub.na_sales = stof(row[4]);
                pub.eu_sales = stof(row[5]);
                pub.others_sales = stof(row[6]);

                temp_RBtree.insertValue(pub);
            }
        } catch (const exception &e){
            cerr<<"error was encountered while processing the row."<<e.what()<<endl;
        }
        ++i;
    }

    return temp_RBtree;
}

int main(int argc, char* argv[]) 
{
    if(argc < 2) 
    {
        cerr<<"provide CSV filename"<<endl;
        return EXIT_FAILURE;
    }

    string fname = argv[1];
    vector<int> tpoint = {1990, 2000, 2010, 2020};

    auto start = high_resolution_clock::now();

    size_t m_index = 0;
    while(m_index < tpoint.size()){
        int year = tpoint[m_index];
        RB_tree RBtree = generate_RBT_tree_from_csv_year(fname, year);

        RBtree.find_best_seller();
        print_best_sellers(year, RBtree.best_seller);

        ++m_index;
    }

    RB_tree RBtree = generate_RBT_tree_from_csv(fname);
    RBtree.preorder();

    auto end = high_resolution_clock::now();
    auto insert_time = duration_cast<microseconds>(end - start).count();

    cout<<"Time taken to insert all data into RBT: "<<insert_time<<" \u00b5s"<<endl;

    
    auto entries = read_csv(fname);
    sort(entries.begin(), entries.end(), [](const vector<string> &a, const vector<string> &b){
        return a[3] < b[3];
    });

    srand(static_cast<unsigned>(time(0))); 
    vector<string> rand_stime;
    for(int i = 0; i < 50; ++i){
        int index = rand() % entries.size(); 
        rand_stime.push_back(entries[index][3]);
    }

    long long rbt_tot_stime = 0;
     size_t s_index = 0;

    do{
        const auto& search_name = rand_stime[s_index];
        auto rbt_sstart = high_resolution_clock::now();
        RBtree.searchPublisher(search_name);
        auto rbt_srend = high_resolution_clock::now();
        rbt_tot_stime += duration_cast<nanoseconds>(rbt_srend - rbt_sstart).count();

        ++s_index;
    }while(s_index < rand_stime.size());

    double rbt_avg_stime = (double)rbt_tot_stime / rand_stime.size();
    cout<<"Average time for 50 random searches: "<<rbt_avg_stime<<" ns"<<endl;

    return EXIT_SUCCESS;
}
