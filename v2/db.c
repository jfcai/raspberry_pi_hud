#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define dbFile "/home/pi/msg.db"

int get_1_Msg(char *str)
{
	char **dbresult;
    char *errmsg;
    int nrow,ncolumn,index,ret;
    int id = 0;
    sqlite3 *db = NULL;
    sqlite3_open(dbFile,&db);

    ret = sqlite3_get_table(db,"select * from t_msg where fkey is null or fkey = '0' order by fid desc limit 1",&dbresult,&nrow,&ncolumn,&errmsg);
    if(ret == SQLITE_OK && nrow > 0){
    	printf("ncolumn=%d,nrow=%d\n",ncolumn,nrow);
        index=ncolumn;
        id = atoi(dbresult[index]);
        //printf("%d\n",id);
        //printf("%s,%s,%s\n",dbresult[3],dbresult[4],dbresult[5]);
        strcpy(str,dbresult[index+1]);
        //printf("%s,%s,%s\n",dbresult[3],dbresult[4],dbresult[5]);
    }
    
    sqlite3_free_table(dbresult);
    sqlite3_close(db);

    return id;
}

void del_1_Msg(int id)
{
	char sql[4096] = {0};
	sqlite3 *db = NULL;
    sqlite3_open(dbFile,&db);

    sprintf(sql,"update t_msg set fkey = '1' where fid = %d",id);

    sqlite3_exec(db,sql,NULL,NULL,NULL);
    sqlite3_close(db);
}

void insert_1_msg(const char *msg)
{
	char sql[4096] = {0};
    char buff[121] = {0};
    int len;
	sqlite3 *db = NULL;
    sqlite3_open(dbFile,&db);

    len = strlen(msg);
    //如果数据超长则拆分
    if(len > 120){
        //for(i = 0;i+= 120;i < len){
        //    strncpy(buff,msg,120);
        //    if(buff[119] )
        //}
        strncpy(buff,msg,200);
    }
    sprintf(sql,"insert into t_msg(fmsg) select '%s' where not exists(select * from t_msg where fmsg = '%s')",buff,buff);
    sqlite3_exec(db,sql,NULL,NULL,NULL);
    sqlite3_close(db);
}
