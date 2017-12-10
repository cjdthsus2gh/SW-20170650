#include <string.h> 
#include <io.h> 
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
void FindName(char * path, char * key);      //이름에 키워드를 포함하는 파일을 검색 
void GetFile(char * path, char * key);      //본문에 키워드를 포함하는 파일을 검색  
void Ranking();                        //검색한 파일을 순서대로 정렬 

int sub(char *);            //접미사가 있는지의 여부를 검사 
float std_dev(float *, int);   //단순 표준편차 계산 함수 
struct Found               //키워드를 포함하는 파일이 하나 있을 때마다 구조체 하나 할당  
{
   int num[4];
   int total;
   float * dist;

   char FileName[20];
   int size;

   float FinalScore;         //최종 점수 = 클수록 Ranking에서 상위에 위치  
};
typedef struct Found FOUND;
FOUND * list;
int j = 0;
int main()
{
   int i;
   float start, end;
   char path[120] = "C:/Users/cjdth/Desktop/과제 C/1학년 과제/dataset-2nd";
   char key[40];

   printf("Input the keyword to search the relative file. : ");
   gets(key);
   for (i = 0; i<strlen(key); i++)      //대소문자 포함 검색을 위한 처리  
   {
      if (65 <= key[i] && key[i] <= 90)
         key[i] += 32;
   }

   start = clock();
   FindName(path, key);
   GetFile(path, key);
   Ranking();
   end = clock();

   printf("(처리까지 %f초 경과)\n", (float)(end - start) / CLOCKS_PER_SEC);

   free(list);
   system("pause");
   return 0;
}
void FindName(char * path, char * key)
{
   long h_file;
   char search_Path[200];
   char filename[200];

   _finddata_t searching;
   sprintf(search_Path, "%s/*%s*.txt", path, key);
   printf("===================Files that include keyword in its name.==============\n");
   if ((h_file = _findfirst(search_Path, &searching)) != -1L)
   {
      do
      {
         printf("||   File Name : %s.   ||   File Size : %dbyte.   ||\n", searching.name, searching.size);
      } while (_findnext(h_file, &searching) == 0);
   }
   _findclose(h_file);
}
void GetFile(char * path, char * key)
{

   char * text;
   FILE * fp;
   char * text2;
   char * temdis;
   char ch;

   int h, i, k = 0;

   long g_file;
   char search_Path[200];
   char filename[200];

   _finddata_t searching;
   sprintf(search_Path, "%s/*.txt", path);

   list = (FOUND *)malloc(sizeof(FOUND));

   if ((g_file = _findfirst(search_Path, &searching)) != -1L)
   {
      do
      {
         sprintf(filename, "%s/%s", path, searching.name);
         fp = fopen(filename, "r");

         text = (char *)malloc((searching.size + 4) * sizeof(char));
         text2 = text;
         fread(text + 1, searching.size, 1, fp);      //fread 함수를 통해 파일 내용을 통째로 불러옴 >> 띄어쓰기,문자,숫자까지 전부 받아들임 
         text[0] = 32; text[searching.size + 1] = 32;


         for (i = 1; i <= searching.size; i++)      //대소문자 검색을 위한 처리 
         {
            if (65 <= text[i] && text[i] <= 90)
               text[i] += 32;
         }
         while ((temdis = strstr(text, key)) != NULL)   //strstr을 통해 키워드가 temdis에 처음 등장하는 주소를 대입 
         {
            if (k == 0)
            {
               j++;
               list = (FOUND *)realloc(list, (j + 1) * sizeof(FOUND));
               list[j].dist = (float *)calloc((k + 1), sizeof(float));
               sprintf(list[j].FileName, "%s", searching.name);
               list[j].size = searching.size;
               for (h = 0; h < 4; h++)
                  list[j].num[h] = 0;
            }
            else
               list[j].dist = (float *)realloc(list[j].dist, (k + 1) * sizeof(float));
            if (*(temdis - 1) >= 97 && *(temdis - 1) <= 123)
            {
               if (*(temdis + strlen(key)) >= 97 && *(temdis + strlen(key)) <= 122)
                  list[j].num[3]++;   //키워드 앞뒤가 알파벳 
               else
                  list[j].num[2]++;   //키워드 앞이 알파벳  
            }
            else
            {
               if (*(temdis + strlen(key)) >= 97 && *(temdis + strlen(key)) <= 122 && sub(temdis + strlen(key)) == 0) //접미사 검사 
                  list[j].num[1]++;   //키워드 뒤가 알파벳 
               else
                  list[j].num[0]++;   //키워드 앞뒤가 공백or문자 >> 최우선순위 
            }
            list[j].dist[k] = (float)(temdis - text);   //키워드에서 다음키워드까지의 간격 저장 
            text = temdis + strlen(key);  //text를 키워드의 시작주소로 두고 다시 반복  
            k++;
         }
         k = 0;
         text = text2;
         free(text);
         fclose(fp);
      } while (_findnext(g_file, &searching) == 0);
      _findclose(g_file);
   }
}
void Ranking()
{
   int * file_seq;

   int i, k;

   for (i = 1; i <= j; i++)
   {
      list[i].total = list[i].num[0] + list[i].num[1] + list[i].num[2] + list[i].num[3];
      list[i].FinalScore = 0;
      for (k = 0; k<list[i].num[0]; k++)
         list[i].FinalScore += 1000;   // 랭킹의 최우선순위 
      for (k = 0; k<list[i].num[1]; k++)
         list[i].FinalScore += 80;      //아래의 단어들은 표준편차와 함께 랭킹 
      for (k = 0; k<list[i].num[2]; k++)
         list[i].FinalScore += 30;
      for (k = 0; k<list[i].num[3]; k++)
         list[i].FinalScore += 20;

      if (list[i].total == 1)      //나온 단어가 하나밖에 없을 땐, 키워드가 가장 일찍 나온 파일에 우선순위 부여 
      {
         list[i].dist[0] = 100 - list[i].dist[0]/100;
         list[i].FinalScore += list[i].dist[0];
      }
      else
         list[i].FinalScore += 200 - std_dev(list[i].dist, list[i].total);   //표준편차가 작은 파일에 우선순위 부여 
   }
   file_seq = (int *)malloc((j + 1) * sizeof(int));

   for (i = 1; i <= j; i++)
   {
      file_seq[i] = i;
   }

   for (i = 1; i <= j - 1;)   //FinalScore에 따라 정렬  
   {
      if (list[i].FinalScore < list[i + 1].FinalScore)
      {
         list[0].FinalScore = list[i].FinalScore;
         list[i].FinalScore = list[i + 1].FinalScore;
         list[i + 1].FinalScore = list[0].FinalScore;

         file_seq[0] = file_seq[i];
         file_seq[i] = file_seq[i + 1];
         file_seq[i + 1] = file_seq[0];
         i = 1;
      }
      else
         i++;
   }
   printf("\n===================Files that include keyword.==========================\n");
   for (i = 1; i <= j; i++)
   {
		printf("  File Name : %s.		   File Size : %dbyte.   \n", list[file_seq[i]].FileName, list[file_seq[i]].size);
		printf("   '유의미한 키워드 개수' : %d\n", list[file_seq[i]].num[0]);
		printf("	순위 :  %d\n",i);
		printf("----------------------------------------------------------\n");
	}
  
   printf("\n==========================================================================\n");
   free(file_seq);
}
float std_dev(float * arr, int size)   //표준편차 계산 함수  
{
   int i;
   float average = 0, dev = 0;

   for (i = 0; i<size; i++)
      average += arr[i] / 100;
   average /= size;

   for (i = 0; i<size; i++)
      dev += (average - arr[i] / 100)*(average - arr[i] / 100);
   dev /= size;
   dev = sqrt(dev);

   return dev;
}
// 키워드 뒤의 접미사를 검사하여 키워드 뒤의 문자가 s,es(복수형), d,ed(과거형),ing(진행형)일 경우 최우선단어와 동일하게 취급 
int sub(char * temdis)
{
   if (*temdis == 's' || *temdis == 'd')
   {
      if (*(temdis + 1) == 32)
         return 1;
   }
   if (*temdis == 'e')
   {
      if (*(temdis + 1) == 's' || *(temdis + 1) == 'd' || *(temdis + 1) == 'n' || *(temdis + 1) == 'r')
      {
         if (*(temdis + 2) == 32)
            return 1;
      }
   }
   if (*temdis == 'i')
   {
      if (*(temdis + 1) == 'n')
      {
         if (*(temdis + 2) == 'g')
         {
            if (*(temdis + 3) == 32)
               return 1;
         }
      }
   }
   return 0;
}
