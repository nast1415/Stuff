#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <vector>


using namespace std;

struct file_information {
	char name[256];
	int type;
	int next_block;
}; //Structure for storing file or 

void read_info_from_block(FILE* block, struct file_information data_info[], int data_info_size) {
	
	fread(data_info, sizeof(struct file_information), data_info_size, block);
	//cout << "data_info: " << endl;

	//for (int i = 0; i < data_info_size; i++) {
	//	cout << "Name: " << data_info[i].name << ", type: " << data_info[i].type << ", next_block: " << data_info[i].next_block << endl;
	//}

	//cout << endl;

	if (block) {
		fclose(block);
	}
} //Read an array of file_information structures (we use this function after we read number of infos in the block)

void add_info_to_block(string dir, int number_of_block, string info, int size_of_name, int info_type, int next_block, int allocation_table[], int table_size) {
	//Create new info object with our parameters
	const char * info_name = info.c_str();
	struct file_information new_info;

	for (int i = 0; i < size_of_name; i++) {
		new_info.name[i] = info_name[i];
	}
	for (int i = size_of_name; i < 256; i++) {
		new_info.name[i] = ' ';
	}

	new_info.type = info_type;
	new_info.next_block = next_block;

	struct file_information new_data[1] = {new_info};
	cout << "Now we're in add_info_to_block function" << endl;

	//Get block name
	char buffer[100];
	sprintf (buffer, "%d", number_of_block);
	string name = dir + "/block" + buffer;
	const char * block_name = name.c_str();

	int number_of_info[1] = {0};

	FILE * block;
	block = fopen(block_name, "rb");
	fread(number_of_info, sizeof(int), 1 , block); //Read number of the infos
	cout << "Now we read first int of the block and it is: " << number_of_info[0] << endl;

	if (block) {
		fclose(block);
	}

	if (number_of_info[0] < 15) {
		cout << "We're in the first case " << endl;
		block = fopen(block_name, "rb");
		fseek(block, 4, SEEK_SET);
		struct file_information data[number_of_info[0] + 1];
		for (int i = 0; i < number_of_info[0]; i++) {
			fread(data, sizeof(struct file_information), number_of_info[0], block);
		} //Now we have data array with all infos from this block

		if (block) {
			fclose(block);
		}

		data[number_of_info[0]] = new_info;
		//cout << "New data's name: " << data[number_of_info[0]].name << endl;
		//cout << "New data's type: " << data[number_of_info[0]].type << endl;
		//cout << "New data's next_block: " << data[number_of_info[0]].next_block << endl;
		number_of_info[0]++;

		block = fopen(block_name, "wb");
		fwrite(number_of_info, sizeof(int), 1, block);
		fwrite(data, sizeof(struct file_information), number_of_info[0], block);


		if (block) {
			fclose(block);
		}


		int read_int_info[1] = {0};

		block = fopen(block_name, "rb");
		fread(read_int_info, sizeof(int), 1, block);
		cout << "Number of infos: " << read_int_info[0] << endl;

		struct file_information data_info[read_int_info[0]];

		read_info_from_block(block, data_info, read_int_info[0]);

	} else {
		int first_free_block = -2;
		for (int i = 0; i < table_size; i++){
			if (allocation_table[i] == -1) {
				first_free_block = i;
				break;
			}
		}

		if (first_free_block == -2) {
			cerr << "There is not enough memory in the file system!" << endl;
			return;
		}

		first_free_block--; //Get real block name

		char buffer[100];
		sprintf (buffer, "%d", first_free_block);
		string name = dir + "/block" + buffer;
		const char * block_name = name.c_str();

		int number_of_info[1] = {1};

		FILE * block;
		block = fopen(block_name, "wb");
		fwrite(number_of_info, sizeof(int), 1, block);
		fwrite(new_data, sizeof(struct file_information), 1, block);

		allocation_table[first_free_block + 1] = first_free_block + 1;

	}

} //This function add an info structure to the block (and update the allocation table if it need to be updated)


