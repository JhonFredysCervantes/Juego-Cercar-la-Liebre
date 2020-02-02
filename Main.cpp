#include <allegro.h>
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<fstream>
#include<conio.h>

//definiendo las dimenciones de pantalla
#define anchura 640
#define altura 480

using namespace std;

struct nodo{
    int x;
    int y;
    int st;
    nodo *sig;
    struct arista *sigA;

}*ListaNodo=NULL, *p, *r, *k,*w,*z,*d;

struct arista{
    struct nodo *sigN;
    arista *sigAr;
}*h,*q,*j,*g,*v;
//variables relacionadas con el color de las rayas
int R=53,G=255,B=0;

ifstream leer;
ofstream escribir;
int muertes=0,turno=0,reintentar=0;
int opcion=0;
int hor=0,m=0,s=0,ml=0;
bool ganar=false;
char vect[18]={' '};//para guardar nombre
char nombre[18];//vector auxiliar para manejar archivos
//mover vacio
nodo *vec[8]={NULL};
int i,ii;

BITMAP *Buffer;
BITMAP *strella, *lobo,*iluminado,*muerto,*conejo,*fallecido,*argusto1,*argusto2,*argusto3;
BITMAP *perder1,*perder2,*perder3,*ganarI,*ganarM;
BITMAP *cursor;
BITMAP *m1,*m2,*m3,*m4,*m5,*instruc,*fondo;
MIDI *Mjuego, *Mperder;
FONT *times,*viner;

void PintarEscena();
void cargarEnlaces();
void PosIni();
void pintarObjetos();
void imprime();
void mostrarAdyacentes(nodo *N);
void MovLobos();
void evaluar();
void Gana();
void perder();
void reiniciar();
void cronometro();
void evitarlobos();
void Mover_A_vacio();
void matarlobo();
void loboCol();
void loboFil();
void diagonalSupIzq();
void diagonalSupDer();
void diagonalInfIzq();
void diagonalInfDer();
void PedirNombre();
void guardarNombre();
void cargarMaterial();
void clasificatoria();
void instrucciones();
nodo *buscar(nodo *&ptr, int x, int y);
nodo *buscarST(int s);
nodo *buscarAux(int xlimS,int xlimI, int ylimS, int ylimI);
nodo *insertarNodo(nodo *&Ap,int x, int y);
arista *crearEnlases(nodo *&N,int xf, int yf);
int menuInicio();
int buscaLobos(nodo *&n);

/*para documentacion, en st=1 lobo, st=0 vacio, =st -1 conejo st=2 seleccionado  */

int main(){
    cargarEnlaces();//monta multilista

   allegro_init();
   set_color_depth(16);
   set_gfx_mode(GFX_AUTODETECT_WINDOWED, anchura, altura, 0, 0);

   if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) != 0) {
       allegro_message("Error: inicializando sistema de sonido\n%s\n", allegro_error);
    }
   install_keyboard();
   install_mouse();
   set_volume_per_voice(1);

    set_window_title("RUN RUN RABBIT");
    cargarMaterial();
    text_mode(-1);

    while(opcion!=1){

        if(reintentar==0){
            opcion=menuInicio();
        }else{
            opcion=2;
        }

       if(opcion==2){
            PosIni();
            play_midi(Mjuego,1);
            if(reintentar==0){
                PedirNombre();
            }
           while (!key[KEY_ESC]&&(muertes<4)&&(!ganar)&&(hor!=1||m!=30)){ //CICLO GENERAL
           		if(muertes==3){
                	muertes=4;
				}
                //clear_to_color(Buffer, makecol(0, 128, 128));
                draw_sprite(Buffer,fondo,0,0);
                PintarEscena();
                pintarObjetos();
                if(muertes==4){
                    textprintf(Buffer,font,10,15,makecol(250, 250, 250),"Bajas: %i",muertes-1);
                }else{
                    textprintf(Buffer,font,10,15,makecol(250, 250, 250),"Bajas: %i",muertes);
                }
                textprintf(Buffer,font,560,458,makecol(250, 250, 250),"%i: %i: %i",hor,m,s);
                cronometro();
                MovLobos();
                evaluar();
                draw_sprite(screen,Buffer,0,0);
           }
           clear_keybuf(); //borra las entradas de teclado que esperan en el buffer
           if(reintentar==0){
            guardarNombre();
           }
           reintentar=0;
           midi_pause();
           if(muertes>=3||(hor==1&&m==30)){
               muertes=0;
               textprintf(Buffer,viner,290,250,makecol(255,0,0),"--Perdiste--");
                while(muertes<50){
                    rest(20);
                    draw_sprite(screen,Buffer,0,0);
                    muertes++;
                }
                play_midi(Mperder,0);
                perder();
           }else if(ganar){
                Gana();
           }

           reiniciar(); //funcion que reinicia variables


        }else if(opcion==3){

            clasificatoria();

        }else if(opcion==4){

            instrucciones();

        }

    }

    allegro_exit();
   return 0;
}
END_OF_MAIN();

