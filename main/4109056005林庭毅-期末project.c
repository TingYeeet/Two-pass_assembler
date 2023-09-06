#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char op[60][8];
int opcode[60];
char SYMTAB_label[60][8];
int SYMTAB_loc[60];
int SYM_top = 0;

int check_SYMTAB(char label[]);
int count_LOCCTRadd(char field_2[],char field_3[]);
int str_to_num_10(char field_3[]);
int str_to_num_16(char field_3[]);
int generate_object_code(char field_2[],char field_3[]);

int main()
{
	int LOCCTR=0,now_field=0,file_len=0;
	int i=0,j=0,k=0;
	char c;
	
	char field_1[8];
	char field_2[8];
	char field_3[8];
	
	FILE *read_opcode = fopen("opcode.txt","r");
	FILE *read_source_code = fopen("source.txt","r");
	FILE *write_LOCCTR = fopen("location counter.txt","w");
	FILE *write_SYMTAB = fopen("symbol table.txt","w");
	FILE *write_source_program = fopen("source program.txt","w");
	FILE *write_result = fopen("final object program.txt","w");
	
	//read opcode and make store in array
	while(fscanf(read_opcode,"%s%x",op[i],&opcode[i]) != EOF)
		i++;
		
	//for(i=0;i<59;i++)
	//	printf("%s %x\n",op[i],opcode[i]);
	
	//read first line
	fscanf(read_source_code,"%s%s%x",field_1,field_2,&LOCCTR);
	fprintf(write_LOCCTR,"%x\t%s\t%s\t%x\n",LOCCTR,field_1,field_2,LOCCTR);
	for(i=0;i<8;i++)
	{
		field_1[i] = '\0';
		field_2[i] = '\0';
		field_3[i] = '\0';
	}
	
	//read source code once a character
	c = getc(read_source_code);
	c = getc(read_source_code);
	while(1)
	{
		if(c == '\n' || c == EOF)
		{
			if(strcmp(field_2,"END") == 0)
			{
				fprintf(write_LOCCTR,"\t%s\t%s\t%s\n",field_1,field_2,field_3);
				//fprintf(write_source_program,"\t%s\t%s\t%s\n",field_1,field_2,field_3);
				break;
			}
			else
			{
				fprintf(write_LOCCTR,"%X\t%s\t%s\t%s\n",LOCCTR,field_1,field_2,field_3);
				
				//printf("%s",field_2);
				//fprintf(write_source_program,"%X\t%s\t%s\t%s\t%X\n",LOCCTR,field_1,field_2,field_3,generate_object_code(field_2,field_3));
			}
			
			//if label doesn't exist in SYMTAB,add in it
			if(strcmp(field_1,"") != 0 && !check_SYMTAB(field_1))
			{
				SYMTAB_loc[SYM_top] = LOCCTR;
				strcpy(SYMTAB_label[SYM_top++],field_1);
			}
			
			if(strcmp(field_2,"BYTE") == 0)
			{
				LOCCTR += count_LOCCTRadd(field_2,field_3);
			}
			else if(strcmp(field_2,"RESB") == 0 || strcmp(field_2,"RESW") == 0)
				LOCCTR += count_LOCCTRadd(field_2,field_3);
			else
				LOCCTR += 3;
			
			for(i=0;i<8;i++)
			{
				field_1[i] = '\0';
				field_2[i] = '\0';
				field_3[i] = '\0';
			}
			now_field = 0;
			
			if(c == EOF)
				break;
			else
			{
				c = getc(read_source_code);
				continue;
			}
		}
		
		if(c == '\t')
		{
			now_field++;
			c = getc(read_source_code);
			continue;
		}
		
		if(c == ' ')
		{
			now_field++;
			while(c == ' ' || c == '\t')
			{
				c = getc(read_source_code);
			}
			continue;
		}
		
		//printf("%c\n",c);
		
		if(now_field == 0)
			strncat(field_1,&c,1);
		else if(now_field == 1)
			strncat(field_2,&c,1);
		else if(now_field == 2)
			strncat(field_3,&c,1);
			
		c = getc(read_source_code);
	}
	
	for(i=0;i<SYM_top;i++)
		fprintf(write_SYMTAB,"%s\t%X\n",SYMTAB_label[i],SYMTAB_loc[i]);
	
	//fclose(write_SYMTAB);
	//fclose(write_LOCCTR);
	//pass 1
	//-------------------------------------------------------------------------------
	//pass 2
	rewind(read_source_code);
	
	char file_name[10],charted_16[10];
	char write_in_buffer[100]="\0";
	int for_byteX[10],byteX_top=0;
	int start_loc=0,GOC=0,loc_temp=0;
	int obcnt=0;
	
	fscanf(read_source_code,"%s%s%x",file_name,field_2,&start_loc);
	file_len = LOCCTR - start_loc;
	fprintf(write_source_program,"%X\t%s\t%s\t%X\n",start_loc,file_name,field_2,start_loc);
	fprintf(write_result,"H%s\t%06X%06X\n",file_name,start_loc,file_len);

	LOCCTR = start_loc;
	loc_temp = LOCCTR;
	now_field=0;
	
	for(i=0;i<8;i++)
	{
		field_1[i] = '\0';
		field_2[i] = '\0';
		field_3[i] = '\0';
	}
	
	c = getc(read_source_code);
	c = getc(read_source_code);
	while(1)
	{
		//printf("%c",c);
		if(c == '\n' || c == EOF)
		{
			if(strcmp(field_2,"END") == 0)
				fprintf(write_source_program,"\t%s\t%s\t%s\n",field_1,field_2,field_3);
			else
			{
				if(obcnt == 0)
					loc_temp = LOCCTR;
				
				//according to opcode and LOCCTR,generate object code
				GOC = generate_object_code(field_2,field_3);
				fprintf(write_source_program,"%X\t%s\t%s\t%-10s\t%06X\n",LOCCTR,field_1,field_2,field_3,GOC);
				
				obcnt++;
				
				if(strcmp(field_2,"BYTE") == 0 && field_3[0] == 'X')
				{
					for(i=2;i<strlen(field_3)-1;i++)
						strncat(write_in_buffer,&field_3[i],1);
				}
				else if(strcmp(field_2,"RESB")!=0 && strcmp(field_2,"RESW")!=0)
				{
					sprintf(charted_16,"%06X",GOC);
					charted_16[6] = '\0';
					strncat(write_in_buffer,charted_16,6);
				}	
				
				if(obcnt == 10)
				{
					fprintf(write_result,"T%06X%02X",loc_temp,strlen(write_in_buffer)/2);
					fprintf(write_result,"%s",write_in_buffer);
					fprintf(write_result,"\n");
					
					obcnt = 0;
					write_in_buffer[0] = '\0';
					
				}
			}
			
			if(strcmp(field_2,"BYTE") == 0)
			{
				LOCCTR += count_LOCCTRadd(field_2,field_3);
			}
			else if(strcmp(field_2,"RESB") == 0 || strcmp(field_2,"RESW") == 0)
				LOCCTR += count_LOCCTRadd(field_2,field_3);
			else
				LOCCTR += 3;
			
			for(i=0;i<8;i++)
			{
				field_1[i] = '\0';
				field_2[i] = '\0';
				field_3[i] = '\0';
			}
			now_field = 0;
			
			if(c == EOF)
				break;
			else
			{
				c = getc(read_source_code);
				continue;
			}
		}
		
		if(c == '\t')
		{
			now_field++;
			c = getc(read_source_code);
			continue;
		}
		
		if(now_field == 0)
			strncat(field_1,&c,1);
		else if(now_field == 1)
			strncat(field_2,&c,1);
		else if(now_field == 2)
			strncat(field_3,&c,1);
			
		c = getc(read_source_code);
	}
	fprintf(write_result,"T%06X%02X",loc_temp,strlen(write_in_buffer)/2);
	fprintf(write_result,"%s\n",write_in_buffer);
	fprintf(write_result,"E%06X\n",start_loc);

	fclose(read_opcode);
	fclose(read_source_code);
	fclose(write_LOCCTR);
	fclose(write_SYMTAB);
	fclose(write_source_program);
	fclose(write_result);
	return 0;
}

