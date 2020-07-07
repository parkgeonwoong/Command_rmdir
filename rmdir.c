#include <stdio.h>      //표준 입출력
#include <stdlib.h>     //표준 라이브러리
#include <string.h>     //문자열 
#include <unistd.h>     //파일 입출력 라이브러리
#include <fcntl.h>      //파일 입출력 
#include <sys/types.h>  //디렉토리 라이브러리, readdir
#include <sys/stat.h>   
#include <dirent.h>     // 디렉토리 라이브러리

#define False -1;

int argv_check(char* argv1, char* argv2);    //명령 인자 판단 함수
void help();                                                    //도움말 함수
void version();                                                // 버젼 함수
void error_m(char* argv1);                           // 에러 함수
void Remove(char* path);                            // 지우는 함수
void option_v(char* argv1, char* path);     // -v 옵션 함수
void option_p(char* argv1, char* path);     // -p 옵션 하무

/* argv[]_인자_확인
void print_argv(char *argv[]){
   int k=0;
   for(k=0; !(argv[k]==NULL); k++) {   printf("argv[%d]%s\n",k,argv[k]);   }
} */

int main(int argc, char* argv[])
{
    //print_argv(argv);

    int num;

    num = argv_check(argv[1], argv[2]);             // argv 확인

    //printf("num: %d\n", num);

    enum exam { help_n = 1, version_n, option_v_n, option_p_n, Remove_n };

    /////////   스위치   //////////
    switch (num) {

    case help_n:
        help();                                          //  --help 함수 호출
        break;
    case version_n:
        version();                                     // --version 함수 호출
        break;
    case option_v_n:                            //  -v 옵션 함수 호출
        option_v(argv[1], argv[2]);
        break;
    case option_p_n:                            // -p 옵션 함수 호출
        option_p(argv[1], argv[2]);
        break;
    case Remove_n:                            // 제거하는 함수 호출
        Remove(argv[1]);
        break;
    default:
        error_m(argv[1]);                     // 에러 함수 호출
    }
}

//////////// 명령 인자 판단 함수///////////
int argv_check(char* argv1, char* argv2)
{
    if (argv1 == NULL) {    //변수 값이 없을때 argv1의 값 NULL
        return False;
    }
    else if (!strcmp(argv1, "--help")) {
        return 1;
    }
    else if (!strcmp(argv1, "--version")) {
        return 2;
    }
    else if (!strcmp(argv1, "-v")) {
        return 3;
    }
    else if (!strcmp(argv1, "-p")) {
        return 4;
    }
    else if (!strcmp(argv1, ".") || (argv1, "..")) {
        return 5;
    }
    else {};
}

////// 도움말 메세지 함수 /////
void help()
{
    printf("Usage: ./rmdir [OPTION]... DIRECTORY...\n");
    printf("Remove the DIRECTORY(ies), if they are empty.\n\n");
    printf(" --ignore-fail-on-non-empty\n");
    printf("                  ignore each failure that is solely because a directory\n");
    printf("                    is non-empty\n");
    printf("  -p, --parents   remove DIRECTORY and its ancestors; e.g., 'rmdir -p a/b/c' is\n");
    printf("                    similar to 'rmdir a/b/c a/b a'\n");
    printf("  -v, --verbose   output a diagnostic for every directory processed\n");
    printf("      --help     이 도움말을 표시하고 끝냅니다\n");
    printf("      --version  버전 정보를 출력하고 끝냅니다\n\n");
    printf("GNU coreutils online help: <http://www.gnu.org/software/coreutils/>\n");
    printf("Report rmdir translation bugs to <http://translationproject.org/team/>\n");
    printf("For complete documentation, run: info coreutils 'rmdir invocation'\n");
}

//////// 버젼 메세지 함수 /////
void version()
{
    printf("./rmdir (GNU coreutils) 8.22\n");
    printf("Copyright (C) 2013 Free Software Foundation, Inc.\n");
    printf("License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n");
    printf("This is free software: you are free to change and redistribute it.\nThere is NO WARRANTY, to the extent permitted by law.\n");
    printf("\n박건웅이 만들었습니다.\n");
}

/////// 에러 메세지 함수 /////// 
void error_m(char* argv1)
{
    printf("./rmdir : 인식할수 없는 옵션 '%s' \n", argv1);
    printf("Try 'rmdir --help' for more information.\n");
}