void guardarNombre(){
    int cont=0;
    if(ganar){
        leer.open("ganadores.txt",ios::in);
        escribir.open("auxi.txt",ios::out);
        if(leer.is_open()){
            escribir<<vect<<endl;
            escribir<<hor<<":"<<m<<":"<<s<<endl;
            cont=cont+2;
            leer.getline(nombre,18);
            while(!leer.eof()&&cont<=10){
                    escribir<<nombre<<endl;
                    leer.getline(nombre,18);
                    cont++;

            }
        }else{
            textprintf(Buffer,font,200,200,10,"Error fichero ganadores",allegro_error);
        }
        leer.close();
        escribir.close();
        remove("ganadores.txt");
        rename("auxi.txt","ganadores.txt");
    }else{
        leer.open("participantes.txt",ios::in);
        escribir.open("auxi.txt",ios::out);
        if(leer.is_open()){
            escribir<<vect<<endl;
            cont++;
            leer.getline(nombre,18);
            while(!leer.eof()&&cont<=10){
                    escribir<<nombre<<endl;
                    cont++;
                    leer.getline(nombre,18);
            }
        }else{
            textprintf(Buffer,font,200,200,10,"Error fichero participantes",allegro_error);
        }
        leer.close();
        escribir.close();
        remove("participantes.txt");
        rename("auxi.txt","participantes.txt");
    }
}

void clasificatoria(){
    int y,x,a;

    text_mode(-1);
    clear_to_color(Buffer,makecol(255,255,255));
    textprintf(Buffer,times,10,20,10,"Ganadores Recientes");
    textprintf(Buffer,times,200,20,10,"Tiempos");
    textprintf(Buffer,times,530,450,makecol(255,0,0),"ESC para ir atras");
    leer.open("ganadores.txt",ios::in);
    if(leer.is_open()){
        y=70;
        leer.getline(nombre,18);
        while(!leer.eof()){
            a=0;
            x=10;
            while(nombre[a]!='\0'){
                textprintf(Buffer,times,x,y,10,"%c",nombre[a]);
                a++;
                x=x+8;
            }
            leer.getline(nombre,18);
            a=0;
            x=200;
            while(nombre[a]!='\0'){
                textprintf(Buffer,times,x,y,10,"%c",nombre[a]);
                a++;
                x=x+8;
            }
            leer.getline(nombre,18);
            y=y+12;
        }
    }else{
        textprintf(screen,font,200,200,10,"Error al cargar archivos",allegro_error);
    }
    leer.close();
    leer.open("participantes.txt",ios::in);
    if(leer.is_open()){
        leer.getline(nombre,18);
        textprintf(Buffer,times,290,20,10,"Participantes Recientes");
        y=40;
        while(!leer.eof()){
            a=0;
            x=290;
            while(nombre[a]!='\0'){
                textprintf(Buffer,times,x,y,10,"%c",nombre[a]);
                a++;
                x=x+8;
            }
            leer.getline(nombre,18);
            y=y+12;
        }
    }else{
        textprintf(screen,font,200,200,10,"Error al cargar archivos",allegro_error);
    }
    leer.close();
    line(Buffer,275,20,275,460,makecol(0, 0, 0));
    while(!key[KEY_ESC]){
        draw_sprite(screen,Buffer,0,0);
    }
}

void instrucciones(){
    draw_sprite(Buffer,instruc,0,0);
    textprintf(Buffer,times,530,460,makecol(255,0,0),"ESC para salir");
    while(!key[KEY_ESC]){
        draw_sprite(screen,Buffer,0,0);
    }
}

