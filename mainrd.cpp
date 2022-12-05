#pragma once
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <iostream>
#include "array.h"
#include <time.h>
typedef  int CLtype; 
#define MAXK 30
#define MAXATTR 65
#define MAXINST 100000
#define STEPC 100
#define PRECISION 0.000001
int vmovecnt[MAXINST]; 
int CLPR;
int PRDEC = 10;
int Thalf1 = 1;
int Thalf2 = 2;

int RandSeed(int seed){
	srand(seed);
}
int Rand(int mod){
	return rand()%mod;
}
class InstancePosition{
public:
	double val[MAXATTR];
	int nb_attrval;
	
	void init(int nb_attrvalc){
		nb_attrval = nb_attrvalc;
		for(int i=0;i<nb_attrval;i++)
			val[i] = 0.0;
	}
	
	void avg(int nb){
		if(nb==0) return;
		for(int i=0;i<nb_attrval;i++)
			val[i] /= nb;
	}
};
class Instance{
public:
	Instance(){	}
	int inst_id;
	double val[MAXATTR];
	string str[MAXATTR];
	CLtype inst_class;
	CLtype LS_class;
	Array_DoubleHash MLlist;
	Array_DoubleHash CLlist;
	void setVal(int index, double valp){
		val[index] = valp;		
	}
	void addVal(double* cent, int nb_attrval){
		for(int i=0;i<nb_attrval;i++)
			cent[i] += val[i];
	}
	double computeSED(double* cent, int nb_attrval){
		double dist=0.0;
		for(int i=0;i<nb_attrval;i++)
			dist+=(cent[i] - val[i])*(cent[i] - val[i]);	
		return dist;
	}
}; 

class Node{
public:
	int node_id;
	Array_DoubleHash instList;
	Array_DoubleHash CLlist;	
}; 
class ClusteringProblem{
public:
	int nb_inst;
	int nb_node;
	int nb_attrval;
	int nb_attrstr;
	int nb_cluster;
	int nb_Link;
	Instance instList[MAXINST];
	Node nodeList[MAXINST];
	int instance2node[MAXINST];
	Array_DoubleHash clusters[MAXK];
	int node2cluster[MAXINST];
	InstancePosition centroid[MAXK];
	double sumSEDofClusters[MAXK];
	double sumSEDTotal;
	int movesorce[MAXINST];
	int updatemovesorce(int mode){}
	Array_Doubleline unsat_node;
	int node_CL_cnt[MAXINST];
	int getunsatnode(){
		for(int i=0;i<unsat_node.size();i++){
			int unode = unsat_node.elementAt(i);
			if(node_CL_cnt[unode] > 0){
				return unode;
			}
			else{
				unsat_node.deleteElementAt(i);
				i--;	
			}
		}
		return -1;
	}
	int getsatnode(int nb){
		return Rand(nb_node);
	}
	void setV(int*ar, int size, int v){
		for(int i=0;i<size;i++){
			ar[i] = v;
		}
	}
	
	int findBestToCluster(int id_node, double& orgsed, double& tosed){
			int CLcnt[MAXK];
			setV(CLcnt, nb_cluster, 0);
			Node& cnode = nodeList[id_node];
			for(int i=0;i<cnode.CLlist.size();i++){
				int clnode = cnode.CLlist.elementAt(i);
				CLcnt[ node2cluster[clnode] ] ++;
			}
			int cmin = CLcnt[node2cluster[id_node]];
			double sedorgbefore = sumSEDofClusters[node2cluster[id_node]];
			double sedorgafter = this->computeWCSSexcept(
			node2cluster[id_node],id_node);
			orgsed = sedorgafter;
			double bestimprove = 0.0;
			int objcluster = node2cluster[id_node];		
			double objsedtoafter = 0.0;
		if(Rand(100)<CLPR){
			for(int i=0;i<nb_cluster;i++){
				if(node2cluster[id_node] == i) continue;
				if( CLcnt[i] < cmin){
					cmin = CLcnt[i];
					objcluster = i;	
					double oo = sumSEDofClusters[ i];
					double ow = this->computeWCSSwith(i, id_node);
					double improve = (sedorgbefore - sedorgafter) +(oo-ow);
					objsedtoafter = ow;
					bestimprove = improve;									
				}
				else if(CLcnt[i] == cmin){
					 double oo = sumSEDofClusters[ i];
					 double ow = this->computeWCSSwith(i, id_node);
					 double improve = 
					    (sedorgbefore - sedorgafter) +(oo-ow);
					fitnesscnt++;
					 if(improve > bestimprove){
					     	objcluster = i;
					     	objsedtoafter = ow;
					     	bestimprove = improve;
					     	cmin=CLcnt[i];
				    }
				}
			}
		}else{		
			for(int ii=0;ii<nb_cluster;ii++){
				int i = ii;
				if(node2cluster[id_node] == i) continue;
				double oo = sumSEDofClusters[ i];
				double ow = this->computeWCSSwith(i, id_node);
				double improve = (sedorgbefore - sedorgafter) +(oo-ow);
				fitnesscnt++;			 
				if(improve > bestimprove){
					objcluster = i;
					objsedtoafter = ow;
					bestimprove = improve;
					cmin=CLcnt[i];	     	
				}
			}
		}
		tosed = objsedtoafter;
		return objcluster;
	}
	
