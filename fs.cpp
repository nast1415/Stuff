#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cstdlib>


using namespace std;

//Functions, that helps us with format function
void create_update_block_with_int_data(string dir, int number, int data[], int size) {
	//Get block name
	char buffer[100];
	sprintf (buffer, "%d", number);
	string name = dir + "/block" + buffer;
	const char * block_name = name.c_str();
	
	//Create block with block_name and fill it with data
	FILE * block;
	block = fopen(block_name, "wb");
	fwrite(data, sizeof(int), size, block);
	fclose(block);
}

//We need this function just for tests
void read_data_from_block(int* data, const char* block_name, int size) {
	FILE * block;
	block = fopen (block_name, "rb");
	fread(data, sizeof(int), size , block);
	if (block) {
		fclose(block);
	}
}

void create_data_array(int data[], int size, int size_of_first_part, int filler) {
	data[0] = size_of_first_part + 1;

	for (int i = 1; i < size_of_first_part; i++) {
		data[i] = i + 1;
	}

	data[size_of_first_part] = size_of_first_part;
	data[size_of_first_part + 1] = size_of_first_part + 1;

	for (int i = size_of_first_part + 2; i < size; i++) {
		data[i] = filler;
	}
}

void format (const char* dir, int blocks_count) {
	
	//Get all information about block structure
	//cout << "Number of all blocks: " << blocks_count << endl;

	int number_of_info_blocks= ceil(double((blocks_count + 1) * sizeof(int)) / 4096);
	//cout << "Number of blocks for allocation table: " << number_of_info_blocks << endl;

	int number_of_ints_in_block = floor(4096 / sizeof(int));
	//cout << "Number of ints in one block: " << number_of_ints_in_block << endl;

	int size_of_last_info_block =number_of_ints_in_block - ceil(double(number_of_info_blocks * 4096 / sizeof(int) - (blocks_count + 1)));	
	//cout << "Number of ints in the last information block: " << size_of_last_info_block << endl; 

	
	
	if (number_of_info_blocks < 1022) { //CORRECT
		
		int information_data[number_of_ints_in_block] = {0, }; //Array size of one block, filled with nulls

		create_data_array(information_data, number_of_ints_in_block, number_of_info_blocks, -1);
		create_update_block_with_int_data(dir, 0, information_data, number_of_ints_in_block); //Create null block

	} else {
		
		int real_size = ceil(double((number_of_info_blocks + 2) * sizeof(int)) / 4096); //Real size shows us the real number of blocks we need to fill 
		// (null element for the root link + number_of_info_blocks elements + root element)
		int size_of_array = (real_size * number_of_ints_in_block); //How many blocks we need to use for first number_of_info_blocks elements of our allocation table

		cout << "Number of blocks used to fill the top of the table: " << real_size << endl;
		cout << "Real size of our big array: " << size_of_array << endl;

		int information_data[size_of_array] = {0, }; //Create  an array with size that divide by number of ints blocks

		create_data_array(information_data, size_of_array, number_of_info_blocks, -1);
		

		cout << "number_of_ints_in_block + 1 element of the big array: " << information_data[number_of_ints_in_block] << endl;

		//Create new array, based on our big information_data array and fill first real_size blocks

		int itog_data[number_of_ints_in_block] = {0, };
		for (int i = 0; i < real_size; i++) {
			int index = 0;
			for (int j = i * number_of_ints_in_block; j < (i + 1) * number_of_ints_in_block; j++) {
				itog_data[index] = information_data[j];
				index++;
			}
			create_update_block_with_int_data(dir, i, itog_data, number_of_ints_in_block);
		}
		
	}
	

	//Create blocks from real_size to block_size and fill them with nulls
	
	int null_data[number_of_ints_in_block] = {0, };
	int real_size = ceil(double(number_of_info_blocks * sizeof(int)) / 1047551); //Real size shows us the real number of blocks we need to fill 
		
	//Fill all blocks with 0
	for (int i = real_size; i < blocks_count; i++) {
		//create_update_block_with_int_data(dir, i, null_data, number_of_ints_in_block);
	}

	
	//Fill all alloc table blocks exept first real_size and last

	int alloc_table_data[number_of_ints_in_block] = {0, };
	for (int i = 0; i < number_of_ints_in_block; i++) {
		alloc_table_data[i] = -1;
	}
	
	for (int i = real_size; i < number_of_info_blocks - 1; i++) {
		create_update_block_with_int_data(dir, i, alloc_table_data, number_of_ints_in_block);
	}

	// Fill last alloc table block
	
	if (number_of_info_blocks != 1) {
		int last_alloc_table_data[number_of_ints_in_block] = {0, };
		for (int i = 0; i < size_of_last_info_block; i++) {
			last_alloc_table_data[i] = -1;
		}
		

		create_update_block_with_int_data(dir, number_of_info_blocks - 1, last_alloc_table_data, size_of_last_info_block);
	}
	
}

void find_directory_by_name(string dir) {

}

void create_a_directory_in_the_root(string dir, string directory_name) {
	//int array 
}

void mkdir(string dir, string target_dir) {
	//string 
}

/*void import (string local_file, string target_file) {

}

void export (string target_file, string local_file) {

}

void ls(string dir, string target_dir) {

}
*/

int main (int argc, char* argv[])
{
    /*int data[100];
    for (int i = 0; i < 100; i++) {
    	data[i] = i + 1;
    }

    

    //create_update_block_with_int_data("/home/anastasi/Documents/AU/FS/test_fs", 0, data, 100);
    */
	format("/home/anastasi/Documents/AU/FS/test_fs", 4106);

	cout << endl;
    cout << "Return null block: " << endl;
    int size = floor(4096 / sizeof(int));
    int data_null_res[size] = {5, };
    read_data_from_block(data_null_res, "/home/anastasi/Documents/AU/FS/test_fs/block0", size);

    for (int i = 0; i < size; i++){
    	cout << data_null_res[i] << " ";
    }

    cout << endl;
    cout << "Return first block: " << endl;
    int data_first_res[size] = {5, };
    read_data_from_block(data_first_res, "/home/anastasi/Documents/AU/FS/test_fs/block1", size);

    for (int i = 0; i < size; i++){
    	cout << data_first_res[i] << " ";
    }


	cout << endl;
    cout << "Return other block: " << endl;
    int data_last_res[size] = {5, };
    read_data_from_block(data_last_res, "/home/anastasi/Documents/AU/FS/test_fs/block2", size);

    for (int i = 0; i < size; i++){
    	cout << data_last_res[i] <<" ";
    }
    
    return 0;
}