void Gana(){
    int auxiliar=1;
    clear_to_color(screen,makecol(0, 0, 0));
    while(!key[KEY_ESC]&&auxiliar){
        if(mouse_x > 260 && mouse_x< 410 && mouse_y >280 && mouse_y < 320){
            draw_sprite(Buffer,ganarI,0,0);
            if( mouse_b & 1){
                auxiliar=0;
                position_mouse(10,10);
            }
        }else{
            draw_sprite(Buffer,ganarM,0,0);
        }
        masked_blit(cursor,Buffer,0,0,mouse_x,mouse_y,13,22);
        draw_sprite(screen,Buffer,0,0);
    }
}

void perder(){
    int auxiliar=1;
    clear_to_color(screen,makecol(0, 0, 0));
    while(auxiliar){
        if(mouse_x > 175 && mouse_x< 475 && mouse_y >175 && mouse_y < 245){
            draw_sprite(Buffer,perder2,0,0);
            if( mouse_b & 1){
                auxiliar=0;
                reintentar=1;
                position_mouse(10,50);
            }
        }else if(mouse_x > 210 && mouse_x< 430 && mouse_y >285 && mouse_y < 330){
            draw_sprite(Buffer,perder3,0,0);
            if( mouse_b && 1){
                auxiliar=0;
                reintentar=0;
                position_mouse(10,10);
            }
        }else{
            draw_sprite(Buffer,perder1,0,0);
        }
        masked_blit(cursor,Buffer,0,0,mouse_x,mouse_y,13,22);
        draw_sprite(screen,Buffer,0,0);
    }
}

void PedirNombre(){
    char letra;
    int tam=-1,auxi=1,aux2=0,aux3;
    clear_to_color(Buffer,makecol(255,255,255));
    clear_keybuf();
    while(!key[KEY_ESC]&&auxi){

        textprintf(Buffer,viner,50,50,8,"Usuario: ");
        textprintf(Buffer,times,50,100,8,"Una vez escriba su nombre Presione:"); //cambiar tip de letra
        textprintf(Buffer,times,50,115,8,"ENTER para seguir con el juego");
        textprintf(Buffer,times,50,130,8,"ESC para ir a menu");
        draw_sprite(screen,Buffer,0,0);
        clear_to_color(Buffer,makecol(255,255,255));

        if(tam<15){
            letra=readkey()>>8;

            switch(letra){
                case KEY_A:
                    tam++;
                    vect[tam]='A';
                break;
                case KEY_B:
                    tam++;
                    vect[tam]='B';
                break;
                case KEY_C:
                    tam++;
                    vect[tam]='C';
                break;
                case KEY_D:
                    tam++;
                    vect[tam]='D';
                break;
                case KEY_E:
                    tam++;
                    vect[tam]='E';
                break;
                case KEY_F:
                    tam++;
                    vect[tam]='F';
                break;
                case KEY_G:
                    tam++;
                    vect[tam]='G';
                break;
                case KEY_H:
                    tam++;
                    vect[tam]='H';
                break;
                case KEY_I:
                    tam++;
                    vect[tam]='I';
                break;
                case KEY_J:
                    tam++;
                    vect[tam]='J';
                break;
                case KEY_K:
                    tam++;
                    vect[tam]='K';
                break;
                case KEY_L:
                    tam++;
                    vect[tam]='L';
                break;
                case KEY_M:
                    tam++;
                    vect[tam]='M';
                break;
                case KEY_N:
                    tam++;
                    vect[tam]='N';
                break;
                case KEY_O:
                    tam++;
                    vect[tam]='O';
                break;
                case KEY_P:
                    tam++;
                    vect[tam]='P';
                break;
                case KEY_Q:
                    tam++;
                    vect[tam]='Q';
                break;
                case KEY_R:
                    tam++;
                    vect[tam]='R';
                break;
                case KEY_S:
                    tam++;
                    vect[tam]='S';
                break;
                case KEY_T:
                    tam++;
                    vect[tam]='T';
                break;
                case KEY_U:
                    tam++;
                    vect[tam]='U';
                break;
                case KEY_V:
                    tam++;
                    vect[tam]='V';
                break;
                case KEY_W:
                    tam++;
                    vect[tam]='W';
                break;
                case KEY_X:
                    tam++;
                    vect[tam]='X';
                break;
                case KEY_Y:
                    tam++;
                    vect[tam]='Y';
                break;
                case KEY_Z:
                    tam++;
                    vect[tam]='Z';
                break;
                case KEY_BACKSPACE:
                if(tam>=0){
                    tam--;
                }
                break;
                case KEY_SPACE:
                    tam++;
                    vect[tam]=' ';
                break;
                case KEY_ENTER:
                auxi=0;
                break;

            }
        }else{
            textprintf(Buffer,font,200,80,makecol(240,15,15),"Cantidad de caracteres excedida, Maximo 15");
            if((readkey()>>8)==KEY_BACKSPACE){
                tam--;
            }
            if((readkey()>>8)==KEY_ENTER){
                auxi=0;
            }
        }
        aux3=180;
        aux2=0;
        while(aux2<=tam){
            textprintf(Buffer,font,aux3,65,8,"%c",vect[aux2]);
            aux3=aux3+10;
            aux2++;
        }
    }
    vect[tam+1]='\0';
}

