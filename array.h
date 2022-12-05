#ifndef _ARRAY_H
#define _ARRAY_H
#endif // _ARRAY_H

#include <cstring>
#include <iostream>
#include <unordered_map>
using namespace std;

class Array_Doubleline{
public:
    int *array;
    int *index_in_array;
    int array_size;
    int array_capacity;


public:
    Array_Doubleline(){
        array = NULL;
        index_in_array = NULL;
        array_size = 0;
        array_capacity = 0;
    }

    Array_Doubleline(int capacity){
        array_capacity = capacity + 1;
        array = new int[array_capacity];
        index_in_array = new int[array_capacity];
        memset(array, -1, sizeof(int) * array_capacity);
        memset(index_in_array, -1, sizeof(int) * array_capacity);
        array_size = 0;
    }
	void initsize(int capacity){
        array_capacity = capacity + 1;
        array = new int[array_capacity];
        index_in_array = new int[array_capacity];
        memset(array, -1, sizeof(int) * array_capacity);
        memset(index_in_array, -1, sizeof(int) * array_capacity);
        array_size = 0;
    }

    ~Array_Doubleline(){
        delete[] array;
        array = NULL;
        delete[] index_in_array;
        index_in_array = NULL;
        array_size = 0;
        array_capacity = 0;
    }

    void clear(){
        //for(int i = 0; i < array_size; i++)
            //index_in_array[array[i]] = -1;
        memset(index_in_array, -1, sizeof(int) * array_size);
        //for(int i = 0; i < array_capacity; i++)
            //index_in_array[i] = 0;
        array_size = 0;
    }

    void insertElement(int e){
		array[array_size] = e;
        index_in_array[e] = array_size;
        array_size++;
    }

    void deleteElement(int e){
        array_size--;
        int last_e = array[array_size];
        int e_pos = index_in_array[e];
        array[e_pos] = last_e;
        index_in_array[last_e] = e_pos;
        index_in_array[e] = -1;
    }

    void deleteElementAt(int pos){
        array_size--;
        int last_e = array[array_size];
        int e = array[pos];
        array[pos] = last_e;
        index_in_array[last_e] = pos;
        index_in_array[e] = -1;
    }

    void swapDeleteAt(int pos){
        array_size--;
        int last_e = array[array_size];
        int e = array[pos];
        array[pos] = last_e;
        index_in_array[last_e] = pos;
	array[array_size] = e;
        index_in_array[e] = array_size;
    }

    void swapDelete(int e){
        array_size--;
        int last_e = array[array_size];
        int e_pos = index_in_array[e];
        array[e_pos] = last_e;
        index_in_array[last_e] = e_pos;
		array[array_size] = e;
        index_in_array[e] = array_size;
    }

    void deleteTo(int newsize){
        array_size = newsize;
    }

    void popORrecoverN(int new_size){
	array_size = new_size;
    }

    int indexOf(int e){
        return index_in_array[e];
    }

    int elementAt(int pos){
        return array[pos];
    }

    bool isInArray(int e){
        if(index_in_array[e] == -1 || index_in_array[e] >= array_size)
            return false;
        else
            return true;
    }

    int size(){
        return array_size;
    }

    int begin(){
        return 0;
    }

    int end(){
        return array_size;
    }

    int randElement(){
        return array[rand() % array_size];
    }
    int shuffle(int nb){
    	nb/=2;
    	if(nb==0) nb=size()/2;
		for(int i=0;i<nb;i++){
			int a = rand()%size();
			int b= rand()%size();
			if(a==b){
				i--;continue;
			}
			
			int temp = this->elementAt(a);
					
			array[a] = array[b];
			index_in_array[ array[a] ] = a;
			array[b] = temp;
			index_in_array[ array[b] ] = b;
			
		}
    	
	}
};


class Array_DoubleHash{
public:
    int *array;
	unordered_map<int,int> index_in_array;
     
    int array_capacity;
    int array_size;
    
    Array_DoubleHash(){
        array = NULL;
        array_size =  0;
        array_capacity = 0;
    }

