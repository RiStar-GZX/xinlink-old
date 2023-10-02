#include <stdio.h>
#include <dsp.h>
#include <core.h>
#include <curl/curl.h>

struct FtpFile {
    const char *filename;
    FILE *stream;
};

static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
    struct FtpFile *out = (struct FtpFile *)stream;
    if(!out->stream) {
        /* open file for writing */
        out->stream = fopen(out->filename, "wb");
        if(!out->stream)
            return -1; /* failure, cannot open file to write */
    }
    return fwrite(buffer, size, nmemb, out->stream);
}


int curl_get(const char * url,const char * recv_path)
{
    if(url==NULL||recv_path==NULL)return -1;
    CURL *curl;
    CURLcode res;
    struct FtpFile ftpfile = {
        recv_path, /* name to store the file as if successful */
        NULL
    };

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL,url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);

        if(CURLE_OK != res) {
            fprintf(stderr, "curl told us %d\n", res);
        }
    }

    if(ftpfile.stream)
        fclose(ftpfile.stream); /* close the local file */

    curl_global_cleanup();
    return 0;
}

/**********************************************/
/*              curl client                   */
/**********************************************/
void * curl(XLevent_par * par){
    printf("NOW is curl:%s\n",par->pak_ins->ins);
    if(ins_get_par(par->pak_ins->ins,(char*)"ftp")>0){
        printf("FTP mode\n");
        char * url;
        if((url=ins_get_par_str(par->pak_ins->ins,(char *)"get"))!=NULL){
            printf("get\n");
            curl_get(url,"KEY3");
            //curl_get("http://ftp.mozilla.org/pub/firefox/releases/0.10.1/KEY","KEY2");
        }
    }
    return NULL;
}