void cargarMaterial(){

   Buffer= create_bitmap(anchura,altura);
   strella=load_bitmap("estrella.bmp",NULL);
   lobo=load_bitmap("Lobo.bmp",NULL);
   iluminado=load_bitmap("iluminado.bmp",NULL);
   conejo=load_bitmap("rabbit-.bmp",NULL);
   fallecido=load_bitmap("Fallecido.bmp",NULL);
   cursor= load_bitmap("cursor.bmp",NULL);
   argusto1=load_bitmap("ll.bmp",NULL);
   argusto2=load_bitmap("lll.bmp",NULL);
   argusto3=load_bitmap("llll.bmp",NULL);
   ganarI=load_bitmap("ganar.bmp",NULL);
   ganarM=load_bitmap("ganar-m.bmp",NULL);
   perder1=load_bitmap("perder1.bmp",NULL);
   perder2=load_bitmap("perder2.bmp",NULL);
   perder3=load_bitmap("perder3.bmp",NULL);
   instruc=load_bitmap("instruc.bmp",NULL);
   fondo=load_bitmap("Fondo1.bmp",NULL);
   muerto=load_bitmap("Difunto.bmp",NULL);
   m1=load_bitmap("M1.bmp",NULL);
   m2=load_bitmap("M2.bmp",NULL);
   m3=load_bitmap("M3.bmp",NULL);
   m4=load_bitmap("M4.bmp",NULL);
   m5=load_bitmap("M5.bmp",NULL);

   Mjuego=load_midi("Mjuego.mid");
   Mperder=load_midi("Mperder.mid");

   font=load_font("forte.pcx",NULL,NULL);
   times=load_font("Times.pcx",NULL,NULL);
   viner=load_font("VINER.pcx",NULL,NULL);
}

void reiniciar(){
    ganar=false;
    muertes=0;
    turno=0;
    hor=0;
    m=0;
    s=0;
    ml=0;
    opcion=0;
    turno=0;
}

void cronometro(){
    ml++;
    if(ml>3){
        s++;
        ml=0;
        if(s>60){
            m++;
            s=0;
            if(m>60){
                hor++;
                m=0;
            }
        }
    }
}

int menuInicio(){
    int salida=0;
    while(!salida){
        if(mouse_x> 235 && mouse_x< 405 && mouse_y >175 && mouse_y < 210){
            draw_sprite(Buffer,m2,0,0);
            if(mouse_b & 1){
                salida=2;
                return salida;
            }
        }else if(mouse_x >150 && mouse_x <490 && mouse_y> 225 && mouse_y< 270){
            draw_sprite(Buffer,m3,0,0);
            if(mouse_b & 1){
                salida=3;
                return salida;
            }
        }else if(mouse_x >140 && mouse_x <500 && mouse_y> 300 && mouse_y< 335){
            draw_sprite(Buffer,m4,0,0);
            if(mouse_b & 1){
                salida=4;
                return salida;
            }
        }else if(mouse_x >230 && mouse_x <405 && mouse_y> 360 && mouse_y< 400){
            blit(m5,Buffer,0,0,0,0,1080,680);
            if(mouse_b & 1){
                salida=1;
                return salida;
            }
        }else{
            draw_sprite(Buffer,m1,0,0);
        }
        masked_blit(cursor,Buffer,0,0,mouse_x,mouse_y,13,22);
        draw_sprite(screen,Buffer,0,0);
    }
}
//funcion para comprobar enlaces
void imprime(){
    z=ListaNodo;
    while(z!=NULL){
        cout<<z->x<<"-"<<z->y<<"**";
        j=z->sigA;
        while(j!=NULL){
            cout<<j->sigN->x<<"-"<<j->sigN->y<<" ";
            j=j->sigAr;
        }
        cout<<endl;
        z=z->sig;
    }
}