int check_SYMTAB(char label[])
{
	int i=0;
	
	for(i=0;i<SYM_top;i++)
	{
		if(strcmp(label,SYMTAB_label[i]) == 0)
			return 1;
	}
	
	return 0;
}

int count_LOCCTRadd(char field_2[],char field_3[])
{
	if(strcmp(field_2,"BYTE") == 0)
	{
		if(field_3[0] == 'C')
			return strlen(field_3) - 3;
		else if(field_3[0] == 'X')
			return (strlen(field_3)-3+1)/2;
	}
	else if(strcmp(field_2,"RESB") == 0)
		return str_to_num_10(field_3);
	else if(strcmp(field_2,"RESW") == 0)
		return str_to_num_10(field_3)*3;
}

int str_to_num_10(char field_3[])
{
	int i,digit=1,ans=0;
	
	//printf("%s\n",field_3);
	
	for(i=strlen(field_3)-1;i>=0;i--)
	{
		ans += (field_3[i]-48)*digit;
		digit*=10;
	}
	//printf("ans:%d\n",ans);
	return ans;
}

int str_to_num_16(char field_3[])
{
	int i,digit=1,ans=0;
	
	//printf("%s\n",field_3);
	
	for(i=strlen(field_3)-1;i>=0;i--)
	{
		if(field_3[i] >= 48 && field_3[i] <= 57)
			ans += (field_3[i]-48)*digit;
		else if(field_3[i] >= 65 && field_3[i] <= 70)
			ans += (field_3[i]-55)*digit;
		digit*=16;
	}
	//printf("ans:%d\n",ans);
	return ans;
}