/////// 디렉터리 삭제하는 함수 ////////
void Remove(char* path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) < 0)     //stat() : 파일에 대한 상태 정보 , 파일의 상태 정보를 stat 구조체  buf에 저장
    {
        printf("rmdir: failed to remove '%s' : 그런 파일이나 디렉터리가 없습니다. \n", path);
        error_m(path);      //에러 출력
        //perror("stat error");
        exit(1);
    }

    /// S_ISDIR : 디렉터리 파일 검사하는 매크로 함수, st_mode : stat구조체의 필드 내부
    if (S_ISDIR(statbuf.st_mode))
    {
        // DIR : 디렉터리 구조체 , opendir : 디렉터리 열기
        DIR* dir = opendir(path);
        struct dirent* de = NULL;


        while ((de = readdir(dir)) != NULL)    //각 디렉터리 엔트리를 반복해서 읽음
        {
            //strcmp : 두 문자열 비교 (엔트리 내 파일 이름 과 현재 디렉터리 대상)
            if (strcmp(de->d_name, ".") == 0)
            {
                continue;
            }
            if (strcmp(de->d_name, "..") == 0)
            {
                continue;
            }
            char npath[PATH_MAX];
            sprintf(npath, "%s/%s", path, de->d_name);
            Remove(npath);
        }
        closedir(dir);
        remove(path);
    }
    else
    {
        printf("./rmdir: failed to remove `%s': 디렉터리가 비어있지 않음 \n", path);
    }
}

///////// rmdir -v 옵션 함수 ///////////
void option_v(char* argv1, char* path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) < 0)     //stat() : 파일에 대한 상태 정보 , 파일의 상태 정보를 stat 구조체  buf에 저장
    {
        error_m(path);
        exit(1);
    }

    /// S_ISDIR : 디렉터리 파일 검사하는 매크로 함수, st_mode : stat구조체의 필드 내부
    if (S_ISDIR(statbuf.st_mode))
    {
        // DIR : 디렉터리 구조체 , opendir : 디렉터리 열기
        DIR* dir = opendir(path);
        struct dirent* de = NULL;

        while ((de = readdir(dir)) != NULL)    //각 디렉터리 엔트리를 반복해서 읽음
        {
            //strcmp : 두 문자열 비교 (엔트리 내 파일 이름 과 현재 디렉터리 대상)
            if (strcmp(de->d_name, ".") == 0)
            {
                continue;
            }
            if (strcmp(de->d_name, "..") == 0)
            {
                continue;
            }
            char npath[PATH_MAX];
            sprintf(npath, "%s/%s", path, de->d_name);
            Remove(npath);

        }
        closedir(dir);

        remove(path);
        printf("./rmdir: removing directory, '%s'\n", path);
    }
    else
    {
        printf("./rmdir: failed to remove `%s': 디렉터리가 비어있지 않음 \n", path);
    }
}

///////// rmdir -p 옵션 함수 /////////
void option_p(char* argv1, char* path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) < 0)     //stat() : 파일에 대한 상태 정보 , 파일의 상태 정보를 stat 구조체  buf에 저장
    {
        error_m(path);
        exit(1);
    }

    /// S_ISDIR : 디렉터리 파일 검사하는 매크로 함수, st_mode : stat구조체의 필드 내부
    if (S_ISDIR(statbuf.st_mode))
    {
        // DIR : 디렉터리 구조체 , opendir : 디렉터리 열기
        DIR* dir = opendir(path);
        struct dirent* de = NULL;

        while ((de = readdir(dir)) != NULL)    //각 디렉터리 엔트리를 반복해서 읽음
        {
            //strcmp : 두 문자열 비교 (엔트리 내 파일 이름 과 현재 디렉터리 대상)
            if (strcmp(de->d_name, ".") == 0)
            {
                continue;
            }
            if (strcmp(de->d_name, "..") == 0)
            {
                continue;
            }
            char npath[PATH_MAX];
            sprintf(npath, "%s/%s", path, de->d_name);
            Remove(npath);
        }
        closedir(dir);
        printf("%s \n", path);
        remove(path);           // 자식 디렉터리 제거 , path = 부모 경로 + 자식 경로

        ///////  부모 경로 (상위 경로) ///////
        char* e_path = path;
        char* strpath = strtok(e_path, "/");        //strtok : 문자열 제거 함수 
        printf("%s \n", strpath);
        remove(strpath);        // 부모 디렉터리 제거 
    }
    else
    {
        printf("./rmdir: failed to remove `%s': 디렉터리가 비어있지 않음 \n", path);
    }
}