void MovLobos(){
    if((mouse_b & 1)&&turno==0){
        w=buscarAux(mouse_x+20, mouse_x-20, mouse_y+20, mouse_y-20);
        if(w!=NULL&&(w->st==1)){
            z=buscarST(2);
            if(z!=NULL){
                z->st=1;
            }
            w->st=2;
        }
    }
}

void mostrarAdyacentes(nodo *N){
    g=N->sigA;//me posiciono en la sublista
    while(g!=NULL){
        if(g->sigN->st==0&&(!turno)){//pregunto si esta vacio
            circlefill(Buffer,g->sigN->x ,g->sigN->y, 20 , makecol(210, 0, 0));//pinto circulo
            if(mouse_b & 1){
                w=buscarAux(mouse_x+20, mouse_x-20, mouse_y+20, mouse_y-20);//busco sobre cual circulo hizo click
                if(g->sigN==w){ //parte del movimiento del lobo
                    N->st=0;
                    w->st=1;
                    turno=1;
                }
            }
        }
        g=g->sigAr;
    }
}

nodo *buscarAux(int xlimS,int xlimI, int ylimS, int ylimI){
    p=ListaNodo;
    while(p!=NULL){
        if((xlimS>p->x&&p->x>xlimI)&&(ylimS>p->y&&p->y>ylimI)){
            return p;
        }
        p=p->sig;
    }
    return NULL;
}

void PosIni(){
    w=ListaNodo;
    while(w!=NULL){
        if(w->x<=590&&w->y<=130){
            w->st=1;
        }else if((w->x==150&&w->y==240)||(w->x==590&&w->y==240)){
            w->st=1;
        }else if(w->x==370&&w->y==240){
            w->st=-1;
        }else{
            w->st=0;
        }
        w=w->sig;
    }
}

void pintarObjetos(){
    z=ListaNodo;
    draw_sprite(Buffer,argusto1,170,40);
    draw_sprite(Buffer,argusto1,490,300);
    draw_sprite(Buffer,argusto1,520,300);
    draw_sprite(Buffer,argusto1,160,320);
    draw_sprite(Buffer,argusto2,280,270);
    draw_sprite(Buffer,argusto3,70,360);
    draw_sprite(Buffer,argusto2,490,30);
    while(z!=NULL){
        if(z->st==1){
            draw_sprite(Buffer,lobo,z->x-50,z->y-50);
        }else if(z->st==-1){
            draw_sprite(Buffer,conejo,z->x-50,z->y-50);
        }else if(z->st==2){
            draw_sprite(Buffer,iluminado,z->x-50,z->y-50);
            mostrarAdyacentes(z);
        }else if(z->st==3){
            draw_sprite(Buffer,muerto,z->x-50,z->y-50);
        }

        if(muertes>0){
            draw_sprite(Buffer,fallecido,10,40);
        }

        z=z->sig;
    }
    //quitar lobo muerto
    p=buscarST(3);
    if(p!=NULL){
        i=0;
        while(i<10){
            draw_sprite(screen,Buffer,0,0);
            rest(10);
            i++;
        }
        p->st=0;
    }
    //espacio para decoracion

    //pintar cursor
    masked_blit(cursor,Buffer,0,0,mouse_x,mouse_y,13,22);
}

void evitarlobos(){
    if((g->sigN->st==0)&&(buscaLobos(g->sigN)==0)&&(turno)){ //si no hay lobos al rededor
        r->st=0;
        g->sigN->st=-1;
        turno =0;
    }
}

void loboCol(){
    if((r->x==w->x)&&turno){ //pregunto si esta en la misma columna
        if(r->y<w->y){
            k=buscar(ListaNodo,w->x,w->y+110);
            if(k!=NULL){
                if(k->st==0){
                    r->st=0;
                    w->st=3;
                    k->st=-1;
                    turno=0;
                    muertes++;
                }
            }
        }else{
            k=buscar(ListaNodo,w->x,w->y-110);
            if(k!=NULL){
                if(k->st==0){
                    r->st=0;
                    w->st=3;
                    k->st=-1;
                    turno=0;
                    muertes++;
                }
            }
        }
    }

}

