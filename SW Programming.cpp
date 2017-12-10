#include <string.h> 
#include <io.h> 
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
void FindName(char * path, char * key);      //�̸��� Ű���带 �����ϴ� ������ �˻� 
void GetFile(char * path, char * key);      //������ Ű���带 �����ϴ� ������ �˻�  
void Ranking();                        //�˻��� ������ ������� ���� 

int sub(char *);            //���̻簡 �ִ����� ���θ� �˻� 
float std_dev(float *, int);   //�ܼ� ǥ������ ��� �Լ� 
struct Found               //Ű���带 �����ϴ� ������ �ϳ� ���� ������ ����ü �ϳ� �Ҵ�  
{
   int num[4];
   int total;
   float * dist;

   char FileName[20];
   int size;

   float FinalScore;         //���� ���� = Ŭ���� Ranking���� ������ ��ġ  
};
typedef struct Found FOUND;
FOUND * list;
int j = 0;
int main()
{
   int i;
   float start, end;
   char path[120] = "C:/Users/cjdth/Desktop/���� C/1�г� ����/dataset-2nd";
   char key[40];

   printf("Input the keyword to search the relative file. : ");
   gets(key);
   for (i = 0; i<strlen(key); i++)      //��ҹ��� ���� �˻��� ���� ó��  
   {
      if (65 <= key[i] && key[i] <= 90)
         key[i] += 32;
   }

   start = clock();
   FindName(path, key);
   GetFile(path, key);
   Ranking();
   end = clock();

   printf("(ó������ %f�� ���)\n", (float)(end - start) / CLOCKS_PER_SEC);

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
         fread(text + 1, searching.size, 1, fp);      //fread �Լ��� ���� ���� ������ ��°�� �ҷ��� >> ����,����,���ڱ��� ���� �޾Ƶ��� 
         text[0] = 32; text[searching.size + 1] = 32;


         for (i = 1; i <= searching.size; i++)      //��ҹ��� �˻��� ���� ó�� 
         {
            if (65 <= text[i] && text[i] <= 90)
               text[i] += 32;
         }
         while ((temdis = strstr(text, key)) != NULL)   //strstr�� ���� Ű���尡 temdis�� ó�� �����ϴ� �ּҸ� ���� 
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
                  list[j].num[3]++;   //Ű���� �յڰ� ���ĺ� 
               else
                  list[j].num[2]++;   //Ű���� ���� ���ĺ�  
            }
            else
            {
               if (*(temdis + strlen(key)) >= 97 && *(temdis + strlen(key)) <= 122 && sub(temdis + strlen(key)) == 0) //���̻� �˻� 
                  list[j].num[1]++;   //Ű���� �ڰ� ���ĺ� 
               else
                  list[j].num[0]++;   //Ű���� �յڰ� ����or���� >> �ֿ켱���� 
            }
            list[j].dist[k] = (float)(temdis - text);   //Ű���忡�� ����Ű��������� ���� ���� 
            text = temdis + strlen(key);  //text�� Ű������ �����ּҷ� �ΰ� �ٽ� �ݺ�  
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
         list[i].FinalScore += 1000;   // ��ŷ�� �ֿ켱���� 
      for (k = 0; k<list[i].num[1]; k++)
         list[i].FinalScore += 80;      //�Ʒ��� �ܾ���� ǥ�������� �Բ� ��ŷ 
      for (k = 0; k<list[i].num[2]; k++)
         list[i].FinalScore += 30;
      for (k = 0; k<list[i].num[3]; k++)
         list[i].FinalScore += 20;

      if (list[i].total == 1)      //���� �ܾ �ϳ��ۿ� ���� ��, Ű���尡 ���� ���� ���� ���Ͽ� �켱���� �ο� 
      {
         list[i].dist[0] = 100 - list[i].dist[0]/100;
         list[i].FinalScore += list[i].dist[0];
      }
      else
         list[i].FinalScore += 200 - std_dev(list[i].dist, list[i].total);   //ǥ�������� ���� ���Ͽ� �켱���� �ο� 
   }
   file_seq = (int *)malloc((j + 1) * sizeof(int));

   for (i = 1; i <= j; i++)
   {
      file_seq[i] = i;
   }

   for (i = 1; i <= j - 1;)   //FinalScore�� ���� ����  
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
		printf("   '���ǹ��� Ű���� ����' : %d\n", list[file_seq[i]].num[0]);
		printf("	���� :  %d\n",i);
		printf("----------------------------------------------------------\n");
	}
  
   printf("\n==========================================================================\n");
   free(file_seq);
}
float std_dev(float * arr, int size)   //ǥ������ ��� �Լ�  
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
// Ű���� ���� ���̻縦 �˻��Ͽ� Ű���� ���� ���ڰ� s,es(������), d,ed(������),ing(������)�� ��� �ֿ켱�ܾ�� �����ϰ� ��� 
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
