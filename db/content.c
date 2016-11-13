#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include <curl/curl.h>
#include "cJSON.h"
#include "db.h"
 


void insertToDB(const char *news)
{
  cJSON *json,*result,*data;
  cJSON* pArrayItem = NULL;
  json = cJSON_Parse(news);
  if(!json)
  {
  	printf("error json %s\n",cJSON_GetErrorPtr());
  }
  else{
  	result = cJSON_GetObjectItem(json,"result");
  	if(!result){
  		printf("error result %s\n",cJSON_GetErrorPtr());
  	}else{
  		data = cJSON_GetObjectItem(result,"data");
  		if(!data){

  		}else{
  			int jsonSize = cJSON_GetArraySize(data);
  			int i;
  			for(i=0;i < jsonSize;i++)
  			{
  				pArrayItem = cJSON_GetArrayItem(data,i);
  				if(pArrayItem)
  				{
  					printf("%s\n",cJSON_GetObjectItem(pArrayItem,"title")->valuestring);
  					insert_1_msg(cJSON_GetObjectItem(pArrayItem,"title")->valuestring);

  				}
  			}
  		}
  	}
  }

  //cJSON_Delete(pArrayItem);
  //cJSON_Delete(data);
  //cJSON_Delete(result);
  cJSON_Delete(json);
}



void insertJokeToDB(const char *news)
{
  cJSON *json,*result,*data;
  cJSON* pArrayItem = NULL;
  json = cJSON_Parse(news);
  if(!json)
  {
    printf("error json %s\n",cJSON_GetErrorPtr());
  }
  else{
    result = cJSON_GetObjectItem(json,"result");
    if(!result){
      printf("error result %s\n",cJSON_GetErrorPtr());
    }else{
      data = cJSON_GetObjectItem(result,"data");
      if(!data){

      }else{
        int jsonSize = cJSON_GetArraySize(data);
        int i;
        for(i=0;i < jsonSize;i++)
        {
          pArrayItem = cJSON_GetArrayItem(data,i);
          if(pArrayItem)
          {
            printf("%s\n",cJSON_GetObjectItem(pArrayItem,"content")->valuestring);
            insert_1_msg(cJSON_GetObjectItem(pArrayItem,"content")->valuestring);

          }
        }
      }
    }
  }

  //cJSON_Delete(pArrayItem);
  //cJSON_Delete(data);
  //cJSON_Delete(result);
  cJSON_Delete(json);
}


struct MemoryStruct {
  char *memory;
  size_t size;
};
 
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}
 
int getTopNews(const char *type)
{
  CURL *curl_handle;
  CURLcode res;
  char url[1024] = {0};
 
  struct MemoryStruct chunk;
 
  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
  chunk.size = 0;    /* no data at this point */ 
 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* init the curl session */ 
  curl_handle = curl_easy_init();
 
  /* specify URL to get */ 
  sprintf(url,"http://v.juhe.cn/toutiao/index?type=%s&key=d284a2bfe4c40b7f8ced79d192ab6c56",type);
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
 
  /* send all data to this function  */ 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
 
  /* we pass our 'chunk' struct to the callback function */ 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
 
  /* some servers don't like requests that are made without a user-agent
     field, so we provide one */ 
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
 
  /* get it! */ 
  res = curl_easy_perform(curl_handle);
 
  /* check for errors */ 
  if(res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
  }
  else {
    /*
     * Now, our chunk.memory points to a memory block that is chunk.size
     * bytes big and contains the remote file.
     *
     * Do something nice with it!
     */ 
 
    printf("%lu bytes retrieved\n", (long)chunk.size);
  }
 
  /* cleanup curl stuff */ 
  curl_easy_cleanup(curl_handle);


  insertToDB(chunk.memory);
  
  //cJSON_GetObjectItem(json,"reason")->
 
  free(chunk.memory);
 
  /* we're done with libcurl, so clean it up */ 
  curl_global_cleanup();
 
  return 0;
}



int getJoke(int t)
{
  CURL *curl_handle;
  CURLcode res;
  char url[1024] = {0};
 
  struct MemoryStruct chunk;
 
  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
  chunk.size = 0;    /* no data at this point */ 
 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* init the curl session */ 
  curl_handle = curl_easy_init();
 
  /* specify URL to get */ 
  sprintf(url,"http://japi.juhe.cn/joke/content/list.from?pagesize=20&time=%d&key=bf1ec1946b1d9fe99e62ca91a194905b",t);
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
 
  /* send all data to this function  */ 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
 
  /* we pass our 'chunk' struct to the callback function */ 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
 
  /* some servers don't like requests that are made without a user-agent
     field, so we provide one */ 
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
 
  /* get it! */ 
  res = curl_easy_perform(curl_handle);
 
  /* check for errors */ 
  if(res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
  }
  else {
    /*
     * Now, our chunk.memory points to a memory block that is chunk.size
     * bytes big and contains the remote file.
     *
     * Do something nice with it!
     */ 
 
    printf("%lu bytes retrieved\n", (long)chunk.size);
  }
 
  /* cleanup curl stuff */ 
  curl_easy_cleanup(curl_handle);


  insertJokeToDB(chunk.memory);
  
  //cJSON_GetObjectItem(json,"reason")->
 
  free(chunk.memory);
 
  /* we're done with libcurl, so clean it up */ 
  curl_global_cleanup();
 
  return 0;
}