int find_directory_name(string dir, int number_of_block, string dir_name, int dir_size, int allocation_table[]) {

	cout << "-----------------------------FIND_DIRECTORY_NAME FUNCTION--------------------------" << endl;
	int number = number_of_block;
	int next_block_number = 0;
	const char * find_name = dir_name.c_str();
	cout << "Now we have name like this: " << find_name << "." << endl;

	char new_info[256];

	for (int i = 0; i < dir_size; i++) {
		new_info[i] = find_name[i];
	}
	for (int i = dir_size; i < 256; i++) {
		new_info[i] = ' ';
	}

	cout << "Our new_info: " << new_info << "." << endl;
	int flag = 0;

	while (true) {

		char buffer[100];
		sprintf (buffer, "%d", number);
		string name = dir + "/block" + buffer;
		const char * block_name = name.c_str();

		int number_of_info[1] = {0};

		FILE * block;
		block = fopen(block_name, "rb");

		fread(number_of_info, sizeof(int), 1, block);
		struct file_information data_info[number_of_info[0]];

		fread(data_info, sizeof(struct file_information), number_of_info[0], block);
	
		for (int i = 0; i < number_of_info[0]; i++) {
			flag = 0;
			cout << "Our new_info: " << data_info[i].name << ". And our name is: " << new_info << "." << endl;
			for (int j = 0; j < 256; j++) {
				if (data_info[i].name[j] != new_info[j]) {
					flag = 1;
				}
			}
			if (flag == 0) {
				return data_info[i].next_block;
			}
		}

		next_block_number = allocation_table[number + 1];
		next_block_number--;

		if (next_block_number == number) {
			break;
		} else {
			number = next_block_number;
		}
	}

	return 0;
} //This function checks, is it true that the directory named find_name is placed in this directory (which first block we give as a parameter)


void create_update_block_with_info_data(string dir, int number, struct file_information data[], int size) {
	//Get block name
	//const char * block_name = get_block_name(dir, number);
	//cerr << "Block name is: " << block_name << endl;
	char buffer[100];
	sprintf (buffer, "%d", number);
	string name = dir + "/block" + buffer;
	const char * block_name = name.c_str();
	
	
	//Create block with block_name and fill it with data
	FILE * block;
	block = fopen(block_name, "wb");
	fwrite(data, sizeof(struct file_information), size, block);
	fclose(block);
}

void create_update_block_with_int_data(string dir, int number, int data[], int size) {
	//Get block name
	//const char * block_name = get_block_name(dir, number);
	//cerr << "Block name is: " << block_name << endl;
	char buffer[100];
	sprintf (buffer, "%d", number);
	string name = dir + "/block" + buffer;
	const char * block_name = name.c_str();
	
	
	//Create block with block_name and fill it with data
	FILE * block;
	block = fopen(block_name, "wb");
	fwrite(data, sizeof(int), size, block);
	fclose(block);
} //This function helps us with the format function

//We need this function for format tests and mkdir
void read_data_from_block(int* data, const char* block_name, int size) {
	FILE * block;
	block = fopen (block_name, "rb");
	fread(data, sizeof(int), size , block);
	if (block) {
		fclose(block);
	}
}

//We need this function for many other functions such as mkdir
int parse_string(string str, vector<string> &directories, vector<int> &sizes, string &real_directory_name, int &size_of_real_name) {
	string part_of_the_path = "";
	int size_of_the_string = 0;
	string work_str = str + " "; //Add space at the end of our string (because we know that our path don't contain spaces)

	string::iterator first = work_str.begin(); //Create string iterator and now it is in the beginning of our string
	string::iterator last = str.end() - 1;
	cout << "Iterator equals: " << *last << endl;
	if (*first != '/') {
		cerr << "Error: path need to begin with /" << endl;
		return -1;
	}
	if (*last != '/') {
		cerr << "Error: this is not a directory" << endl;
		return -1;
	}

	first++;
	
	while (*first != ' ') {
		while ((*first != '/') && (*first != ' ')) {
			part_of_the_path += *first;
			size_of_the_string++;
			first++;
		}

		/*const char * dir_name = part_of_the_path.c_str();
		char new_info[256];

		for (int i = 0; i < size_of_the_string; i++) {
			new_info[i] = dir_name[i];
		}
		for (int i = size_of_the_string; i < 256; i++) {
			new_info[i] = ' ';
		}*/

		
		directories.push_back(part_of_the_path);
		sizes.push_back(size_of_the_string);


		part_of_the_path = "";
		size_of_the_string = 0;

		if (*first != ' ') {
			first++;
		}
	}
	string new_dir_name = directories[directories.size() - 1];
	directories.pop_back();
	int new_dir_size = sizes[sizes.size() - 1];
	sizes.pop_back();

	real_directory_name = new_dir_name;
	size_of_real_name = new_dir_size;

	cout << "Directories size: " << directories.size() << endl;
	
	cout << "And the directory we wanted to add have a name: " << new_dir_name << ", and size: " << new_dir_size << endl;
	return 0;
}

