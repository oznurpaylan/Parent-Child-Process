#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <fcntl.h>

#define NUM_THREADS 4
int nThread = 0;
pthread_mutex_t mutex;
char encrypText[200];
char metin1[50];
char metin2[50];
char metin3[50];
char metin4[50];

//Thread fonksiyonu
void *CreateThread(void *yazi) {
    //thread sayacı
    nThread = nThread + 1;

    printf("\n%d.Thread Calisiyor...\n", nThread);
    int i;
    char ch;
    //Şifreleme işlemi yapılıyor
    for(i=0; (*((char *)yazi + i)) !='\0'; ++i){ 
            ch=(*((char *)yazi + i));
            if(ch>= 'a' && ch<='z'){
                ch=ch+1;
                if(ch > 'z'){
                    ch=ch-'z'+'a'-1;
                }
            (*((char *)yazi + i))=ch;
            }
            else if(ch>= 'A' && ch<='Z'){
                ch=ch+1;
                if(ch > 'Z'){
                    ch=ch-'Z'+'A'-1;
                }
            (*((char *)yazi + i))=ch;
            }
        }
    //Shared Memory Tanımlanıyor
    const int SIZE = 4096;
    const char *name = "OS";
    int shm_fd;
    void *ptr;
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SIZE);
    ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    //Mutex Lock ve shared memory yazma işlemleri
    pthread_mutex_lock(&mutex);
    if(nThread == 1){
        strncat(metin1, (char *)yazi, 50);
        sprintf(ptr, "%s", metin1);
        ptr += strlen(metin1);
    }
    else if(nThread == 2){
        strncat(metin2, (char *)yazi, 50);
        sprintf(ptr, "%s", metin2);
        ptr += strlen(metin2);
    }
    else if(nThread == 3){
        
        strncat(metin3, (char *)yazi, 50);
        sprintf(ptr, "%s", metin3);
        ptr += strlen(metin3);
    }
    else if(nThread == 4){
        
        strncat(metin4, (char *)yazi, 50);
        sprintf(ptr, "%s", metin4);
        ptr += strlen(metin4);
    }
    //mutex unlock
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

int main(){
    //Bolunecek yazilar icin array tanimlaniyor
    char yaziBir[50];
    char yaziIki[50];
    char yaziUc[50];
    char yaziDort[50];
    int pipefds[2];           
    //Parent yazacagi mesaj
    char writemessage[200]="hukuk fakultesini bitirdigi zaman noter olmayacagi yazarlik yapacagini soylemisti ailesine yazarlik yapmanin gucluklerini ogrenmeye baslamasi yazdigi yazilari gozdenden gecirerek gozden yayinlamaliydi";    //yazılacak mesaj  
    //Child okunacak mesaj
    char readmessage[200];                   
    //Ordinary pipe oluşturuluyor           
    if (pipe(pipefds) == -1){
        printf("Pipe Olusturulamadi/n");
        return 1;
    }

    //Fork işlemi yapılıyor.
    pid_t pid = fork();

    if(pid==0) {       
        
        //Çocuk(child) proses     
        printf ("Child Process Calisti...\n");
        //Pipe write kısmı kapatılıyor
        close(pipefds[1]);
        //Mesaj Okunuyor...
        read(pipefds[0], readmessage, sizeof(readmessage));
        //pipe read kısmı kapatılıyor
        close(pipefds[0]);

        printf("\nChild Okudu: %s \n", readmessage);
        //metin 4 eşit parçaya bölünüyor.
        int i;
        for(i = 0;i < 50;i++){
            yaziBir[i] = writemessage[i];
        }
        for(i = 50;i <(2*50);i++){
            yaziIki[i - 50] = writemessage[i];
        }
        for(i = (2*50);i<(3*50);i++){
            yaziUc[i-(2*50)] = writemessage[i];
        }
        for(i = (3*50);i<=200;i++){
            yaziDort[i-(3*50)] = writemessage[i];
        }
        
        //Threadler Oluşturuluyor.
        pthread_t threads[5];
        
        pthread_create(&threads[1], NULL, CreateThread, &yaziBir);
        pthread_create(&threads[2], NULL, CreateThread, &yaziIki);
        pthread_create(&threads[3], NULL, CreateThread, &yaziUc);
        pthread_create(&threads[4], NULL, CreateThread, &yaziDort);
        //threadler bekleniyor
        for(i = 1 ; i<NUM_THREADS + 1; i++){
            pthread_join(threads[i], NULL);
        }
    }
    else if(pid != 0) {
        //parent prosess
        printf ("Parent Prosess Calisti...\n");
        //pipe read kapatılıyor
        close(pipefds[0]);
        //parent proses yazıyor
        write(pipefds[1], writemessage, sizeof(writemessage));
        //pipe write kısmı kapatılıyor
        close(pipefds[1]);
        //child proses bekleniyor
        wait(NULL);
        //Shared Memory tanımlanıyor
        const int SIZE = 4096;
        const char *name = "OS";
        int shm_fd;
        void *ptr;
        shm_fd = shm_open(name, O_RDONLY, 0666);
        ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
        //shared memoryden okuma yapılıyor.
        printf("\n\nMetin: %s\n", (char*)ptr);
        shm_unlink(name);
    }
}