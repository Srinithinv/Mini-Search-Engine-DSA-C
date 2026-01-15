#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SIZE 101

struct DocNode{char filename[50];int count;struct DocNode*next;};
struct WordNode{char word[50];struct DocNode*docList;struct WordNode*next;};
struct WordNode*hashTable[SIZE];

int hash(char*w){int s=0;for(int i=0;w[i];i++)s+=w[i];return s%SIZE;}

void addDoc(struct WordNode*w,char*f){
    struct DocNode*d=w->docList;
    while(d){if(strcmp(d->filename,f)==0){d->count++;return;}d=d->next;}
    d=malloc(sizeof(struct DocNode));strcpy(d->filename,f);d->count=1;
    d->next=w->docList;w->docList=d;
}

void insertWord(char*w,char*f){
    int i=hash(w);struct WordNode*n=hashTable[i];
    while(n){if(strcmp(n->word,w)==0){addDoc(n,f);return;}n=n->next;}
    n=malloc(sizeof(struct WordNode));strcpy(n->word,w);n->docList=NULL;
    n->next=hashTable[i];hashTable[i]=n;addDoc(n,f);
}

void indexFile(char*f){
    FILE*fp=fopen(f,"r");if(!fp)return;char w[50];int i=0,c;
    while((c=fgetc(fp))!=EOF){
        if(isalpha(c))w[i++]=tolower(c);
        else if(i){w[i]='\0';insertWord(w,f);i=0;}
    }
    if(i){w[i]='\0';insertWord(w,f);}fclose(fp);
}

void buildIndex(){
    FILE*fp=fopen("files.txt","r");if(!fp)return;char f[50];
    while(fscanf(fp,"%s",f)!=EOF)indexFile(f);fclose(fp);
}

void printResults(char*word){
    int i=hash(word);struct WordNode*w=hashTable[i];
    int top=0,docs=0;
    while(w){
        if(strcmp(w->word,word)==0){
            struct DocNode*d=w->docList;
            while(d){if(d->count>top)top=d->count;d=d->next;}
            d=w->docList;
            printf("<p class='summary'>Documents found</p>");
            while(d){
                printf("<div class='result'>");
                if(d->count==top)printf("<span class='badge'>TOP</span>");
                printf("<h3>%s</h3>",d->filename);
                printf("<p>Count: <span class='hl'>%d</span></p>",d->count);
                printf("</div>");
                d=d->next;docs++;
            }
            return;
        }
        w=w->next;
    }
    printf("<p class='nomatch'>No results found</p>");
}

int main(){
    for(int i=0;i<SIZE;i++)hashTable[i]=NULL;
    buildIndex();

    char*data=getenv("QUERY_STRING"),word[50];
    printf("Content-type:text/html\n\n");
    printf("<html><head><meta charset='UTF-8'><title>Results</title><style>");

    printf("body{margin:0;height:100vh;background:linear-gradient(135deg,#e0f2ff,#f7e8ff);display:flex;align-items:center;justify-content:center;font-family:Segoe UI,sans-serif;}");
    printf(".card{background:rgba(255,255,255,0.75);padding:45px;border-radius:26px;width:520px;text-align:center;box-shadow:0 25px 60px rgba(120,80,200,0.35);animation:pop .5s ease}");
    printf("@keyframes pop{from{transform:scale(.9);opacity:0}to{transform:scale(1);opacity:1}}");
    printf("h1{color:#4b2aad}");
    printf(".summary{color:#777;font-size:13px;margin-bottom:12px}");
    printf(".result{background:white;padding:14px;border-radius:14px;text-align:left;margin-bottom:12px;position:relative;box-shadow:0 8px 20px rgba(0,0,0,.1);transition:.3s}");
    printf(".result:hover{transform:translateY(-4px)}");
    printf(".result h3{margin:0;color:#333}");
    printf(".result p{margin:6px 0 0;color:#666;font-size:14px}");
    printf(".hl{color:#6a5cff;font-weight:bold}");
    printf(".badge{position:absolute;top:-10px;right:-10px;background:#6a5cff;color:white;font-size:11px;padding:6px 12px;border-radius:20px}");
    printf(".back{display:inline-block;margin-top:18px;padding:12px 22px;border-radius:16px;background:linear-gradient(135deg,#6a5cff,#c77dff);color:white;text-decoration:none;transition:.3s}");
    printf(".back:hover{transform:translateY(-3px)}");
    printf(".nomatch{color:#555;margin-top:15px}");

    printf("</style></head><body><div class='card'>");

    if(data){
        sscanf(data,"word=%49s",word);
        for(int i=0;word[i];i++)word[i]=tolower(word[i]);
        printf("<h1>Results for \"%s\"</h1>",word);
        printResults(word);
    }else printf("<h1>No input received</h1>");

    printf("<a class='back' href='index.html'>Search Again</a>");
    printf("</div></body></html>");
    return 0;
}