	void moveto(int id_node, int fromc, int toc, double orgsed, double tosed, int prin = 0 ){
		clusters[fromc].deleteElement(id_node);
		clusters[toc].insertElement(id_node);
		sumSEDTotal = sumSEDTotal - sumSEDofClusters[fromc] - sumSEDofClusters[toc] + orgsed + tosed;
		sumSEDofClusters[fromc] = orgsed;
		sumSEDofClusters[toc] = tosed;		
		node2cluster[id_node] = toc;
		Node& cnode = nodeList[id_node];
			int clmark=0;
			for(int i=0;i<cnode.CLlist.size();i++){
				int clnode = cnode.CLlist.elementAt(i);
				if(toc == node2cluster[clnode]){
					clmark ++;
					if( unsat_node.isInArray( clnode) ==false)
						unsat_node.insertElement(clnode);
					node_CL_cnt[clnode]++;
				}
				else if(fromc == node2cluster[clnode]){
					node_CL_cnt[clnode]--;
				}				
			}
			node_CL_cnt[id_node ] = clmark;
			if(clmark>0 && unsat_node.isInArray(id_node) == false)
				unsat_node.insertElement(id_node);
	}
	void moveto_simple(int id_node, int fromc, int toc){	
		clusters[fromc].deleteElement(id_node);
		clusters[toc].insertElement(id_node);
		node2cluster[id_node] = toc;
	}
	void move_all_unsat(int max_move){
		int cnt=0;		
		while(unsat_node.size()>0){
			if(this->node_CL_cnt[unsat_node.elementAt(0)] ==0){
				unsat_node.deleteElementAt(0);
				continue;
			}
			else{
			         //if(1){	
				int id_node = unsat_node.elementAt(0);
				int fromc =  this->node2cluster[id_node];
				int toc = Rand(nb_cluster);
				
				while(fromc==toc ){
					toc =  Rand(nb_cluster);
				}
				moveto_simple(id_node,fromc,toc);
			         // }
			          unsat_node.deleteElementAt(0);
			          cnt++;
			}
			if(cnt>max_move)
			        break;
		}		
		updateSED_CLcnt();
	}	
	void move_one_random(int nb){	
	while(nb){	
		int id_node = Rand(nb_node);
		int fromc =  this->node2cluster[id_node];
		int toc = Rand(nb_cluster);
				
		while(fromc==toc ){
			toc =  Rand(nb_cluster);
		}
		moveto_simple(id_node,fromc,toc);
		updateSED_CLcnt();
		nb--;
	}
	}	
	
