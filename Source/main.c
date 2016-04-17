#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void special_char_choose(char *receive,int column,FILE *dosya,FILE *dosya2);

int main()
{
    FILE *dosya,*dosya2;
    int column_count,i,flag,kontrol;
    char file_name[50]= {};
    char file2_name[50];
    char character_array[50]="";
    char *characterPtr=character_array;
    char receive_char;
    char *keywords [20] = {"PROGRAM","EXTERNAL","COMMON","PRINT","READ","CALL","END","SUBROUTINE","DATA","IF","THEN","DO","CONTINUE","STOP","FUNCTION","ELSE","RETURN","INTEGER","REAL","DOUBLE"};
    char *rel_op[6] = {".EQ.",".NE.",".LT.",".LE.",".GT.",".GE."};

    printf("Please write the file name!\n");//kullanicidan dosya ismi aliniyor
    scanf("%s",file_name);

    strcpy(file2_name,file_name);

    dosya=fopen(strcat(file_name,".for"),"r");//ve istenen formatta dosya aciliyor
    if (dosya==NULL)
        printf("Bu dosya acilamadi!\n");
    else
    {
        dosya2=fopen(strcat(file2_name,".lex"),"w");//output dosyamiz da aciliyor
        if (dosya2==NULL)
            printf("Cikti dosyasi yaratilamadi!\n");
        else
        {
            column_count=0;
            while (!feof(dosya))
            {
                fscanf(dosya,"%c",&receive_char);//dosyadan bir karakter okunuyor
                column_count++;

                if (receive_char=='C' && column_count==1)//ilk sutundaysak ve ilk karakter c harfi ise 'comment'
                {
                    while (receive_char!='\n')
                        fscanf(dosya,"%c",&receive_char);
                    column_count=0;
                }

                if (1<column_count && column_count<=5 && isdigit(receive_char))//1 le 5. sutunlar arasindaysak 'label'
                {
                    printf("%c is a Statement Label!\n",receive_char);
                    fprintf(dosya2,"%c is a Statement Label!\n",receive_char);
                }

                if ((column_count>=7 && isalnum(receive_char)) || (column_count>=7 && receive_char=='.') || (column_count>=7 && receive_char=='_'))//harf,sayi,nokta veya alt cizgi ise diziye atiliyor
                {
                    *characterPtr=receive_char;
                    characterPtr++;
                }

                if (column_count>=7 && !isalnum(receive_char) && isalnum(*character_array) && receive_char!='_' && receive_char!='.')//dizimiz doluysa ve yeni gelen karakterimiz harf,sayi,nokta veya alt cizgi degil ise kontrol edicez
                {
                    flag=0;
                    for (i=0; i<20; i++)
                    {
                        if (strcmp(character_array,keywords[i])==0)//dizimizin icinde keyword mu var kontrol ediliyor
                        {
                            if (i==19)//iki kelimeli double precision keywordu bosluklarda durdugumuz icin burda kontrol ediliyor
                            {
                                *characterPtr=receive_char;
                                characterPtr++;
                                fscanf(dosya,"%c",characterPtr);
                                while (*characterPtr!=' ')
                                {
                                    characterPtr++;
                                    fscanf(dosya,"%c",characterPtr);
                                }
                            }
                            printf("%s is a keyword\n",character_array);
                            fprintf(dosya2,"%s is a keyword\n",character_array);
                            while (characterPtr!=character_array)//dizimiz bosaltiliyor
                            {
                                *characterPtr='\0';
                                characterPtr--;
                            }
                            *characterPtr='\0';
                            flag=1;
                            break;
                        }
                    }

                    if (flag==0 && isalpha(*character_array))// dizimizin icinde identifier mi var
                    {
                        printf("%s is a identifier\n",character_array);
                        fprintf(dosya2,"%s is a identifier\n",character_array);
                        while (characterPtr!=character_array)// dizimiz bosaltiliyor
                        {
                            *characterPtr='\0';
                            characterPtr--;
                        }
                        *characterPtr='\0';
                    }

                    if (isdigit(*character_array))// dizimizin icinde int constan veya real constant mi var
                    {
                        kontrol=0;
                        for (i=0; i<50; i++)
                        {
                            if (character_array[i]=='\0')
                                break;
                            if (isalpha(character_array[i]) || character_array[i]=='.')//eger dizinin icinde harf veya nokta varsa real constant
                            {
                                printf("%s is a real constant\n",character_array);
                                fprintf(dosya2,"%s is a real constant\n",character_array);
                                kontrol=1;
                                break;
                            }
                        }

                        if (kontrol==0)//harf veya nokta yoksa int constant
                        {
                            printf("%s is a int constant\n",character_array);
                            fprintf(dosya2,"%s is a int constant\n",character_array);
                        }

                        while (characterPtr!=character_array)//dizimiz bosaltiliyor
                        {
                            *characterPtr='\0';
                            characterPtr--;
                        }
                        *characterPtr='\0';
                    }
                }

                if (column_count>=7 && *character_array=='.')//dizimizin ilk karakteri nokta ise bu bir relational op tur
                {
                    for (i=0; i<6; i++)
                    {
                        if (strcmp(character_array,rel_op[i])==0)
                        {
                            printf("%s is a relational operator\n",character_array);
                            fprintf(dosya2,"%s is a relational operator\n",character_array);
                            while (characterPtr!=character_array)// dizimiz bosaltiliyor
                            {
                                *characterPtr='\0';
                                characterPtr--;
                            }
                            *characterPtr='\0';
                            break;
                        }
                    }
                }

                if (!isalnum(receive_char) && receive_char!=' ' && receive_char!='\n' && receive_char!='.')// dizimiz bir ozel karakterse secim yapicak fonk cagiriliyor
                    special_char_choose(&receive_char,column_count,dosya,dosya2);

                if (receive_char=='\n')// satir sonu karakteri gorduysek sutun sayacini sifirliyoruz
                    column_count=0;
            }

        }

        fclose(dosya);
    }

    return 0;
}

