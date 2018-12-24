typedef struct Account
{
	char name[31];
	char pass[31];
}Account;
typedef struct Alist
{
	struct Alist *next;
	Account acc;
}Alist;

void update(Alist *root){			//update account data
	FILE *f;
	char filename[] = "account.txt";
	if ((f = fopen(filename, "w")) == NULL) 
		printf("Can not open file %s!\n",filename);
	else {
		Alist *cur;
		cur=root;
		while(cur->next!=NULL){
			cur=cur->next;
			fprintf(f, "%s ",(cur->acc).name);
			fprintf(f, "%s ",(cur->acc).pass);
			if(cur->next!=NULL) fprintf(f, "\n");
		}
	}
	fclose(f);
}
void readfile(Alist *root){			//Read account data
	char s[255];
	int i;
	Alist *cur;
	cur=root;
	FILE *f;
	char filename[] = "account.txt";
	if ((f = fopen(filename, "r")) == NULL){
		printf("Can not open file %s!\n",filename);
	} else{
		while (1){
			Alist *new;
			new=(Alist*)malloc(sizeof(Alist));
			fscanf(f,"%s", (new->acc).name);
			fscanf(f,"%s", (new->acc).pass);
			memset(s,'\0',strlen(s));
			printf("%s %s\n", (new->acc).name, (new->acc).pass);
			new->next=NULL;
			cur->next=new;
			cur=new;
			if(feof(f)) break;
		}
		fclose(f);
	}
}
/*void regis(Alist *root, char *name, char *pass){ 			//Register
	Alist *new,*cur;
	new=(Alist*)malloc(sizeof(Alist));
	cur=root;
	strcpy(new->name,name);
	strcpy(new->Password,pass)
	new->next=NULL;
	cur->next=new;
	cur=new;
	printf("Successful registration\n");
	update(root);
}*/