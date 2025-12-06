#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

typedef union { 
  int   int_value;
  float float_value;
  char *string_value;
} answer_t;

extern char *strdup(const char *);

typedef bool (*check_func)(char *str);
typedef answer_t (*convert_func)(char *str);


int read_string(char *buf, int buf_siz){
  int length = 0;    
  int c;
  
  do{    
    if(length > buf_siz-1){
      break;
    }
    c = getchar();
    buf[length] = c;
    length++;
  }
  while (c != '\n' && c != EOF);
  
  buf[length-1] = '\0';
  return length-1;
}

/// Hjälpfunktion till ask_question_string
bool not_empty(char *str) {
  return strlen(str) > 0;
}

answer_t ask_question(char *question, check_func check, convert_func convert){

  char buf[255];
  int buf_siz = 255;
  
  do{
  printf("%s\n",question);
  read_string(buf,buf_siz);
  }while(!check(buf));
  
  answer_t result = convert(buf);

  return result;
}

int ask_question_int(char *question)
{
  answer_t answer = ask_question(question, is_number, (convert_func) atoi);
  return abs(answer.int_value); // svaret som ett heltal
}

char *ask_question_string(char *question)
{
  return ask_question(question, not_empty, (convert_func) strdup).string_value;
}


/*
// Varning för negativt exempel – nedanstående kod är dålig.

char *ask_question_string(char *question, char *buf, int buf_siz){
  
  printf("%s\n", question);
  read_string(buf,buf_siz);		  
  
  return buf;
}



int ask_question_int(char *question){

  int result = 0;
  int conversions = 0;
  
  do
    {
      printf("%s\n", question);
      conversions = scanf("%d", &result);
      int c;
      do
        {
          c = getchar();
        }
      while (c != '\n');
      putchar('\n');
    }
  while (conversions < 1);
  
  return result;
}

*/

void print(char *str){
  int length = 0;
  do{
      putchar(str[length]);
      length++;
    }  
  while(str[length] != '\0');

}
void println(char *str){

  print(str);
  putchar('\n');
}

bool is_number(char *num){
	if((num[0] == '-' && num[1] != '\0') || isdigit(num[0])){			
		for(int i = 1; i < strlen(num); i++){
			if(!isdigit(num[i])){
				if(num[i])
					return false;
			}
		}	
	}else{
		return false;
	}	
	return true;	
}