void special_char_choose(char *receive,int column,FILE *dosya,FILE *dosya2)
{
    int sayac=1,flag=0;
    char temp_array[100]="";
    char *temp_char=temp_array;

    switch(*receive)
    {
    case ',':
        printf("%c is a seperator\n",*receive);
        fprintf(dosya2,"%c is a seperator\n",*receive);
        break;
    case '/':// iki durumumuz var ya slash ya da bolme operatoru
        fseek(dosya,(6-column)*sizeof(char),SEEK_CUR);//bulundugumuz satirin basina gidiyoruz
        fscanf(dosya,"%c",temp_char);

        if (!isalpha(*temp_char))//eger 7. sutundan baslamadiysak harf gorene kadar bakiyoruz ve sonra kelimeyi aliyoruz
        {
            fscanf(dosya,"%c",temp_char);
            sayac++;
            while (!isalpha(*temp_char))
            {
                fscanf(dosya,"%c",temp_char);
                sayac++;
            }

            while (*temp_char!=' ')
            {
                temp_char++;
                fscanf(dosya,"%c",temp_char);
                sayac++;
            }
        }
        else//7. sutunda harf gorduysek kelimeyi aliyoruz
        {
            while (*temp_char!=' ')
            {
                temp_char++;
                fscanf(dosya,"%c",temp_char);
                sayac++;
            }
        }

        if (strcmp(temp_array,"DATA ")==0 || strcmp(temp_array,"COMMON ")==0)//aldigimiz kelime 'data' veya 'common' ise bu ozel karakter bir slash tir
        {
            printf("%c is a slash\n",*receive);
            fprintf(dosya2,"%c is a slash\n",*receive);
        }
        else// ustteki iki kelimeden biri degilse bolme operatorudur
        {
            printf("%c is a division operator\n",*receive);
            fprintf(dosya2,"%c is a division operator\n",*receive);
        }

        fseek(dosya,(column-sayac-6)*sizeof(char),SEEK_CUR);//fonksiyon cagrilirkenki noktaya geri donuyoruz
        break;
    case '=':
        printf("%c is a assignment operator\n",*receive);
        fprintf(dosya2,"%c is a assignment operator\n",*receive);
        break;
    case '*'://3 durum mevcut asteriks,carpma operatoru veya us alma operatoru
        fseek(dosya,(6-column)*sizeof(char),SEEK_CUR);//bulundugumuz satirin basina gidiyoruz
        fscanf(dosya,"%c",temp_char);

        if (!isalpha(*temp_char))//eger 7. sutundan baslamadiysak harf gorene kadar bakiyoruz ve sonra kelimeyi aliyoruz
        {
            fscanf(dosya,"%c",temp_char);
            sayac++;
            while (!isalpha(*temp_char))
            {
                fscanf(dosya,"%c",temp_char);
                sayac++;
            }

            while (*temp_char!=' ')
            {
                temp_char++;
                fscanf(dosya,"%c",temp_char);
                sayac++;
            }
        }
        else//7. sutunda harf gorduysek kelimeyi aliyoruz
        {
            while (*temp_char!=' ')
            {
                temp_char++;
                fscanf(dosya,"%c",temp_char);
                sayac++;
            }
        }

        if (strcmp(temp_array,"READ ")==0 || strcmp(temp_array,"PRINT ")==0)//aldigimiz kelime 'read' veya 'print' ise bu ozel karakterimiz bir asterikstir
        {
            printf("%c is a asterix\n",*receive);
            fprintf(dosya2,"%c is a asterix\n",*receive);
            flag=1;
            fseek(dosya,(column-sayac-6)*sizeof(char),SEEK_CUR);
        }
        else
        {
            fseek(dosya,(column-sayac-6)*sizeof(char),SEEK_CUR);
            fscanf(dosya,"%c",temp_char);

            if (*temp_char=='*')//aldigimiz kelime ustteki iki kelimeden biri degil ve sonraki karakterimiz asteriks ise bu us alma operatorudur
            {
                printf("** is a exponentiation operator\n");
                fprintf(dosya2,"** is a exponentiation operator\n");
                flag=1;
            }
        }

        if (flag==0)//ustteki iki durumu da saglayamazsak bu bir carpma operatorudur
        {
            printf("%c is a multiplication operator\n",*receive);
            fprintf(dosya2,"%c is a multiplication operator\n",*receive);
            fseek(dosya,-1*sizeof(char),SEEK_CUR);
        }

        break;
    case '\''://tirnak isareti gorduysek o satirdaki diger tirnaga kadar butun cumleyi aliyoruz bu bir string constant tir
        *temp_char=*receive;
        temp_char++;
        fscanf(dosya,"%c",temp_char);
        while (*temp_char!='\'')
        {
            temp_char++;
            fscanf(dosya,"%c",temp_char);
        }

        printf("%s is a string constant\n",temp_array);
        fprintf(dosya2,"%s is a string constant\n",temp_array);
        break;
    case '(':
        printf("%c is a open paranthesis\n",*receive);
        fprintf(dosya2,"%c is a open paranthesis\n",*receive);
        break;
    case ')':
        printf("%c is a close paranthesis\n",*receive);
        fprintf(dosya2,"%c is a close paranthesis\n",*receive);
        break;
    case '+':
        printf("%c is a addition operator\n",*receive);
        fprintf(dosya2,"%c is a addition operator\n",*receive);
        break;
    case '-':// iki durum var cikarma operatoru ve eksi isareti
        fscanf(dosya,"%c",temp_char);

        if (*temp_char==' ')//bir sonraki karakterimiz bosluksa bu bir cikarma operatorudur
        {
            printf("%c is a subtraction operator\n",*receive);
            fprintf(dosya2,"%c is a subtraction operator\n",*receive);
        }
        else//diger durumda eksi isaretidir
        {
            printf("%c is a unary minus\n",*receive);
            fprintf(dosya2,"%c is a unary minus\n",*receive);
        }

        fseek(dosya,-1*sizeof(char),SEEK_CUR);
        break;
    }
}
