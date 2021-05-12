#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "card.h"

//Comparison function which compares two cards on the basis of name and then on the basis of their id and put them in ascending order
int comp_func (const void *a, const void *b){
	
	struct card **first_card = (struct card **)a;
	struct card **second_card = (struct card **)b;
	
	//Use of strcmp() to compare two card names
	int ret_val = strcmp((*first_card)->name, (*second_card)->name);
	
	//Determining ascending order on the basis of card id
	if (ret_val == 0) {
		if ((*first_card)->id < (*second_card)->id){
			
			ret_val = 1;
		} else if ((*first_card)->id > (*second_card)->id) {
			
			ret_val = -1;
		} else {
			
			ret_val = 0;
		}
	}
		
	return ret_val;
}

int main(int argc, char **argv){
	
	char *buf = NULL;
	size_t bufsize = 0;
	
	//Open the file consisting all the data
	FILE *input_file = fopen(argv[1], "r");
	
	//Check if the file exists or not and display stderr output
	if (input_file == NULL) {
		fprintf(stderr, "./parser: cannot open(\"%s\"): No such file or directory\n", argv[1]);
		exit(1);
	}
	
	ssize_t num;
	getline(&buf, &bufsize, input_file);
	 
	int num_cards = 200000;
	struct card **cards = NULL;
	
	//Allocate the memory for pointers to the pointers of struct card
	cards = realloc(cards, sizeof(struct card*) * num_cards);
	
	int i = 0;
	int j = 0;
	char *address_buf[num_cards];
	while( (num = getline(&buf, &bufsize, input_file)) > 0 ){
		
		//Saves the starting address which needs to be freed later in the program
		address_buf[j] = strdup(buf);
		
		//Save the data line by line from file read each field needed to make cards
		char *stringp = address_buf[j];
		if (stringp[strlen(stringp) - 1] == '\n'){
			stringp[strlen(stringp) - 1] = '\0';
		}
		
		//Allocate memory for each card
		cards[i] = malloc(sizeof(struct card));
		
		unsigned int id = atoi(strsep(&stringp, ","));
		
		//Extract id field for card
		cards[i]->id = id;
		
		stringp++;
		//Extract name field for card
		cards[i]->name = strsep(&stringp, "\"");
		
		stringp++;
		
		//Extract cost field for card if it exists
		if (stringp[0] == '\"'){
			stringp++;
			cards[i]->cost = strsep(&stringp, "\"");
			stringp++;
			
		} else {
			cards[i]->cost = strsep(&stringp, ",");
		}
		
		unsigned int converted_cost = atoi(strsep(&stringp, ","));
		
		//Extract converted cost field for card
		cards[i]->converted_cost = converted_cost;
		stringp++;
		
		//Extract type field for card
		cards[i]->type = strsep(&stringp, "\"");
		stringp+=2;
		
		//Extract text field for card
		cards[i]->text = stringp;
		
		//Reach the last character for backtracking
		stringp += strlen(stringp);
		
		int count = 0;
		
		while(count < 2){
			
			char *find = stringp;
			
			//Extracting rarity field for card
			if (find[0] == ','){
				count++;
				if (count == 1) {
					stringp += 2;
					char *rr = strsep(&stringp, "\"");
					size_t rr_size = strlen(rr);
					if (strcmp(rr, "common") == 0){
						cards[i]->rarity = common;
					} else if(strcmp(rr, "uncommon") == 0){
						cards[i]->rarity = uncommon;
					} else if(strcmp(rr, "rare") == 0){
						cards[i]->rarity = rare;
					} else{
						cards[i]->rarity = mythic;
					}
					
					stringp = stringp - rr_size - 3;
				}
				
				//Extract stats field for card
				if (find[1] == ','){
					cards[i]->stats = "";
					
				} else if (count == 2) {
					stringp += 2;
					char *stats = strsep(&stringp, "\"");
					size_t stats_size = strlen(stats);
					cards[i]->stats = stats;
					stringp = stringp - stats_size -3;
				}
			}
			
			stringp--;
		}
		
		//Extracting and manipulating text field (extracting new line, double quotes and so on)
		if (stringp[0] == '\"'){
			stringp[0] = '\0';
			int s = 0;
			for (int r = 0;  r < strlen(cards[i]->text); r++){
			
				if(cards[i]->text[r] == '\\'){
					if(cards[i]->text[r+1]  == 'n'){
						cards[i]->text[s] = '\n';
						s++;
						r++;
					continue;
					}
				}
				
				if(cards[i]->text[r] == '\"'){
					if(cards[i]->text[r+1]  == '\"'){
					continue;
					}
				}
			
			cards[i]->text[s] = cards[i]->text[r];
			s++;
			}
			cards[i]->text[s] = '\0';
		} else if (stringp[0] == ',') {
			cards[i]->text = "";
		}
		
		int n = 0;
		for(int t = 0; t < i; t++){
			if(strcmp(cards[i]->name, cards[t]->name) == 0){
				if(cards[i]->id <= cards[t]->id){
					free(cards[i]);
					i--;
				} else{
					for(int p = 0; p <= i; p++){
						if(p == t){
							free(cards[t]);
							continue;
						}
						cards[n] = cards[p];
						n++;
					}
				i--;	
				}
			}
		}
		
		j++;
		i++;
	}
	
	//Create and write on the cards.bin file with all the fields except name field along with string length for character array fields
	FILE *cards_file = fopen("cards.bin", "w");
	
	//Getting string length for cost, type, text and stats fields
	size_t length_cost[i];
	for(int l = 0; l < i; l++){
		length_cost[l] = strlen(cards[l]->cost);
	}
	
	size_t length_type[i];
	for(int l = 0; l < i; l++){
		length_type[l] = strlen(cards[l]->type);
	}
	
	size_t length_text[i];
	for(int l = 0; l < i; l++){
		length_text[l] = strlen(cards[l]->text);
	}
	
	size_t length_stats[i];
	for(int l = 0; l < i; l++){
		length_stats[l] = strlen(cards[l]->stats);
	}
	
	size_t off_t[i];
	
	for(int wb = 0; wb < i; wb++){
		
		//Get the offset for each card in the file
		if(wb == 0){
			off_t[wb] = 0;
		} else {
			off_t[wb] = off_t[wb-1] + 2*sizeof(unsigned int) + 4*sizeof(u_int32_t) + length_cost[wb-1] + length_type[wb-1] + length_text[wb-1] + length_stats[wb-1] + sizeof(enum rarity);
		}
		
		//Write the contents in the file with all cards 
		fwrite(&cards[wb]->id, sizeof(unsigned int), 1, cards_file);
		
		fwrite(&length_cost[wb], sizeof(u_int32_t), 1, cards_file);
		fwrite(cards[wb]->cost, length_cost[wb], 1, cards_file);
		
		fwrite(&cards[wb]->converted_cost, sizeof(unsigned int), 1, cards_file);
		
		fwrite(&length_type[wb], sizeof(u_int32_t), 1, cards_file);
		fwrite(cards[wb]->type, length_type[wb], 1, cards_file);
		
		fwrite(&length_text[wb], sizeof(u_int32_t), 1, cards_file);
		fwrite(cards[wb]->text, length_text[wb], 1, cards_file);
		
		fwrite(&length_stats[wb], sizeof(u_int32_t), 1, cards_file);
		fwrite(cards[wb]->stats, length_stats[wb], 1, cards_file);
		
		fwrite(&cards[wb]->rarity, sizeof(enum rarity), 1, cards_file);
	}
	
	//Get the offset for each card on how they are saved in cards.bin file
	for(int p = 0; p < i; p++){
		cards[p]->offset = off_t[p];
	}
	
	qsort(cards, i, sizeof(struct card*), comp_func);
	
	//Open and write on index.bin file with name field, its string length of name and offset
	FILE *index_file = fopen("index.bin", "w");
	
	//Get string length for name field
	size_t length_name[i];
	for(int l = 0; l < i; l++){
		length_name[l] = strlen(cards[l]->name);
	}
	
	//Write the contents in index.bin file from all cards
	for(int wb = 0; wb < i; wb++){
		fwrite(&length_name[wb], sizeof(u_int32_t), 1, index_file);
		fwrite(cards[wb]->name, length_name[wb], 1, index_file);
		fwrite(&cards[wb]->offset, sizeof(unsigned long), 1, index_file);
	}
	
	//Freeing all the space allocated for all the cards
	for(int free_count = 0; free_count < i; free_count++){
		
		free(cards[free_count]);
	}
	
	//Freeing the duplicated buffer using all the starting addresses
	for(int free_buf = 0; free_buf < j; free_buf++){
		
		free(address_buf[free_buf]);
	}
	
	//Freeing the allocated space for all the pointers of cards
	free(cards);
	
	//Closing the file
	fclose(input_file);
	fclose(cards_file);
	fclose(index_file);
	
	//Freeing the getline buffer
	free(buf);
	
	return 0;
}