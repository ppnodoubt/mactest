#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

void doLs(char []);
void doStat(char *, int dir_len);
void showFileInfo( char *filename, struct stat *info_p, int dir_len);
void modeToLetter( int , char []);
char *uidToName( uid_t );
char *gidToName( gid_t );
int cmpfunc(const void * a, const void * b);
int cntNumOfFile(char dirname[]);

int main(int ac, char *av[])
{
	if( ac == 1 )
		doLs( "." );
	else
		while( --ac ){
			printf("%s:\n", *++av );
			doLs( *av );
		}
}

int cmpfunc(const void *a , const void *b)
{
	return strcmp(*(char**)a , *(char**)b);		//qsort는 cmpfunc에 elements를 넘겨줄때 pointer로 넘겨주는데,
       							//이때 elements가 pointer라면 **로 type 변경 후 역참조를 해야
							//제대로 값을 비교하여 정렬함.
}

int cntNumOfFile(char dirname[])	//배열 동적할당을 위해 필요한 함수. array의 height를 결정.
{
	DIR		*dir_ptr;
	struct dirent	*direntp;

	int cnt = 0;
	
	dir_ptr = opendir( dirname );

	if( dir_ptr == NULL)
		fprintf(stderr, "ls1: cannot open %s\n", dirname);
	else
		while ( (direntp = readdir(dir_ptr)) != NULL )
		{
			if( strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0 || direntp->d_type == DT_UNKNOWN )
				continue;
			cnt++;
		}
	return cnt;
}

				
void doLs( char dirname[] )
{
	DIR		*dir_ptr;
	struct dirent	*direntp;
	
	int numOfFile = 0;
	int mEnd = 0;
	int mStart = 0;
	char	**asFileName;
	
	dir_ptr = opendir( dirname );
	numOfFile = cntNumOfFile( dirname );
	
	asFileName = (char **)malloc( sizeof(char*) * numOfFile );

	if ( dir_ptr == NULL )
		fprintf(stderr, "ls1: cannot open %s\n", dirname);
	else
	{
		while( ( direntp = readdir( dir_ptr ) ) != NULL )
		{
			if( strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0 || direntp->d_type == DT_UNKNOWN )
				continue;
				asFileName[mEnd] = (char *)malloc(sizeof(char) * ((strlen(direntp->d_name)+strlen(dirname) + 2)));	//+2한 이유는 strlen은 null문자를 생략하기때문에 +1 그리고 저장되어야할 문자열 %s/%s에서 슬래시를 더하여 총 2를 더하였다.
				sprintf( asFileName[mEnd++], "%s/%s", dirname, direntp->d_name );
		}
		qsort( asFileName, mEnd, sizeof(asFileName[0]), cmpfunc );
		int dir_len = 0;
		dir_len = strlen( dirname );
		for(mStart = 0; mStart < mEnd; mStart++)
			doStat( asFileName[mStart], dir_len);
	}
	for( int i = 1; i < mEnd; i++)
	{
		free(asFileName[i]);
	}
	free(asFileName);
	closedir(dir_ptr);
}

void doStat(char *filename, int dir_len)
{
	struct stat info;

	if( lstat(filename, &info) == -1 )
		perror( filename );
	else
		showFileInfo( filename, &info, dir_len);
}

void showFileInfo( char *filename, struct stat *info_p, int dir_len)
{
	char *uidToName(), *ctime(), *gidToName(), *filemode();
	void modeToLetters();
	char modestr[11];

	modeToLetters( info_p->st_mode, modestr );

	printf( "%s"	, modestr );
	printf( "%4d "	, (int) info_p->st_nlink );
	printf( "%-8s "	, uidToName(info_p->st_uid) );
        printf( "%-8s " , gidToName(info_p->st_gid) );
        printf( "%8ld " , (long) info_p->st_size );
	printf( "%.12s ", 4+ctime(&info_p->st_mtime) );
	printf( "%s\n"	, filename+(dir_len + 1));
}

void modeToLetters( int mode, char str[] )
{
	strcpy( str, "----------" );

	if( S_ISDIR(mode) )	str[0] = 'd';
	if( S_ISCHR(mode) )	str[0] = 'c';
	if( S_ISBLK(mode) )	str[0] = 'b';
	if( S_ISLNK(mode) )	str[0] = 'l';

	if( mode & S_IRUSR )	str[1] = 'r';
	if( mode & S_IWUSR )	str[2] = 'w';
	if( mode & S_IXUSR )	str[3] = 'x';

	if( mode & S_IRGRP )	str[4] = 'r';
	if( mode & S_IWGRP )	str[5] = 'w';
	if( mode & S_IXGRP )	str[6] = 'x';

	if( mode & S_IROTH )	str[7] = 'r';
	if( mode & S_IWOTH )	str[8] = 'w';
	if( mode & S_IXOTH )	str[9] = 'x';
}

char *uidToName( uid_t uid )
{
	struct passwd *getpwuid(), *pw_ptr;
	static char numstr[10];
	
	pw_ptr = getpwuid( uid );
	if( pw_ptr == NULL )
	{
		sprintf(numstr, "%d", uid );
		return numstr;
	}
	else
		return pw_ptr->pw_name;
}

char *gidToName( gid_t gid )
{
        struct group *getgrgid(), *grp_ptr;
        static char numstr[10];

        grp_ptr = getgrgid( gid );
        if( grp_ptr == NULL ){
                sprintf(numstr,"%d", gid );
                return numstr;
        }
        else
                return grp_ptr->gr_name;
}



