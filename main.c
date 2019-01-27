
#include "main.h"


char *builtIn_Komutlari[] = {
  "cd",
  "help",
  "quit",
  "increment" //increment
};

void GirisEkrani()
{
	printf("\n\t============================================\n");
        printf("\t               Linux C Shell  \n");
        printf("\t--------------------------------------------\n");
        printf("\t                                  \n");
        printf("\t============================================\n");
        printf("\n\n");
}

void Prompt()
{
	char hostn[1204] = "";
	gethostname(hostn, sizeof(hostn));
	printf(MAG "%s@%s:"RESET CYN "%s > " RESET, getenv("LOGNAME"), hostn, getcwd(konum, 1024));
}

char* EkrandanOku()
{
	char* satir = malloc(sizeof(char*) * 1024);
	int i = 0;
	int karakter = 0;


	if (!satir)
	{
    		fprintf(stderr, "lkabuk: allocation error\n");
    		exit(EXIT_FAILURE);
  	}

	while(1)
	{
		karakter = getchar();

		if(karakter == EOF)
		{
			exit(EXIT_SUCCESS);
		}
		else if(karakter == '\n')
		{
			satir[i] == '\0';
			return satir;
		}
		else
		{
			satir[i] = karakter;
		}
		i++;
	}

}

char** ParametreleriAyir(char* satir)
{
	const char* sinir = " \t\r\n\a";
	int i = 0;
	char** parametreler = malloc(sizeof(char*) * 256);
	char* parametre;


	if(!parametreler)
	{
		fprintf(stderr, "lkabuk: allocation error\n");
    		exit(EXIT_FAILURE);
	}

	parametre = strtok(satir, sinir);
	while(parametre != NULL)
	{
		parametreler[i] = parametre;
		i++;
		parametre = strtok(NULL, sinir);
	}
	parametreler[i] = NULL;
	return parametreler;

}

int builtIn_cd(char** komut)
{
	if(komut[1] == NULL)
	{
		fprintf(stderr, "lkabuk: missing argument \"cd\"\n");
	}
	else
	{
		int status = chdir(komut[1]);
		if(status != 0)
			perror("lkabuk");
	}
	return 1;
}

int builtIn_help(char** komut)
{
  	printf("-------SHELL SYSTEM HELP-------\n");
	for(int i = 0; i < 4; i++)
	{
		printf(" %s\n", builtIn_Komutlari[i]);
	}
	printf("You can use commands.\n");
  	return 1;
}

int builtIn_quit(char** komut)
{
	return 0;
}

int builtIn_increment(char** komut)
{
  char buf[20];
  pid_t pid;


  if((pid = fork()) == -1)
  {
    printf("Child could not be created\n");
    return 1;
  }
  if (pid==0)
  {
    if(komut[1]!=NULL)
    {
      if (!(access (komut[2],F_OK) != -1))
    	{
    		  printf("lkabuk: %s could not find a file named \n",komut[2]);
		      kill(getpid(),SIGTERM);
    		  return 1;
    	}
	    if(!strcmp(komut[1],">"))
	    {
		      printf("Invalid parameter");
		      kill(getpid(),SIGTERM);
    		  return 1;
      }

    	int dosyaTanimlayici=0;

    		dosyaTanimlayici = open(komut[2], O_RDONLY, 0600);
    		dup2(dosyaTanimlayici, STDIN_FILENO);
    		close(dosyaTanimlayici);
        fgets(buf,20,stdin);

        int val=atoi(buf);
        val++;
        printf("%d\n", val);

    }
    else
    {
      fgets(buf,20,stdin);


      int val=atoi(buf);
      val++;
      printf("%d\n", val);
      kill(getpid(),SIGTERM);
    }
  }
  waitpid(pid,NULL,0);

  return 1;
}


void girisYonlendirme(char** komut, char* dosya)
{
  pid_t pid;
	if (!(access (dosya,F_OK) != -1))
	{
		printf("lkabuk: %s could not find a file named\n",dosya);
		return;
	}
	int dosyaTanimlayici;
	if((pid = fork()) == -1)
	{
		printf("Child could not be created\n");
		return;
	}
	if (pid==0)
	{
		dosyaTanimlayici = open(dosya, O_RDONLY, 0600);
		dup2(dosyaTanimlayici, STDIN_FILENO);
		close(dosyaTanimlayici);

		if (execvp(komut[0],komut) == -1)
		{
			printf("lkabuk: failed");
			kill(getpid(),SIGTERM);
		}
	}
	waitpid(pid,NULL,0);
}