	void updateCLcnt(int id_cluster){		
		for(int i=0;i<clusters[id_cluster].size();i++){
			int anode = clusters[id_cluster].elementAt(i);
			
			node_CL_cnt[ anode ] = 0;
		}		
		for(int i=0;i<clusters[id_cluster].size();i++){
			int anode = clusters[id_cluster].elementAt(i);
			Node& canode = nodeList[anode];				
			for(int j=i+1;j<clusters[id_cluster].size();j++){
				int bnode = clusters[id_cluster].elementAt(j);
				if( canode.CLlist.isInArray(bnode) == true ){
					node_CL_cnt[ anode ] ++;
					node_CL_cnt[ bnode ] ++;
				}	
			}			
		}
		return;
	}
	void constructUNSATNodeFromCnt(){
		unsat_node.clear();
		for(int i=0;i<nb_node;i++){		
			if(node_CL_cnt[i]>0){			
				unsat_node.insertElement( i );
			}
		}
	}	
	double bestgoal;
	int bestunsat;
	int fitnesscnt;
	void LS(int loopSuper=50000){
		long long movecnt = 0;
		long long moveUP 
			= loopSuper*nb_node*nb_cluster; 
		fitnesscnt = 0;
		bestgoal=1000000000*100000.0;
		bestunsat=nb_node*5000.0;
		int state_nochange_cnt=0;
		int loop=0;
		int move_steps =1;	
		while(1){
			int id_node = -1;
			int alterpick = Rand(2);  
			if(alterpick<5)
				id_node = getunsatnode();
			if(alterpick>=5 || id_node<0 )    //alterpick not used
				id_node = getsatnode(1);
			int bestcluster =0;
			if(  Rand(STEPC)<vmovecnt[id_node]){
				vmovecnt[id_node]=vmovecnt[id_node]-PRDEC;
				if(vmovecnt[id_node] > -1)
					goto LR1; 
			}	
			double orgsed, tosed;
			bestcluster = findBestToCluster(id_node,orgsed, tosed);
			if(bestcluster == node2cluster[id_node]){
				state_nochange_cnt++;
				vmovecnt[id_node] = STEPC;
			}
			else{ 
				vmovecnt[id_node] = 0;
				state_nochange_cnt=0;
				moveto(id_node, node2cluster[id_node], bestcluster, orgsed, tosed);
				movecnt++;
			}	
		   for(int i=0;i<nb_cluster;i++){
				       this->updateCLcnt(i);
		    }
LR1:  ;			
				loop++;
			int tttunsat = cntunsat_node_CL_cnt();
			updatetotalsed();
			if(tttunsat < this->bestunsat){
				this->bestunsat = tttunsat;
				this->bestgoal = this->sumSEDTotal;
				move_steps =1;
				writeLSclass();
			}
			else if(tttunsat == this->bestunsat ){
				if(this->sumSEDTotal*(1+PRECISION) < this->bestgoal){
					this->bestgoal = this->sumSEDTotal;
					writeLSclass();
					move_steps =1;
				}
			
			}				
			if(state_nochange_cnt>nb_node*Thalf1/Thalf2 && unsat_node.size()>0){
				move_all_unsat(1);   
				move_steps++;
				movecnt++;
			}
			else if( state_nochange_cnt>nb_node*Thalf1/Thalf2  && unsat_node.size() == 0 ){
			
				move_one_random(1);
				move_steps++;
				movecnt++;
			}
			unsat_node.shuffle(0);
			if(movecnt>moveUP )
			{
				break;
			}	
		}
	}
	
	void init(){
		sumSEDTotal=0.0;
		for(int i=0;i<MAXINST;i++){
			instance2node[i] = -1;
			node2cluster[i]=-1;
			movesorce[i] = 0;
		}
		for(int i=0;i<MAXK;i++){
			clusters[i].clear();
			sumSEDofClusters[i] = 0.0;
		}
	}
	
	void propagationConstr_BulidNode(int nb_constr){
		nb_node = 0;
		for(int i=0;i<nb_inst;i++){
			vector<int> instlist;
			instlist.clear();
			dfsML_Add(i,nb_node, instlist);
			
			if(instlist.size()>0){
				nodeList[nb_node].instList.initsize( instlist.size()+1);
				nodeList[nb_node].instList.clear();
				for(int j=0;j<instlist.size();j++){
					nodeList[nb_node].instList.insertElement(instlist[j]);
		        }
				nb_node++;
			}
		}
		for(int i=0;i<nb_node;i++){
			vector<int> temp;
			temp.clear();
			for(int j=0;j< nodeList[i].instList.size();j++){
				Instance& ti = instList[nodeList[i].instList.elementAt(j)];
				for(int k=0;k<ti.CLlist.size(); k++){
					int i2n = instance2node[ ti.CLlist.elementAt(k) ];
					temp.push_back(i2n);
				}	
			}		
			nodeList[i].CLlist.initsize(temp.size() );
			for(int j=0;j<temp.size();j++ ){
				if( nodeList[i].CLlist.isInArray(temp[j]) == false ){		
					nodeList[i].CLlist.insertElement(temp[j]);
				}
			}	
		}
		unsat_node.initsize(nb_node);
	}
	
	void dfsML_Add(int root, int nodeID, vector<int>& instlist){
		if( instance2node[root]  == -1 ){
			instance2node[root] = nodeID;
			instlist.push_back(root);
			for(int i=0;i<instList[root].MLlist.size();i++)
				dfsML_Add(instList[root].MLlist.elementAt(i), nodeID, instlist);
		}
		return;	
	}
	
	double computeWCSS(){
		fitnesscnt++;
		double sum=0.0;
		for(int i=0;i<nb_cluster;i++){
			sum+=computeWCSS(i);
		}
		sumSEDTotal=sum;
		return sum;
	}
	
