#include<iostream>
#include<fstream>
#include<unordered_map>
#include<unordered_set>
#include<limits.h>
#include<vector>
#include <sys/stat.h>
using namespace std;

#define BLOCKSIZE 100
#define K 1000

#define NODES "nodes.txt"
#define EDGES "edges.txt"

struct point{
	long double x, y;
	string block_x_y;
	point(long double x, long double y, string block_x_y){
		this->x = x;
		this->y = y;
		this->block_x_y = block_x_y;
	}
};

struct edge{
	int u, v;
	long double length;
	edge(int u, int v, long double length){
		this->u = u;
		this->v = v;
		this->length = length;
	}
};

/*
	Function to write to file
	Parameters:
		1. nodeid_cordinates: nodeid -> (x, y, block_addr)
		2. nodes - array of internal nodes
		3. border_nodes - array of border nodes
		4. inedge - set containing internal edges
		5. outedge - set containing border edges
*/
void insert(string fileName, unordered_map<int, struct point*> &nodeid_cordinates, vector<int> &nodes, vector<int> &border_nodes, unordered_set<struct edge*> &inedge, unordered_set<struct edge*> &outedge ){
	
	// Write internal nodes to file
	int num = nodes.size();
	mkdir("data", 0777);
	ofstream ofp("data/"+fileName+".txt");
	ofp << "----------\n";
	int count = 0;
	int overflowNumber = 0;
	for(int i=0; i<num; i++){
		int id = nodes[i];
		long double x = nodeid_cordinates[id]->x;
		long double y = nodeid_cordinates[id]->y;
		ofp << setprecision (numeric_limits<double>::digits10 + 1) << id << "  " << "<" << x << ", " << y << ">\n"; 
		count++;
		if(count == BLOCKSIZE){
			overflowNumber++;
			count = 0;
			string overflowFile = fileName+"_"+to_string(overflowNumber)+".txt";
			ofp << "?? "+overflowFile << "\n";
			ofp.close();
			ofp.open("data/"+overflowFile);
			ofp << "\n?? "+fileName+".txt" << "\n";
		}
	}

	// write internal edges into file
	ofp << "##\n";
	for(auto i=inedge.begin(); i!=inedge.end(); i++){
		int u = (*i)->u;
		int v = (*i)->v;
		long double length = (*i)->length;
		ofp << setprecision (numeric_limits<double>::digits10 + 1) << u << " " << v << " <" << length << ">\n"; 
		count++;
		if(count == BLOCKSIZE){
			overflowNumber++;
			count = 0;
			string overflowFile = fileName+"_"+to_string(overflowNumber)+".txt";
			ofp << "?? "+overflowFile << "\n";
			ofp.close();
			ofp.open("data/"+overflowFile);
			ofp << "?? "+fileName+".txt" << "\n";
		}
	}

	// write border nodes to file
	ofp << "**\n";
	for(int i=0; i<border_nodes.size(); i++){
		int id = border_nodes[i];
		long double x = nodeid_cordinates[id]->x;
		long double y = nodeid_cordinates[id]->y;
		ofp << setprecision (numeric_limits<double>::digits10 + 1) << id << "  " << "<" << x << ", " << y << ">\n"; 
		count++;
		if(count == BLOCKSIZE){
			overflowNumber++;
			count = 0;
			string overflowFile = fileName+"_"+to_string(overflowNumber)+".txt";
			ofp << "?? "+overflowFile << "\n";
			ofp.close();
			ofp.open("data/"+overflowFile);
			ofp << "?? "+fileName+".txt" << "\n";
		}
	}

	// write border egdes to file
	ofp << "%%\n";
	int outEdgeCount = outedge.size();
	for(auto i=outedge.begin(); i!=outedge.end(); i++){
		int u = (*i)->u;
		int v = (*i)->v;
		long double length = (*i)->length;
		ofp << setprecision (numeric_limits<double>::digits10 + 1) << u << " " << v << " <" << length << ">\n"; 
		count++;
		outEdgeCount--;
		if(count == BLOCKSIZE && outEdgeCount > 0){
			overflowNumber++;
			count = 0;
			string overflowFile = fileName+"_"+to_string(overflowNumber)+".txt";
			ofp << "?? "+overflowFile << "\n";
			ofp.close();
			ofp.open("data/"+overflowFile);
			ofp << "?? "+fileName+".txt" << "\n";
		}
	}
	ofp << "----------\n";
	ofp.close();
}