void loboFil(){
    if((r->y==w->y)&&turno){//pregunto si está en la misma fila
        if(r->x<w->x){
            k=buscar(ListaNodo,w->x+110,w->y);
            if(k!=NULL){
                if(k->st==0){
                    r->st=0;
                    w->st=3;
                    k->st=-1;
                    turno=0;
                    muertes++;
                }
            }
        }else{
            k=buscar(ListaNodo,w->x-110,w->y);
            if(k!=NULL){
                if(k->st==0){
                    r->st=0;
                    w->st=3;
                    k->st=-1;
                    turno=0;
                    muertes++;
                }
            }
        }
    }
}

void diagonalSupIzq(){
    if(((w->x==r->x-110)&&(w->y==r->y-110))&&turno){ //las siguientes 56 lineas de codigo balidad que haya masacre en diagonal
        j=w->sigA;
        while(j!=NULL&&turno){
            if((w->x>j->sigN->x)&&(w->y>j->sigN->y)){
                if(j->sigN->st==0){
                    j->sigN->st=-1;
                    w->st=3;
                    r->st=0;
                    turno=0;
                    muertes++;
                }
            }
            j=j->sigAr;
        }
    }
}

void diagonalSupDer(){
    if((w->x==r->x+110)&&(w->y==r->y-110)&&turno){//2
        j=w->sigA;
        while(j!=NULL&&turno){
            if(w->x<j->sigN->x&&(w->y>j->sigN->y)){
                if(j->sigN->st==0){
                    j->sigN->st=-1;
                    w->st=3;
                    r->st=0;
                    turno=0;
                    muertes++;
                }
            }
            j=j->sigAr;
        }
    }
}

void diagonalInfIzq(){
    if((w->x==r->x-110)&&(w->y==r->y+110)&&turno){//3
        j=w->sigA;
        while(j!=NULL&&turno){
            if(w->x>j->sigN->x&&(w->y<j->sigN->y)){
                if(j->sigN->st==0){
                    j->sigN->st=-1;
                    w->st=3;
                    r->st=0;
                    turno=0;
                    muertes++;
                }
            }
        j=j->sigAr;
        }
    }
}

void diagonalInfDer(){
    if((w->x==r->x+110)&&(w->y==r->y+110)&&turno){//4
        j=w->sigA;
        while(j!=NULL&&turno){
            if(w->x<j->sigN->x&&(w->y<j->sigN->y)){
                if(j->sigN->st==0){
                    j->sigN->st=-1;
                    w->st=3;
                    r->st=0;
                    turno=0;
                    muertes++;
                }
            }
        j=j->sigAr;
        }
    }
}

void matarlobo(){
    int j;
    if((g->sigN->st==1)&&turno){ //opciones para matar lobo
        w=g->sigN;
        j=1+rand()%3;
        if(j==1){
            loboCol();
            loboFil();
            diagonalSupIzq();
            diagonalSupDer();
            diagonalInfIzq();
            diagonalInfDer();
        }else if(j==2){
            diagonalSupDer();
            loboCol();
            diagonalSupIzq();
            loboFil();
            diagonalInfDer();
            diagonalInfIzq();
        }else if(j==3){
            diagonalInfIzq();
            diagonalSupIzq();
            diagonalInfDer();
            loboFil();
            diagonalSupDer();
            loboCol();
        }
    }
}

void evaluar(){
    int jj;
    if(turno){
        r=buscarST(-1); //busco conejo
        d=buscarST(2); //busco iluminado
        if(d!=NULL){
            d->st=1; //quito iluminado
        }
        if(r!=NULL){
            g=r->sigA;
            while(g!=NULL&&turno){//busco adyacentes a conejo
                jj=1+rand()%3;
                if(jj==1){
                    matarlobo();
                    evitarlobos();
                    Mover_A_vacio();
                }else if(jj==2){
                    matarlobo();
                    Mover_A_vacio();
                    evitarlobos();
                }else if(jj==3){
                    matarlobo();
                    Mover_A_vacio();
                   // evitarlobos();
                }

                g=g->sigAr;
            }
            if(turno){
                ganar=true;
            }
        }
    }
}

