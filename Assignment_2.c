#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<stdbool.h>
#include<string.h>

//comfig address_mapping
int Mapping (char * address_mapping, int offset, int row, int column, int bank, int rank) {
	int address = 0;
	char type_1[20] = "RW-RNK-BNK-CL";
	char type_2[20] = "RW-CL-RNK-BNK";
	char type_3[20] = "RNK-BNK-CL-RW";
	char type_4[20] = "RNK-RW-BNK-CL";
	if (!strcmp(address_mapping, type_1)){
		address = offset + (column << 6) + (bank << (6+7)) + (rank << (6+7+3)) + (row << (6+7+3+1));
	}
	else if (!strcmp(address_mapping, type_2)){
		address = offset + (bank << 6) + (rank << (6+3)) + (column << (6+3+15)) + (row << (6+3+15+7));
	}
	else if (!strcmp(address_mapping, type_3)){
		address = offset + (row << 6) + (column << (6+15)) + (bank << (6+15+7)) + (rank << (6+15+7+3));
	}
	else if (!strcmp(address_mapping, type_4)){
		address = offset + (column << 6) + (bank << (6+7)) + (row << (6+7+3)) + (rank << (6+7+3+15));
	}

	else{
		printf("ERROR: please input correct address_mapping method\n");
	}
//	printf(":::%d\n",address);
	return address;
}

int main(int argc, char** argv){
//create input variables
int address_width, num_requests;
char address_mapping[20], address_pattern[20], type_pattern[20];
if (argc != 11){
	printf("INPUT FORMAT ERROR: -w [address_width] -m [address_mapping] -n [num_requests] -p [address_pattern]-t [type_pattern]\n");
	return -1;
}
else{
	for(int i =1; i < argc; i ++){
		if(argv[i][0] == '-'){
			switch(argv[i][1]){
				case 'w':
					address_width = atoi(argv[i + 1]);
					break;
				case 'm':
					strcpy(address_mapping, argv[i + 1]);
					break;
				case 'n':
					num_requests = atoi(argv[i + 1]);
					break;
				case 'p':
					strcpy(address_pattern, argv[i + 1]);
					break;
		                case 't':
					strcpy(type_pattern, argv[i + 1]);
				       	break;
				default: 
					break;
				}
			}
		}	
	}
unsigned int offset, row, column, bank, rank = 0;
unsigned long address = 0;
int address_array[num_requests];
char pattern_1[20] = "Sequential";
char pattern_2[20] = "Random";
char pattern_3[20] = "All_row_hit";
char pattern_4[20] = "All_row_conflicts";

//comfig num_requests
//comfig address_pattern
if (!strcmp(address_pattern, pattern_1)){
	address =  Mapping ( address_mapping, offset, row, column, bank, rank); 
	address_array[0] = address;
//	printf("S\n");
	for (int i = 1; i < num_requests; i++){
		address = address + 64;
		address_array[i] = address;
	}
}
else if (!strcmp(address_pattern, pattern_2)){
//	printf("R\n");
	for (int i = 0; i < num_requests; i++){
		row = rand() % 32768;
		column = rand() % 128;
		bank = rand() % 8;
		rank = rand() % 2;
		address =  Mapping ( address_mapping, offset, row, column, bank, rank); 
		address_array[i] = address;
	}
}
else if (!strcmp(address_pattern, pattern_3)){
//	printf("H\n");
	address =  Mapping ( address_mapping, offset, row, column, bank, rank); 
	address_array[0] = address;
	for (int i = 1; i < num_requests; i++){
		if(column < 128){
			column = column +1;
			address =  Mapping ( address_mapping, offset, row, column, bank, rank); 
			address_array[i] = address;
		} else {
			column =  0;
			continue;
		}
	}
}
else if (!strcmp(address_pattern, pattern_4)){
//	printf("C\n");
	address =  Mapping ( address_mapping, offset, row, column, bank, rank); 
	address_array[0] = address;
	for (int i = 1; i < num_requests; i++){
		if(row < 32768){
			row = row +1;
			address =  Mapping ( address_mapping, offset, row, column, bank, rank); 
			address_array[i] = address;
		} else {
			row =  0;
			continue;
		}
	}
}
else{
       	printf("ERROR: please input correct address pattern\n");
	return -1;
}
/*//comfig address_width
if (address >= (1 << (address_width + 1))){
	printf("MAX: %d\n", 1 << (address_width + 1));
	printf("ERROR: address is too long\n");
	return -1;
}*/

//comfig type_pattern
char type_pattern_1[20] = "R";
char type_pattern_2[20] = "W";
char type_pattern_3[20] = "Switching";

//int to binary
int remain;
int result;
char B_bit;
char output[num_requests][address_width + 2];
for (int i = 0; i < num_requests; i++){
	result = address_array[i];
	for(int j = 0; j < address_width; j++){
		if(result != 1){
			remain = result % 2;
			result = result / 2;
			if (remain == 1){
				B_bit = '1';			
			}else{
				B_bit = '0';
			}
			output[i][address_width - 1- j] = B_bit;
		}else{
		 	output[i][address_width - 1- j] = '1';
			for(int j; j < address_width; j++)
				output[i][address_width - 1- j] = '0';
			break;
			}
	
	}
}

//output
for (int i = 0; i < num_requests; i++){
	printf("0x%08x",address_array[i]);
	if (!strcmp(type_pattern, type_pattern_1)){
		printf(" READ");
	}
	if (!strcmp(type_pattern, type_pattern_2)){
		printf(" WRITE");
	}
	if (!strcmp(type_pattern, type_pattern_3)){
		if (i % 2 == 1)
		printf(" READ");
		else
		printf(" WRITE");
	}
	printf(" 0\n");
}
/*for (int i = 0; i < num_requests; i++){
	printf("%s\n", output[i]);
}*/

//printf("%d,%s,%d,%s,%s\n",address_width,address_mapping,num_requests,address_pattern,type_pattern);

}