    Array_DoubleHash(int capacity){
        array_capacity = capacity + 1;
        array = new int[array_capacity];
        memset(array, -1, sizeof(int) * array_capacity);
        array_size =  0;
    }
    void initsize(int capacity){
        array_capacity = capacity + 1;
        array = new int[array_capacity];
        memset(array, -1, sizeof(int) * array_capacity);
        array_size =  0;
    }

    ~Array_DoubleHash(){
	    delete[] array;
        array = NULL;
        array_size =  0;
        array_capacity = 0;
    }

    void clear(){
        index_in_array.clear();
        array_size = 0;
    }

    void insertElement(int e){
	array[array_size] = e;
        index_in_array[e] = array_size;
        array_size++; 
    }

    void deleteElement(int e){
        array_size--;
        
        int last_e = array[array_size];
        int e_pos = index_in_array[e];
        array[e_pos] = last_e;
        index_in_array[last_e] = e_pos;
        index_in_array[e] = -1;
    }

    void deleteElementAt(int pos){
        array_size--;
        
        int last_e = array[array_size];
        int e = array[pos];
        array[pos] = last_e;
        index_in_array[last_e] = pos;
        index_in_array[e] = -1;
    }

	void swapDeleteAt(int pos){
        array_size--;
        
        int last_e = array[array_size];
        int e = array[pos];
        array[pos] = last_e;
        index_in_array[last_e] = pos;
		array[array_size] = e;
        index_in_array[e] = array_size;
	}
 
	void swapDelete(int e){
        array_size--;
       
        int last_e = array[array_size];
 		        
        int e_pos = index_in_array[e];
        array[e_pos] = last_e;
        index_in_array[last_e] = e_pos;
		array[array_size] = e;
        index_in_array[e] = array_size;
	}

	void popORrecoverN(int new_size){
		array_size = new_size;
	}

    int indexOf(int e){
        return index_in_array[e];
    }

    int elementAt(int pos){
        return array[pos];
    }

    bool isInArray(int e){
        if(index_in_array.find(e) == index_in_array.end() || index_in_array[e] >= array_size)
            return false;
        else
            return true;
    }
	
	void deleteTo(int newsize){
        array_size = newsize;
        
	}
	
    int size(){
        return array_size;
    }
    
    int begin(){
        return 0;
    }

    int end(){
        return array_size;
    }
    int cap(){
        return array_capacity;
    }
    int randElement(){
        return array[rand() % array_size];
    }
    
    int shuffle(int nb){
    	nb/=2;
    	if(nb==0) nb=size()/2;
		for(int i=0;i<nb;i++){
			int a = rand()%size();
			int b= rand()%size();
			if(a==b){
				i--;continue;
			}
			
			int temp = this->elementAt(a);
					
			array[a] = array[b];
			index_in_array[ array[a] ] = a;
			array[b] = temp;
			index_in_array[ array[b] ] = b;
			
		}
    	
	}
};



class Array_Singleline
{
public:
    int *array;
    int array_size;
    int array_capacity;

public:
    Array_Singleline()
    {
        array = NULL;
        array_size = 0;
        array_capacity = 0;
    }

    Array_Singleline(int capacity)
    {
        array_capacity = capacity + 1;
        array = new int[array_capacity];
        memset(array, -1, sizeof(int) * capacity);
        array_size = 0;
    }

    ~Array_Singleline()
    {
        delete[] array;
        array = NULL;
        array_size = 0;
        array_capacity = 0;
    }

    void clear()
    {
        memset(array, -1, sizeof(int) * array_size);
        array_size = 0;
    }

    void insertElement(int e)
    {
        array[array_size] = e;
        array_size++;
    }

    void deleteElementAt(int pos)
    {
        array_size--;
        array[pos] = array[array_size];
    }

    int pop(){
	array_size--;		
    	return array[array_size];
    }


    int elementAt(int pos)
    {
        return array[pos];
    }

    int size()
    {
        return array_size;
    }

    int begin()
    {
        return 0;
    }

    int end()
    {
        return array_size;
    }

    int rand_element()
    {
        return array[rand() % array_size];
    }
};