/*void find_directory(string dir, string name, int number_of_block) {
	int data[number_of_ints_in_block] = {0, };
	
	char buffer[100];
	sprintf (buffer, "%d", number_of_block);
	string b_name = dir + "/block" + buffer;
	const char * block_name = b_name.c_str();

	//const char * dir_name = get_block_name(dir, 0);
	read_data_from_block(data, block_name, number_of_ints_in_block); //Now we have an array of data from the block


}*/





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
	for (int i = real_size; i < 10; i++) {
		create_update_block_with_int_data(dir, i, null_data, number_of_ints_in_block);
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
	int number_of_ints_in_block = floor (4096 / sizeof(int));

	//First we need data from allocation table
	int data[number_of_ints_in_block] = {0, };
	
	char buffer[100];
	sprintf (buffer, "%d", 0);
	string name = dir + "/block" + buffer;
	const char * block_name = name.c_str();

	//const char * dir_name = get_block_name(dir, 0);
	read_data_from_block(data, block_name, number_of_ints_in_block); //Now we have an array of data from null block

	//Next we need to have first int from this alloc_table
	int number_of_the_root_block = data[0] - 1; //Real number of the root block
	cerr << "Root is placed in " << number_of_the_root_block << " block (real value)" << endl; //CORRECT

	//Next we need to read all alloc_table
	int alloc_table_array[number_of_ints_in_block * number_of_the_root_block] = {0, };
	for (int i = 0; i < number_of_the_root_block; i++) {
		int data[number_of_ints_in_block] = {0, };

		char buffer[100];
		sprintf (buffer, "%d", i);
		string name = dir + "/block" + buffer;
		const char * block_name = name.c_str();

		read_data_from_block(data, block_name, number_of_ints_in_block);

		for (int j = 0; j < number_of_ints_in_block; j++) {
			alloc_table_array[number_of_ints_in_block * i + j] = data[j];
		}
	}

	cerr << "Alloc table looks like this: " << endl;
	for (int i = 0; i < number_of_ints_in_block * number_of_the_root_block; i++) {
		cerr << alloc_table_array[i] << " ";
	} //CORRECT

	//Start to parse string of target_dir
	cerr << "We're going to parse the directory name" << endl;
	vector<string> directories;
	vector<int> sizes;

	string real_directory_name;
	int size_of_real_name;

	int res = parse_string(target_dir, directories, sizes, real_directory_name, size_of_real_name);
	if (res != 0) {
		return;
	}

	int number = number_of_the_root_block; //First we're going to search in root
	cout << "End of parsing, we're going to the " <<  number << " (root) block" << endl;

	for (int i = 0; i < directories.size(); i++) {
		cout << "Now we search for " << directories[i] << " directory" << endl;
		number = find_directory_name(dir, number, directories[i], sizes[i], alloc_table_array);
		if (number == 0) {
			cerr << "Error: path is incorrect" << endl;
			return;
		}
	}

	int number_of_last_dir = number;
	cout << "Number of the last directory: " << number_of_last_dir << " (need to be 5)" << endl;

	number = 0;
	number = find_directory_name(dir, number_of_last_dir, real_directory_name, size_of_real_name, alloc_table_array);
	if (number != 0) {
		cerr << "Error: directory with this name already exists" << endl;
		return;
	}

	int next_block_of_last_directory;
	number_of_last_dir++;

	while (true) {
		next_block_of_last_directory = alloc_table_array[number_of_last_dir];
		if (next_block_of_last_directory == number_of_last_dir) {
			break;
		} else {
			number_of_last_dir = next_block_of_last_directory;
		}
	}
	number_of_last_dir--;
	cout << "Number of the directory, where we're going to put our info: " << number_of_last_dir << endl;

	int table_size = number_of_ints_in_block * number_of_the_root_block;

	int first_free_block = -2;
	for (int i = 0; i < table_size; i++){
		if (alloc_table_array[i] == -1) {
			first_free_block = i;
			break;
		}
	}

	if (first_free_block == -2) {
		cerr << "Error: there is not enough memory in the file system" << endl;
		return;
	}

	first_free_block--; //Get real block name

	alloc_table_array[first_free_block + 1] = first_free_block + 1;

	int number_of_changed_blocks = ceil(double((first_free_block + 1) * sizeof(int)) / 4096);
	for (int i = 0; i < number_of_changed_blocks; i++) {
		int data[number_of_ints_in_block] = {0, };
		for (int j = 0; j < number_of_ints_in_block; j++) {
			data[j] = alloc_table_array[(i * number_of_ints_in_block) + j];
		}

		char buffer[100];
		sprintf (buffer, "%d", i);
		string name = dir + "/block" + buffer;
		const char * block_name = name.c_str();

		FILE * block = fopen(block_name, "wb");
		fwrite(data, sizeof(int), number_of_ints_in_block, block);

		if (block) {
			fclose(block);
		}
	}

	/*int alloc_table_array[number_of_ints_in_block * number_of_the_root_block] = {0, };
	for (int i = 0; i < number_of_the_root_block; i++) {
		int data[number_of_ints_in_block] = {0, };

		char buffer[100];
		sprintf (buffer, "%d", i);
		string name = dir + "/block" + buffer;
		const char * block_name = name.c_str();

		read_data_from_block(data, block_name, number_of_ints_in_block);

		for (int j = 0; j < number_of_ints_in_block; j++) {
			alloc_table_array[number_of_ints_in_block * i + j] = data[j];
		}
	}*/

	cout << "First free block, that we found in alloc table is: " << first_free_block << endl;

	//write info about our directory
	add_info_to_block(dir, number_of_last_dir, real_directory_name, size_of_real_name, 0, first_free_block, alloc_table_array, table_size);
	cout << "Wow! We done all this stuff!" << endl;


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

    //parse_string("/hello");
    //parse_string("hello/");
    //parse_string("hello");
    //parse_string("/hello/murr/purr");
    /*vector<string> directories;
    vector<int> sizes;

    string real_directory_name;

    parse_string("/hello/", directories, sizes, real_directory_name);
    cout << "directories size: " << directories.size() << endl;
    cout << "directory name: " << real_directory_name << endl;

    for (int i = 0; i < directories.size(); i++) {
		cout << i + 1 << " - th word in the path is: " << directories[i] << ", and it's length: " << sizes[i] << endl;
	}*/
	//format("/home/anastasi/Documents/AU/FS/test_fs", 4096);
	mkdir("/home/anastasi/Documents/AU/FS/test_fs", "/murr2/purr/");
	string dir = "/home/anastasi/Documents/AU/FS/test_fs";

	cout << "-----------------------------5-TH BLOCK------------------------------" << endl;

	char buffer[100];
	sprintf (buffer, "%d", 5);
	string name = dir + "/block" + buffer;
	const char * block_name = name.c_str();

	int number_of_info[1] = {1};

	FILE * block;
	block = fopen(block_name, "rb");
	fread(number_of_info, sizeof(int), 1, block);

	struct file_information data[number_of_info[0]];
	read_info_from_block(block, data, number_of_info[0]);

	for (int i= 0; i < number_of_info[0]; i++) {
		cout << i + 1 << "-th info have name: " << data[i].name << ", type: " << data[i].type << " and next_block: " << data[i].next_block << endl;
	}

	cout << "------------------------------------6-TH BLOCK------------------------------------" << endl;

	char buffer2[100];
	sprintf (buffer2, "%d", 6);
	string name2 = dir + "/block" + buffer2;
	const char * block_name2 = name2.c_str();

	int number_of_info2[1] = {1};

	FILE * block2;
	block2 = fopen(block_name2, "rb");
	fread(number_of_info2, sizeof(int), 1, block2);

	struct file_information data2[number_of_info2[0]];
	read_info_from_block(block2, data2, number_of_info2[0]);

	for (int i= 0; i < number_of_info2[0]; i++) {
		cout << i + 1 << "-th info have name: " << data2[i].name << ", type: " << data2[i].type << " and next_block: " << data2[i].next_block << endl;
	}

	cout << "------------------------------------8-TH BLOCK------------------------------------" << endl;

	char buffer3[100];
	sprintf (buffer3, "%d", 8);
	string name3 = dir + "/block" + buffer3;
	const char * block_name3 = name3.c_str();

	int number_of_info3[1] = {1};

	FILE * block3;
	block3 = fopen(block_name3, "rb");
	fread(number_of_info3, sizeof(int), 1, block3);

	struct file_information data3[number_of_info3[0]];
	read_info_from_block(block3, data3, number_of_info3[0]);

	for (int i= 0; i < number_of_info3[0]; i++) {
		cout << i + 1 << "-th info have name: " << data3[i].name << ", type: " << data3[i].type << " and next_block: " << data3[i].next_block << endl;
	}
	/*format("/home/anastasi/Documents/AU/FS/test_fs", 4096);
	mkdir("/home/anastasi/Documents/AU/FS/test_fs", "home");

	cout << endl;
	cout << "Return 5-th block: " << endl;
    int size = floor(4096 / sizeof(int));
    int data_null_res[size] = {5, };
    //read_data_from_block(data_null_res, "/home/anastasi/Documents/AU/FS/test_fs/block5", size);

    for (int i = 0; i < size; i++){
    	cout << data_null_res[i] << " ";
    }*/

	/*cout << endl;
    cout << "Return first block: " << endl;
    int data_first_res[size] = {5, };
    read_data_from_block(data_first_res, "/home/anastasi/Documents/AU/FS/test_fs/block1", size);

    for (int i = 0; i < size; i++){
    	cout << data_first_res[i] << " ";
    }


	cout << endl;
	cout << "Return other block: " << endl;
	int data_last_res[size] = {5, };
	read_data_from_block(data_last_res, "/home/anastasi/Documents/AU/FS/test_fs/block3", size);

    for (int i = 0; i < size; i++){
    	cout << data_last_res[i] <<" ";
    }*/
    

    
    return 0;
}