void cikisYonlendirme(char** komut, char* dosya)
{
  pid_t pid;
	int dosyaTanimlayici;
	if((pid = fork()) == -1)
	{
		printf("Child could not be created\n");
		return;
	}

	if (pid == 0)
	{
		dosyaTanimlayici = open(dosya, O_CREAT | O_TRUNC | O_WRONLY, 0600);
		dup2(dosyaTanimlayici,STDOUT_FILENO);
		close(dosyaTanimlayici);

		if (execvp(komut[0],komut) == -1)
		{
			printf("lkabuk: failed");
			kill(getpid(),SIGTERM);
		}
	}
	waitpid(pid,NULL,0);
}

int arkaplanCalistir(char** komut)
{
  pid_t pid;
	int status;

	pid = fork();
	if (pid == 0)
	{
    struct sigaction act;
  	act.sa_handler = sig_child;
  	sigemptyset(&act.sa_mask);
  	act.sa_flags = SA_NOCLDSTOP;
  	if (sigaction(SIGCHLD, &act, NULL) < 0)
  	{
  		fprintf(stderr, "sigaction failed\n");
  		return 1;
  	}

    if(fork() == 0)
    {
  		if (execvp(komut[0], komut) == -1)
  		{
  			printf("Command not found");
  			kill(getpid(), SIGTERM);
  		}
    }
	}
	else if (pid < 0)
	{
		perror("lkabuk");
	}
	else
	{
		printf("Proses PID:%d created with value\n",pid);
	}
	return 1;

}

void sig_child(int signo)
{
  int status, child_val,chid;
	chid = waitpid(-1, &status, WNOHANG);
	if (chid > 0)
	{
		if (WIFEXITED(status))
	    {
	        child_val = WEXITSTATUS(status);
	        printf("[%d] retval : %d \n",chid, child_val);
          Prompt();
	    }
	}
}

int komutCalistir(char** komut)
{
  pid_t pid;
  int status;
  pid = fork();
  if(pid == 0)
  {
    int sonuc = execvp(komut[0], komut);
    if(sonuc == -1)
    {
      perror("lkabuk");
      kill(getpid(),SIGTERM);
    }
  }
  else if(pid > 0)
  {

    waitpid(pid,NULL,0);
  }
  else
  {
    perror("lkabuk");
  }
  return 1;
}

int siraliCalistir(char** komut, int count)
{
  pid_t pid;
  for(int i=0;i<count;i=i+2)
  {
    if((pid = fork()) == -1)
  	{
  		printf("Child could not be created\n");
  		return 1;
  	}
  	if (pid==0)
  	{
  		if (execlp(komut[i],komut[i],komut[i+1],NULL) == -1)
  		{
  			printf("lkabuk: failed");
  			kill(getpid(),SIGTERM);
  		}
  	}
  	waitpid(pid,NULL,0);
  }
  return 1;
}


int pipeKomut(char** komut)
{
int pipes[4];
int kontrol1, kontrol2;


char *ilk_komut[10];
char *ikinci_komut[10];
char *ucuncu_komut[10];

int i = 0;
int komutSayisi = 0;
int ekle1 = 0;
int ekle2 = 0;
int ekle3 = 0;


while(komut[i] != NULL)
{

  if(strcmp(komut[i], "|") == 0)
  {
    komutSayisi++;
  }
  else
  {
    if(komutSayisi == 0)
    {
      ilk_komut[ekle1] = komut[i];
      ekle1++;
    }
    else if(komutSayisi == 1)
    {
      ikinci_komut[ekle2] = komut[i];
      ekle2++;

    }
    else if(komutSayisi == 2)
    {
      ucuncu_komut[ekle3] = komut[i];
      ekle3++;
    }
    else
    {
      printf("lkabuk: Number of unsupported parameters.\n");
      return 1;
    }
  }

  i++;
}
ilk_komut[ekle1] = NULL;
ikinci_komut[ekle2] = NULL;
ucuncu_komut[ekle3] = NULL;

ekle3 = 0;
if(komutSayisi == 1)
{
  int i = 0;
  while(ikinci_komut[i] != NULL)
  {
    ucuncu_komut[ekle3] = ikinci_komut[i];
    ekle3++;
    i++;
  }
  ucuncu_komut[ekle3] = NULL;
}


kontrol1 = pipe(pipes);
kontrol2 = pipe(pipes + 2);

if (kontrol1 == -1 && kontrol2 == -1)
{
  perror("lkabuk: pipe failure");
  return 1;
}

if(fork() == 0)
{
  dup2(pipes[1],1);

  close(pipes[0]);
  close(pipes[1]);
  close(pipes[2]);
  close(pipes[3]);


  if(execvp(*ilk_komut, ilk_komut) == -1)
  {
    perror("lkabuk");
    kill(getpid(),SIGTERM);
    return 1;
  }
}
else
{
  if(fork() == 0)
  {
    dup2(pipes[0], 0);
    dup2(pipes[3], 1);

    close(pipes[0]);
    close(pipes[1]);
    close(pipes[2]);
    close(pipes[3]);

    if(!strcmp(ikinci_komut[0], "increment"))
    {
      builtIn_increment(ikinci_komut);
      return 1;
    }


    if(execvp(*ikinci_komut, ikinci_komut) == -1)
    {
      perror("lkabuk");
      kill(getpid(),SIGTERM);
      return 1;
    }
  }
  else
  {
    if(fork() == 0)
    {
      dup2(pipes[2], 0);

      close(pipes[0]);
      close(pipes[1]);
      close(pipes[2]);
      close(pipes[3]);

      if((!strcmp(ucuncu_komut[0], "increment")))
      {

        if(komutSayisi == 1)
        {
          ucuncu_komut[0] = "sort";
        }
        else
        {
          builtIn_increment(ucuncu_komut);
          return 1;
        }
      }


      if(execvp(*ucuncu_komut, ucuncu_komut) == -1)
      {
        perror("lkabuk");
        kill(getpid(),SIGTERM);
        return 1;
      }
    }
  }
}
close(pipes[0]);
close(pipes[1]);
close(pipes[2]);
close(pipes[3]);

for(int i = 0; i < 3; i++)
  wait(NULL);

}