	double computeWCSS(int id_cluster){
		computeWCSScenter(id_cluster);
		int inst_cnt = 0;
		double sum=0.0;
		for(int i=0;i<clusters[id_cluster].size();i++){
			Node& node = nodeList[clusters[id_cluster].elementAt(i)];
			for(int j=0; j< node.instList.size();j++){
				inst_cnt++;
				Instance& inst = instList[node.instList.elementAt(j) ];
				sum+=inst.computeSED(centroid[id_cluster].val, nb_attrval);
			}
		}
		sumSEDofClusters[id_cluster] = sum ;
		return sum ;
	}
	
	void computeWCSScenter(int id_cluster){
		centroid[id_cluster].init(nb_attrval);
		int inst_cnt = 0;
		for(int i=0;i<clusters[id_cluster].size();i++){
			Node& node = nodeList[clusters[id_cluster].elementAt(i)];			
			for(int j=0; j< node.instList.size();j++){	
				inst_cnt++;
				Instance& inst = instList[node.instList.elementAt(j)];
				inst.addVal(centroid[id_cluster].val, nb_attrval);
			}
		}
		centroid[id_cluster].avg(inst_cnt);
	}
	double computeWCSSexcept(int id_cluster, int id_node){
		InstancePosition newcent;
		computeWCSScenterexcept(newcent, id_cluster, id_node);
		double sum=0.0;
		int inst_cnt = 0;
		for(int i=0;i<clusters[id_cluster].size();i++){
			if(id_node == clusters[id_cluster].elementAt(i))
				continue;
			Node& node = nodeList[clusters[id_cluster].elementAt(i)];
			for(int j=0; j< node.instList.size();j++){
				inst_cnt++;
				Instance& inst = instList[node.instList.elementAt(j) ];
				sum+=inst.computeSED(newcent.val, nb_attrval);
			}
		}
		return sum;
	}
	void computeWCSScenterexcept(InstancePosition& newcent,int id_cluster, int id_node){
	    newcent.init(nb_attrval);
	    int inst_cnt = 0;
		for(int i=0;i<clusters[id_cluster].size();i++){
			if(id_node == clusters[id_cluster].elementAt(i))
				continue;
			Node& node = nodeList[clusters[id_cluster].elementAt(i)];
			for(int j=0; j< node.instList.size();j++){
				inst_cnt++;
				Instance& inst = instList[node.instList.elementAt(j)];
				inst.addVal(newcent.val, nb_attrval);
			}
		}
		newcent.avg(inst_cnt);	
	} 
	double computeWCSSwith(int id_cluster, int id_node){
		InstancePosition newcent;
		computeWCSScenterwith(newcent, id_cluster, id_node);
		double sum=0.0;
		int inst_cnt = 0;
		for(int i=0;i<clusters[id_cluster].size();i++){
			if(id_node == clusters[id_cluster].elementAt(i))
				continue;
			Node& node = nodeList[clusters[id_cluster].elementAt(i)];
			for(int j=0; j< node.instList.size();j++){
				inst_cnt++;
				Instance& inst = instList[node.instList.elementAt(j) ];
				sum+=inst.computeSED(newcent.val, nb_attrval);
			}
		}
		Node& node = nodeList[id_node];
		for(int j=0; j< node.instList.size();j++){
			inst_cnt++;	
			Instance& inst = instList[node.instList.elementAt(j) ];
			sum+=inst.computeSED(newcent.val, nb_attrval);
		}	
		return sum;
	}
	void computeWCSScenterwith(InstancePosition& newcent,int id_cluster, int id_node){
	    newcent.init(nb_attrval);
	    int inst_cnt = 0;
		for(int i=0;i<clusters[id_cluster].size();i++){
			if(id_node == clusters[id_cluster].elementAt(i))
				continue;
			Node& node = nodeList[clusters[id_cluster].elementAt(i)];
			for(int j=0; j< node.instList.size();j++){
				inst_cnt++;
				Instance& inst = instList[node.instList.elementAt(j)];
				inst.addVal(newcent.val, nb_attrval);
			}
		}		
		Node& node = nodeList[id_node];
		for(int j=0; j< node.instList.size();j++){
			inst_cnt++;
			Instance& inst = instList[node.instList.elementAt(j)];
			inst.addVal(newcent.val, nb_attrval);
		}		
		newcent.avg(inst_cnt);		
	}	
	void contructRandom(){
		for(int i=0;i<nb_cluster;i++){
			this->clusters[i].initsize( nb_node +1);
		}
		for(int i=0;i<nb_node;i++){
			int cc = Rand(nb_cluster);
			this->clusters[cc].insertElement(i);
			this->node2cluster[i] = cc;
		}
		updateSED_CLcnt();
	}
	void updateSED_CLcnt(){
		for(int i=0;i<nb_cluster;i++){			
			this->updateCLcnt(i);
		}
		constructUNSATNodeFromCnt();
		this->computeWCSS();
	}	
	int cntunsat_node_CL_cnt(){
		int sum=0;
		for(int i=0;i<nb_node;i++){
			sum+=this->node_CL_cnt[i];
		}
		return sum;
	}
	double updatetotalsed(){
		double sum=0.0;
		for(int i=0;i<nb_cluster;i++){
			sum+=this->sumSEDofClusters[i];
		}
		this->sumSEDTotal= sum;		
	}
	void writeLSclass(){
		for(int k=0;k<nb_cluster;k++){
			int id_cluster =k;
			for(int i=0;i<clusters[id_cluster].size();i++){
				Node& node = nodeList[clusters[id_cluster].elementAt(i)];
				for(int j=0; j< node.instList.size();j++){
					instList[ node.instList.elementAt(j) ].LS_class = k;
				}
			}
		}
	}	
	void writeResults(string fn){
		ofstream fo(fn);
		for(int i=0;i<nb_inst;i++){
			fo<<i<<"\t"<< instList[i].LS_class <<"\n";
		}
		fo.close();
	}	
	
