#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Card structure for search program
struct card {

	u_int32_t name_length;
	char* name;
	unsigned long offset;
};

enum rarity
{
	common,
	uncommon,
	rare,
	mythic
};

//Comparison function which compares two cards on the basis of name utilized for binary search
int comp_func (const void *a, const void *b){
	
	int val = strcmp(*(char **)a, *(char **)b);
	
	return val;
}

int main(int argc, char **argv){
	
	int num_cards = 200000;
	
	//Open the index binary file consisting name field for all the cards in ascending alphabetical order
	FILE *index_file = fopen("index.bin", "rb");
	
	//Check if the file exists or not and display stderr output
	if (index_file == NULL) {
		fprintf(stderr, "./search: cannot open(\"index.bin\"): No such file or directory\n");
		exit(1);
	}
	
	struct card *cards = NULL;
	
	//Alocate the memory to create array of structs
	cards = realloc(cards, sizeof(struct card)*num_cards);
	
	//Using fseek and ftell to find end of binary file
	fseek(index_file, 0, SEEK_END);
	long pos_end = ftell(index_file);
	fseek(index_file, 0, SEEK_SET);
	
	int  i = 0;
	char *address[num_cards];
	
	while(1){
		
		//Finish reading index.bin file when EOF is reached
		long pos_cur = ftell(index_file);
		if (pos_cur == pos_end){
			break;
		}
		
		//Read the string length of name field, name field and offset for all the cards in file
		u_int32_t name_length = 0;
		fread(&name_length, sizeof(u_int32_t), 1, index_file);
		cards[i].name_length = name_length;
		
		char name[name_length+1];
		name[name_length] = '\0';
		fread(name, name_length, 1, index_file);
		address[i] = strdup(name);
		cards[i].name = address[i];
		
		unsigned long offset;
		fread(&offset, sizeof(offset), 1, index_file);
		cards[i].offset = offset;
		i++;
	}

	//Create an array for all the name fields
	char *array[i];
	for (int a = 0; a < i; a++){
		array[a] = cards[a].name;	
	}
	
	//Open the cards binary file consisting all other fields except name for all the cards in the same order as original file
	FILE *cards_file = fopen("cards.bin", "rb");
	
	//Check if the file exists or not and display stderr output
	if (cards_file == NULL) {
		fprintf(stderr, "./search: cannot open(\"cards.bin\"): No such file or directory\n");
		exit(1);
	}
	
	int j = 0;
	char *address_buf[num_cards];
	while(1){
		int index = 0;
		char string[30];
		while(1){
	
			for (int c = 0; c < 30; c++){
				string[c] = '\0';
			}
			
			//Ask for the user input for cards name
			scanf("%[^\n]%*c", string);
			printf(">> %s\n", string);
			
			//If user input is 'q' then break from this loop
			if(strcmp(string, "q") == 0){
				break;
			}
		
			char *str = string;
			
			//Perform binary search for input string
			char** item = (char **) bsearch(&str, array, i, sizeof(char*), comp_func);
			
			if ( item != NULL){
				index = item-array;
				break;
			} else{
				printf("%s: '%s' not found!\n", argv[0], string);
			}
		}	
		
		//If user input is 'q' then break from this loop	
		if(strcmp(string, "q") == 0){
			break;
		}
		
		//Reach to the certain position in cards.bin file using corresponding offset and fseek for the card that matches
		unsigned long off_t = 0; 
		off_t = cards[index].offset;
		fseek(cards_file, off_t, SEEK_SET);
		
		//Read id field from cards.bin file
		unsigned int id = 0;
		fread(&id, sizeof(unsigned int), 1, cards_file);
		
		//Read cost field from cards.bin file
		u_int32_t length_cost = 0;
		fread(&length_cost, sizeof(u_int32_t), 1, cards_file);
		
		char cost[length_cost];
		cost[length_cost] = '\0';
		fread(cost, length_cost, 1, cards_file);
		address_buf[j] = strdup(cost);
		char *p_cost = address_buf[j];
		j++;
		
		//Read converted_cost field from cards.bin file
		unsigned int converted_cost = 0;
		fread(&converted_cost, sizeof(unsigned int), 1, cards_file);
		
		//Read type field from cards.bin file
		u_int32_t length_type = 0;
		fread(&length_type, sizeof(u_int32_t), 1, cards_file);
		
		char type[length_type];
		type[length_type] = '\0';
		fread(type, length_type, 1, cards_file);
		address_buf[j] = strdup(type);
		char *p_type = address_buf[j];
		j++;
		
		//Read text field from cards.bin file
		u_int32_t length_text = 0;
		fread(&length_text, sizeof(u_int32_t), 1, cards_file);
		
		char text[length_text];
		text[length_text] = '\0';
		fread(text, length_text, 1, cards_file);
		address_buf[j] = strdup(text);
		char *p_text = address_buf[j];
		j++;
		
		//Read stats field from cards.bin file
		u_int32_t length_stats = 0;
		fread(&length_stats, sizeof(u_int32_t), 1, cards_file);
		
		char stats[length_stats];
		stats[length_stats] = '\0';
		fread(stats, length_stats, 1, cards_file);
		address_buf[j] = strdup(stats);
		char *p_stats = address_buf[j];
		j++;
		
		//Read rarity field from cards.bin file
		enum rarity rarity;
		fread(&rarity, sizeof(enum rarity), 1, cards_file);
		
		//Get the string form for rarity field of card
		char *rare_value;
		if(rarity == 0){
			rare_value = "common";
		}else if(rarity == 1){
			rare_value = "uncommon";
		}else if(rarity == 2){
			rare_value = "rare";
		} else {
			rare_value = "mythic";
		}
		
		//When the binary search for the user input name matches, extract the required fields from cards.bin file and print all the information	
		int alignment = 52;
		
		printf("%s%*s\n", cards[index].name, alignment - (int)strlen(cards[index].name), p_cost);
			
		printf("%s%*s\n", p_type, alignment - (int)strlen(p_type), rare_value);
			
		for(int k = 0; k < alignment; k++){
			printf("-");
		}
		printf("\n");
			
		printf("%s\n", p_text);
			
		for(int k = 0; k < alignment; k++){
			printf("-");
		}
		printf("\n");
			
		printf("%*s\n", alignment, p_stats);
		printf("\n");
	}
	
	//Freeing the allocated memory space 
	for (int f = 0; f < i; f++){
		free(address[f]);	
	}
	
	//Freeing the allocated memory space
	for (int f = 0; f < j; f++){
		free(address_buf[f]);	
	}
	
	//Closing the file
	fclose(index_file);
	fclose(cards_file);
	
	//Freeing the allocated space for all the pointers of cards
	free(cards);
	return 0;
}