int generate_object_code(char field_2[],char field_3[])
{
	//printf("%s %s\n",field_2,field_3);
	if(strcmp(field_2,"RESB") == 0 || strcmp(field_2,"RESW") == 0)
		return 0;
	else if(strcmp(field_2,"BYTE") == 0)
	{
		if(field_3[0] == 'X')
		{
			//printf("field_3 : %s\n",field_3);
			int i=0;
			char byteX_catcher[strlen(field_3)-2];
			
			for(i=0;i<strlen(field_3)-3;i++)
				byteX_catcher[i] = field_3[i+2];
			byteX_catcher[i] = '\0';
				
			//printf("%s\n",byteX_catcher);
			
			return str_to_num_16(byteX_catcher);
		}
		else
		{
			int i=0,ans=0,digit=1;
			for(i=strlen(field_3)-2;field_3[i] != '\'';i--)
			{
				int ascii = field_3[i];
				ans += ascii*digit;
				digit *= 256;
			}
			
			return ans;
		}
	}
	else if(strcmp(field_2,"WORD") == 0)
	{
		return str_to_num_10(field_3);
	}
	
	int i=0,j=0,k=0,x=0;
	int four_time_16 = 16*16*16*16;
	
	while(strcmp(field_2,op[j]) != 0)
		j++;
	
	if(strcmp(field_2,"RSUB") == 0)
		return opcode[j]*four_time_16;
	
	for(k=0;k<strlen(field_3);k++)
	{
		if(field_3[k] == ',')
			x=1;
	}
	
	if(x == 1)
	{
		char buffer[strlen(field_3)-1];
		int m=0;
		for(m=0;m<strlen(field_3)-2;m++)
			buffer[m] = field_3[m];
		buffer[m] = '\0';
		
		while(strcmp(buffer,SYMTAB_label[i]) != 0)
			i++;
		
		return opcode[j]*four_time_16 + SYMTAB_loc[i] + 8;
	}
	
	while(strcmp(field_3,SYMTAB_label[i]) != 0)
	{
		//printf("%s %s %d\n",field_3,SYMTAB_label[i],strcmp(field_3,SYMTAB_label[i]));
		i++;
	}
	//printf("%s %s\n",field_3,SYMTAB_label[i]);
	
	return opcode[j]*four_time_16 + SYMTAB_loc[i];
}