	void readfileMLCL(string fn){
		ifstream fi;
		fi.open(fn,ios::in);	
			if (!fi){
				cout << "fail file" << endl;
				exit(1);
			}
			fi>>nb_inst;
			fi>>nb_attrval;
			fi>>nb_attrstr;
			fi>>nb_cluster;			
			for(int i=0;i<nb_inst;i++){
				double temp;
				for(int j=0;j<nb_attrval;j++){
					fi>>temp;
					instList[i].setVal(j,temp);
				}		
				CLtype ttt;
				fi>>ttt;	
				instList[i].inst_class = ttt;
			}		
			init();
		int LLnum = 0;
		fi>>LLnum;
		this->nb_Link = LLnum;
		int gencnt = nb_Link;	
		vector<int> tempML[MAXINST];
		vector<int> tempCL[MAXINST];	
		while(gencnt){
			int a,b,c;
			fi>>a>>b>>c;			
			if(c == 1){     
				if(find(tempML[a].begin(),tempML[a].end(), b) != tempML[a].end()){
					continue;
				}
				tempML[a].push_back(b);
				tempML[b].push_back(a);
			}
			else{  
				if(find(tempCL[a].begin(),tempCL[a].end(), b)!= tempCL[a].end()){
					continue;
				}	
				tempCL[a].push_back(b);
				tempCL[b].push_back(a);
			}
			gencnt--;
		}
			fi.close();	
		for(int i=0;i<nb_inst;i++){
			instList[i].MLlist.initsize( tempML[i].size()+1 );
			for(int j=0;j<tempML[i].size();j++){
				instList[i].MLlist.insertElement( tempML[i][j] );
			}
		}
		for(int i=0;i<nb_inst;i++){
			instList[i].CLlist.initsize( tempCL[i].size()+1 );
			for(int j=0;j<tempCL[i].size();j++){
				instList[i].CLlist.insertElement( tempCL[i][j] );
			}
		}		
	}
};
int main(int argc, char** argv) {
	int paracon = 100;
	int paraseed = 200;
	char retfile[100];
	
	if(argc>2){
		paracon = atoi(argv[2]);   // not used
		paraseed = atoi(argv[3]); //  seed
                                  CLPR = atoi(argv[4]);        //  p_h*100  
                                  if(argc>5){
                                             PRDEC = atoi(argv[5]);   // p_f*100    range: [0,100]
	                            if(argc>6){
		              Thalf1 = atoi(argv[6]);
		              Thalf2 = atoi(argv[7]);    //  \alpha_L = Thalf1/Thalf2  
	    	        }
		}
	}
	
	clock_t start, finish;
	start = clock();
	ClusteringProblem cp;
	
	cp.readfileMLCL(argv[1]);  //argv[1]  filename of dataset
	cp.init();
	RandSeed(paraseed);
	cp.propagationConstr_BulidNode(0);
	cp.contructRandom();
	cp.LS(200);   // 200 is iterPara
	finish = clock();
	cout<<" TIME=  "<<finish - start<<"\n";

	sprintf(retfile,"result-%i.txt",paraseed);
	string retstr(retfile);
	cp.writeResults(retstr);	
	return 0;
}