/*
	function to create partitions of nodes 
	Parameters:
		1. nodeid_cordinates: nodeid -> (x, y, block_addr)
		2. block_node - block_addr -> (array of internal nodes)
		3. x_min 
		4. y_min
*/
void createPartition(unordered_map<int, struct point*> &nodeid_cordinates, unordered_map<string, vector<int> > &block_node, long double x_min, long double y_min){
	for(auto itr = nodeid_cordinates.begin(); itr != nodeid_cordinates.end(); itr++){
		int nodeid = itr->first;
		int x_rem = (itr->second->x - x_min) / K;
		int y_rem = (itr->second->y - y_min) / K;
		string s = to_string(x_rem) + "," + to_string(y_rem);
		itr->second->block_x_y = s;
		block_node[s].push_back(nodeid);
	}
}

/*
	Frunction to insert each partition one by one and call insert function for each partition
	Parameters:
		1. nodeid_cordinates: nodeid -> (x, y, block_addr)
		2. block_node:  block_addr -> (array of internal nodes)
		3. border_nodes: block_addr -> (array of boundary nodes)
		3. block_inedge:  block_addr -> (set of internal edges)
		4. block_outedge: block_addr -> (set of boundary edges)
*/
void insertIntoDataBlocks(unordered_map<int, struct point*> &nodeid_cordinates, unordered_map<string, vector<int> > &block_node, unordered_map<string, vector<int> > &border_nodes, unordered_map<string, unordered_set<struct edge*> > &block_inedge, unordered_map<string, unordered_set<struct edge*> > &block_outedge){
	for(auto itr = block_node.begin(); itr != block_node.end(); itr++){
		string block_x_y = itr->first;
		vector<int> nodes = itr->second;
		vector<int> border_nodes_list = border_nodes[block_x_y];
		unordered_set<struct edge*> inedge = block_inedge[block_x_y];
		unordered_set<struct edge*> outedge = block_outedge[block_x_y];
		insert(block_x_y, nodeid_cordinates, nodes, border_nodes_list, inedge, outedge);
	}
}


int main(){

	unordered_map<int, struct point*> nodeid_cordinates;
	unordered_map<string, vector<int> > block_node, border_nodes;
	unordered_map<string, unordered_set<struct edge*> > block_inedge, block_outedge;
	
	//Parse nodes file
	ifstream ifpn(NODES);
	if(!ifpn) return -1;
	int nodeid;
	long double x_min = INT_MAX, y_min = INT_MAX, x_max = INT_MIN, y_max = INT_MIN;
	while(ifpn >> nodeid){
		long double temp1, temp2;
		ifpn >> temp1 >> temp2;

		//map: nide_id -> (x, y, cell_id)
		struct point *p = new point(temp1, temp2, "");
		nodeid_cordinates[nodeid] = p;
		
		// find the x_min, y_min, x_max, y_max
		if(temp1 < x_min) x_min = temp1;
		else if(temp1 > x_max) x_max = temp1;

		if(temp2 < y_min) y_min = temp2;
		else if(temp2 > y_max) y_max = temp2;
	}
	ifpn.close();

	createPartition(nodeid_cordinates, block_node, x_min, y_min);

	//Parse edges file
	ifstream ifpe(EDGES);
	unordered_set<string> edgesDuplicates;
	if(!ifpe) return -1;
	int u, v;
	while(ifpe >> u){
		long double len;
		ifpe >> v;
		ifpe >> len;

		//to remove duplicate edges
		string temp = to_string(u)+"_"+to_string(v);
		if(edgesDuplicates.find(temp) != edgesDuplicates.end()) continue;
		edgesDuplicates.insert(temp);
		
		struct edge* e = new edge(u, v, len);
		string block_u = nodeid_cordinates[u]->block_x_y;
		string block_v = nodeid_cordinates[v]->block_x_y;
		if(block_u == block_v){
			block_inedge[block_u].insert(e);
		} else {
			block_outedge[block_u].insert(e);
			block_outedge[block_v].insert(e);
			border_nodes[block_u].push_back(v);
			border_nodes[block_v].push_back(u);
		}
	}
	edgesDuplicates.clear();
	ifpe.close();

	//insert into file
	insertIntoDataBlocks(nodeid_cordinates, block_node, border_nodes, block_inedge, block_outedge);

	return 0;
}