int kabuk_calistir(char** komut)
{

  int arkaplan;
  char* komut_temp[256];
  char* komut_tempSirali[256];


	if(komut[0] == NULL)
		return 1;

	int builtInKomutKontrol = -1;

	for(int i = 0; i < 4; i++)
	{
		if (strcmp(komut[0], builtIn_Komutlari[i]) == 0)
		{
			builtInKomutKontrol = i;
			break;
		}
		else
			builtInKomutKontrol = -1;
	}

	if(builtInKomutKontrol >= 0)
	{
		if(builtInKomutKontrol == 0)
			return builtIn_cd(komut);
		else if(builtInKomutKontrol == 1)
			return builtIn_help(komut);
		else if(builtInKomutKontrol == 2)
			return builtIn_quit(komut);
    else if(builtInKomutKontrol == 3)
      return builtIn_increment(komut);
		else
			perror("lkabuk");
	}
	else
	{

    int pipeKontrol = 0;
    int siraliKomutKontrol = 0;
    int k = 0;

    while(komut[k] != NULL)
    {
      if(!strcmp(komut[k], "|"))
      {
          pipeKontrol = 1;
      }
      else if(!strcmp(komut[k], ";"))
      {
        siraliKomutKontrol = 1;
      }
      k++;
    }

    if(siraliKomutKontrol == 1)
    {
      int i = 0;
      int ekle = 0;
      while(komut[i] != NULL)
      {
        if(!strcmp(komut[i], ";"))
        {

        }
        else
        {
          komut_tempSirali[ekle] = komut[i];
          ekle++;
        }
        i++;
      }
      komut_tempSirali[i] = NULL;

      siraliCalistir(komut_tempSirali, ekle);
      return 1;
    }

    if(pipeKontrol == 1)
    {
      pipeKomut(komut);
      return 1;
    }


    // io background
    int i = 0;
    while(komut[i] != NULL)
    {
      if(strcmp(komut[i],">") == 0 || strcmp(komut[i],"<") == 0 || strcmp(komut[i],"&") == 0)
        break;
      komut_temp[i] = komut[i];
      i++;
    }
    komut_temp[i] = NULL;
    int j = 0;
    arkaplan = 0;

    while(komut[j] != NULL && arkaplan == 0)
    {
      if(!(strcmp(komut[j],"&")))
      {
        arkaplan = 1;
      }
      else if(!(strcmp(komut[j],">")))
      {
        if(komut[j+1] == NULL)
          printf("Not enough parameters\n");
        cikisYonlendirme(komut_temp, komut[j+1]);
        return 1;
      }
      else if(!(strcmp(komut[j],"<")))
      {
        if(komut[j+1] == NULL)
          printf("Not enough parameters\n");
        girisYonlendirme(komut_temp, komut[j+1]);
        return 1;
      }
      j++;
    }
    komut_temp[j] == NULL;

    if(arkaplan == 0)
    {
      komutCalistir(komut_temp);
      return 1;
    }
    else if(arkaplan == 1)
    {
      arkaplanCalistir(komut_temp);
      return 1;
    }
    return 1;
	}
}

int main()
{
	int status = 1;
	char* satir;
	char** parametreler;


	GirisEkrani();
	do
	{
		Prompt();
		printf("> ");
		satir = EkrandanOku();
		parametreler = ParametreleriAyir(satir);
		status = kabuk_calistir(parametreler);
	}while(status);

	clear();
  return 0;
}