void Mover_A_vacio(){
    i=0;
    while(i<8){
        vec[i]=NULL;
        i++;
    }
    if((g->sigN->st==0)&&turno){
        v=r->sigA;
        i=0;
        ii=0;
        while(v!=NULL){
            if(v->sigN->st==0){
                vec[ii]=v->sigN;
                ii++;
            }
            v=v->sigAr;
            i++;
        }
        if(ii>1){
            i=1+rand()%ii-1;
            w=vec[i];
            w->st=-1;
            r->st=0;
            turno=0;
        }
        if(turno){
            r->st=0;
            g->sigN->st=-1;
            turno =0;
        }
    }
}

int buscaLobos(nodo *&n){
    int l=0;
    h=n->sigA;
    while(h!=NULL){
        if(h->sigN->st!=0){
            l++;
        }
        h=h->sigAr;
    }
    return l;
}

nodo *buscarST(int s){
    k=ListaNodo;
    while(k!=NULL){
        if(k->st==s){
            return k;
        }
        k=k->sig;
    }
    return NULL;
}

nodo *insertarNodo(nodo *&Ap,int x, int y){
    nodo *p = new nodo;
    p->x=x;
    p->y=y;
    p->st=0;
    p->sigA=NULL;
    if(Ap==NULL){
        Ap=p;
        p->sig=NULL;
    }else{
		p->sig=Ap;
		Ap=p;
    }
    return Ap;
}

arista *crearEnlases(nodo *&N,int xf, int yf){
        arista *h = new arista;
        h->sigAr=NULL;
        w=buscar(ListaNodo,xf,yf);
        if(w==NULL){
            ListaNodo=insertarNodo(ListaNodo,xf,yf);
            w=ListaNodo;
        }
        h->sigN=w;
        if(N->sigA==NULL){
        	N->sigA=h;
		}else{
		    q=N->sigA;
		    while(q->sigAr!=NULL){
                q=q->sigAr;
		    }
			q->sigAr=h;
		}
		return N->sigA;
}

void PintarEscena(){
        //Rectangulo
        drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        set_trans_blender(0, 0, 0, 50);
        rectfill(Buffer,150,20,590,460,makecol(0, 160, 33));
        solid_mode();
        //rectfill(Buffer,150,20,590,460,makecol(0, 135, 255));
       //diagonales
       line(Buffer,150,20,590,460,makecol(R, G, B));
       line(Buffer,590,20,150,460,makecol(R, G, B));
       //verticales
       line(Buffer,150,20,150,460,makecol(R, G, B));
       line(Buffer,260,20,260,460,makecol(R, G, B));
       line(Buffer,370,20,370,460,makecol(R, G, B));
       line(Buffer,480,20,480,460,makecol(R, G, B));
       line(Buffer,590,20,590,460,makecol(R, G, B));
       //horizontales
       line(Buffer,150,20,590,20,makecol(R, G, B));
       line(Buffer,150,130,590,130,makecol(R, G, B));
       line(Buffer,150,240,590,240,makecol(R, G, B));
       line(Buffer,150,350,590,350,makecol(R, G, B));
       line(Buffer,150,460,590,460,makecol(R, G, B));
        //diagonal segundaria
       line(Buffer,370,20,150,240,makecol(R, G, B));
       line(Buffer,370,20,590,240,makecol(R, G, B));
       line(Buffer,150,240,370,460,makecol(R, G, B));
       line(Buffer,590,240,370,460,makecol(R, G, B));
}

void cargarEnlaces(){
    int x, y;
    ifstream leer;
    leer.open("grafoCoor.txt",ios::in);
    if(leer.is_open()){
        leer>>x;
        leer>>y;
        ListaNodo=insertarNodo(ListaNodo,x,y);
        k=ListaNodo;
        while(!leer.eof()){
            leer>>x;
            leer>>y;
            if(x==-1&&y==-1){
                leer>>x;
                leer>>y;
                if(leer.eof()){
                    break;
                }
                k=buscar(ListaNodo,x,y);
                if(k==NULL){
                	ListaNodo=insertarNodo(ListaNodo,x,y);
                	k=buscar(ListaNodo,x,y);
				}

                leer>>x;
                leer>>y;
            }

            k->sigA=crearEnlases(k,x,y);
        }
        leer.close();
    }else{
        textout(screen,font,"No se pudo cargar enlaces",200,200,makecol(100, 100, 80));
        getch();
    }
}

nodo *buscar(nodo *&ptr, int x, int y){
    p=ptr;
    while(p!=NULL){
        if(p->x==x && p->y==y){
            return p;
        }
        p=p->sig;
    }
    return NULL;
}
