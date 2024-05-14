/*------------------------------------------------------------------
Fichier: EngProjet.c
---------------------------------------------------------------------*/
#include <stdio.h>
#include <gng1106plplot.h>  // Donne des d�finitions pour utiliser la librarie PLplot
#include <math.h>
#include <float.h>

// Quelques d�finitions
#define N 60   // Nombre de points pour cr�er le graphique
#define TOLERANCE 1e-7         // Tol�rance de la racine calcul�
#define FAUX 0
#define VRAI 1 //
#define FICHIER_BIN  "donnee.bin"

// D�finitions de structure
typedef struct
{
    double l_min;
    double l_max;
    double r;
    double c;
    int contenu;
    double pcmin;
    double pcmax;
    double l_limite;
    double td;
} STRUCT;

// Prototypes des fonctions

int EntreeDonee(STRUCT [], int *, FILE *);
void affichevaleur(STRUCT [],int *);
void demandesauvegarde(STRUCT [],FILE *);
int limiteFct(STRUCT [], int *);
void creeTbl(STRUCT [], int *, double [], double []);
double trouveRacine(STRUCT [], int *, double);
double calculeFct(STRUCT [], int *, double, double);
void pplot(STRUCT [], int *, double [], double []);
void verifieFch(FILE *, int *);
void tblfichier(STRUCT [], FILE *, int *);
void sauvefichier(STRUCT [],FILE *);
void creebin(STRUCT [],FILE *);
/*---------------------------------------------------------------------
 Fonction: main
 Description: Cette fonction cr�e un graphique repr�sentant l'inductance
 L en fonction de pc, apr�s avoir re�u les valeurs de l'utilsateur qui
 a le choix de les enregistr�es et les r�utilis�es. Fait appel aux diff�rents
 fonctions pour faire les calculs n�cessaires.
------------------------------------------------------------------------*/
int main(void)
{
    // D�clarations des variables
    int index; // index du tableau valeur
    STRUCT valeur[5]; // tableau contenant les variable de structure
    double x[N]; // tableau contenant les valeurs de x
    double y[N]; // tableau contenant les valeurs de f(x)
    int save; // Demande de sauvegarde
    FILE *efp;
    // Permettre bonne interpr�tation des caract�res fran�ais
    system("chcp 1252 >NUL");
    save = EntreeDonee(valeur,&index,efp);
    creeTbl(valeur,&index,x,y); //Entre les valeurs dans les tableaux x et y
    affichevaleur(valeur,&index);// Affiche les valeurs entr�es
    pplot(valeur,&index,x,y); // Trace le graphe
    if (save !=1) // Fait demande de sauvegarde si nouvelles donn�es entr�es
        demandesauvegarde(valeur,efp); // Demande de sauvegarder le fichier
    return(0);
}

/*---------------------------------------------------------------------
 Fonction: EntreeDonee
 Param�tres :
 valeur : R�f�rence au tableau de structure pour stocker les valeurs entr�es par l�utilisateur .
 index : Pointeur � la variable contenant l�index de l��l�ment du tableau utilis� pour tracer le graphique.
 efp : pointeur au fichier d�entr�e.
 Valeur de retour: Retourne 1 si le programme ne fait pas un demande de sauvegarder
 les valeurs � l�utilisateur.
 Description: Cette fonction v�rifie en premier lieu si un fichier est d�j� existant.
  Si le fichier n�existe pas, un nouveau fichier est cr�� en faisant appel � creebin
  et donner de nouvelles entr�es. Sinon la liste du contenu des �l�ments du tableau
  est affich�e (avec un message tel que � disponible � pour les �l�ments vide).
  L�utilisateur est alors invit� � choisir un ensemble de valeurs existantes ou
  d�offrir de nouvelles valeurs. Les valeurs entr�es sont v�rifi�es pour s�assurer
  de leur validit�. S�ils ne le sont pas, les valeurs sont rejet�es et
  l�utilisateur est invit� � fournir de nouvelles valeurs. Ainsi les donn�es
  d�entr�e sont assign�es � chaque membre de la variable de structure du premier
  �l�ment vide du tableau.

------------------------------------------------------------------------*/
int EntreeDonee(STRUCT valeur[],int *index,FILE *efp)
{
    // D�claration des variables
    int choix; //choix de l'utilisateur (entrer ou choisir des donn�es)
    int save; // demande de sauvegarde
    int ix;
    int d;
    // Initialisation des variables
    choix = 0;
    save = 0;
    // V�rifie fichier
    verifieFch(efp,&save);
    if (save==-1) // fichier inexistant
    {
        creebin(valeur,efp); // cr�e fichier
    }

    tblfichier(valeur,efp,index); //Entr�e les valeurs du fichier dans le tableau
    for(ix=0; ix<5; ix = ix + 1) //Afficher �lement libre ou sauvegard�
    {
        if(valeur[ix].contenu == 0)
        {
            printf("Emplacement %d : Disponible \n",ix+1);
            printf("\n");
        }
        else
        {
            printf("Emplacement %d : Les valeurs sauvegard�s \n",ix+1);
            printf("La valeur de la r�sistance R : %.10f \n",valeur[ix].r);
            printf("La valeur de la capacit� C : %.10f \n",valeur[ix].c);
            printf("La valeur minimal de pc : %.3f \n",valeur[ix].pcmin);
            printf("La valeur maximal de pc : %.3f \n",valeur[ix].pcmax);
            printf("La valeur du temps de dissipation : %.10f \n",valeur[ix].td);
            printf("\n");
        }
    }

    if(*index!=0) // Demande choix de l'utilisateur si fichier non vide
    {
        do
        {
            printf("Voulez-vous entrer des nouvelles valeurs (0) ou choisir des valeurs sauvegard�es(1) ? R�pondre par 0 ou 1 \n");
            scanf("%d",&choix);
        }
        while(choix!= 0 && choix!= 1);
    }

    if(*index==5 && choix==0) // Demande de remplacer si fichier plein
    {
        do
        {
            printf("Dans quel �l�ment voulez-vous remplacer les valeurs ");
            scanf("%d",&d);
        }
        while(d!=1 && d!=2 && d!=3 && d!=4 && d!=5);
        *index = d-1;
    }

    if(choix==0 || *index==0) // Demande d'entrer les valeurs si possible
    {
        do
        {
            printf("Veuillez entrer des valeurs valides. Sinon veuillez r�ssayer \n");
            do
            {
                printf("Veuillez entrer une valeur de la r�sistance R : ");
                scanf("%lf",&valeur[*index].r);
            }
            while(valeur[*index].r <= 0); //V�rifie entr�e

            do
            {
                printf("Veuillez entrer une valeur de la capacit� C : ");
                scanf("%lf",&valeur[*index].c);
            }
            while(valeur[*index].c <= 0); //V�rifie entr�e

            do
            {
                printf("Veuillez entrer une valeur de pc minimal : ");
                scanf("%lf",&valeur[*index].pcmin);
            }
            while(valeur[*index].pcmin <= 0 || valeur[*index].pcmin >= 1); //V�rifie entr�e

            do
            {
                printf("Veuillez entrer une valeur de pc maximal : ");
                scanf("%lf",&valeur[*index].pcmax); //V�rifie entr�e

            }
            while(valeur[*index].pcmax >= 1 || valeur[*index].pcmax <= valeur[*index].pcmin);

            do
            {
                printf("Veuillez entrer une valeur du temps de dissipation: ");
                scanf("%lf",&valeur[*index].td);
            }
            while(valeur[*index].td <= 0); //V�rifie entr�e

        }
        while(limiteFct(valeur,index)==FAUX); // V�rifie validit� des entr�es.
        valeur[*index].contenu = VRAI ; // Element plein
    }

    if (choix==1) // Recevoir choix d'�lement d�j� sauvegard�
    {
        do
        {
            printf("Choisissez l'�l�ment contenant les valeurs d�sir�es ");
            scanf("%d",&d);
            d = d-1 ;
        }
        while(d>4 || d<0 ||valeur[d].contenu==0);
        *index = d ; // Assigne index de l'�lement choisis
        save = 1 ;
    }

    return(save);
}
/*---------------------------------------------------------------------
 Fonction: verifieFch
 Param�tres :
 efp : Pointeur au fichier � ouvrir.
 choix : Pointeur � la variable choix d�terminant le choix de l�utilisateur.
 Description: Cette fonction est responsable de v�rifier si un fichier existe ou pas.
------------------------------------------------------------------------*/
void verifieFch(FILE *efp,int *save)
{
    efp = fopen(FICHIER_BIN, "rb");
    if(efp == NULL)
    {
        printf("Aucun fichier binaire %s existant \n", FICHIER_BIN);
        printf("Un nouveau fichier sera cr�� \n \n", FICHIER_BIN);
        *save = -1 ; // Affecte -1 pour cr�er fichier
    }
    fclose(efp);
}
/*---------------------------------------------------------------------
 Fonction: tblfichier
 Param�tres :
 valeur : R�f�rence au tableau de structure contenant l�ensemble des valeurs.
 efp : Pointeur au fichier � ouvrir.
 index : Pointeur � la variable contenant l�index de l��l�ment du tableau utilis� pour tracer le graphique.
 Description: Cette fonction est responsable de lire le contenu du fichier
 et l�affecter au tableau de structure.
------------------------------------------------------------------------*/
void tblfichier(STRUCT valeur[], FILE *efp, int *index)
{
    // D�claration et initialisation des variables
    int ix=0;
    int flag=VRAI;
    efp = fopen(FICHIER_BIN, "rb");
    fread(valeur,sizeof(STRUCT),5,efp);
    fclose(efp);
    *index = 5;
    while(flag==VRAI && ix<5) //Assigne index au premier �l�ment vide du tableau
    {
        if (valeur[ix].contenu==0)
        {
            *index = ix;
            flag = FAUX;
        }
        ix = ix + 1;
    }
}

/*---------------------------------------------------------------------
 Fonction: limiteFct
 Valeur de retour:
 Retourne VRAI si les valeurs sont valides et FAUX s�ils ne le sont pas.
 Param�tres :
 valeur : R�f�rence au tableau de structure contenant l�ensemble des valeurs.
 index : Pointeur � la variable contenant l�index de l��l�ment du tableau utilis� pour tracer le graphique.
 Description: V�rifie la validit� des valeurs entr�es en calculant une limite inf�rieur de l�inducteur.
------------------------------------------------------------------------*/
int limiteFct(STRUCT valeur[], int *index)
{
    // D�claration et initialisation des variables
    int flag=VRAI;
    valeur[*index].l_limite = (valeur[*index].c*pow(valeur[*index].r,2))/4 ; // Valeur limite
    valeur[*index].l_min = (-valeur[*index].r*valeur[*index].td)/(2*log(valeur[*index].pcmin)); // Valeur minimal
    valeur[*index].l_max = (-valeur[*index].r*valeur[*index].td)/(2*log(valeur[*index].pcmax)); // Valeur maximal
    if(valeur[*index].l_min < valeur[*index].l_limite)
        flag = FAUX;
    return(flag);

}

/*---------------------------------------------------------------------
 Fonction: creeTbl
 Param�tres :
 valeur : R�f�rence au tableau de structure contenant l�ensemble des valeurs.
 index : Pointeur � la variable contenant l�index de l��l�ment du tableau utilis� pour tracer le graphique.
 x : R�f�rence au tableau des points d�abscisses o� les valeurs de pc vont �tre stock�s.
 y : R�f�rence au tableau des points d�ordonn�s o� les valeurs de L vont �tre stock�s.
 Description: Cette fonction affecte les valeurs de pc au tableau x et les valeurs de L au tableau y.
------------------------------------------------------------------------*/
void creeTbl(STRUCT valeur[], int *index, double x[], double y[])
{
    // D�claration des variables
    double pc;
    double inc;
    int ix;
    // Initialisation des variables
    pc = valeur[*index].pcmin;
    inc = (valeur[*index].pcmax - valeur[*index].pcmin)/N ; //valeur de l'incr�mentation
    x[0] = pc; // 1er �l�ment du tableau x
    y[0] = valeur[*index].l_min; // 1er �l�ment du tableau y
    x[N-1]= valeur[*index].pcmax; // Dernier �l�ment du tableau x
    y[N-1]= valeur[*index].l_max; // Dernier �l�ment du talbeau y
    for(ix=1; ix < N-1; ix = ix + 1)
    {
        pc = pc + inc; // Incr�mentation
        x[ix] = pc;
        y[ix] = trouveRacine(valeur,index,pc); //Calcul de l par bissection
    }
}

/*---------------------------------------------------------------------
 Fonction: trouveRacine
 Valeur de retour:
 Retourne la valeur de la racine lmoy.
 Param�tres :
 valeur : R�f�rence au tableau de structure contenant l�ensemble des valeurs.
 index : Pointeur � la variable contenant l�index de l��l�ment du tableau utilis� pour tracer le graphique.
 pc : Variable contenant la valeur du pourcentage de la charge.
 Description: Utilise la m�thode de bissection afin de trouver la racine de l��quation g(L)=exp(-Rtd/2L) � pc.
------------------------------------------------------------------------*/
double trouveRacine(STRUCT valeur[], int *index, double pc)
{
    // D�claration des variables
    double lmin;
    double lmax;
    double lmoy;
    double finc;
    int flag;
    // Initialisation des variables
    flag = FAUX;
    lmin = valeur[*index].l_min;
    lmax = valeur[*index].l_max; //Intervalle de bissection
    while(flag==FAUX)
    {
        lmoy = (lmax + lmin)/2; //Calcul de valeur moyenne

        if (fabs(calculeFct(valeur,index,lmin,pc)*calculeFct(valeur,index,lmoy,pc))<= TOLERANCE) //Si racine trouv�e
            flag = VRAI;

        else if (calculeFct(valeur,index,lmin,pc)*calculeFct(valeur,index,lmoy,pc)<0) //Si racine dans moiti� inf�rieure
            lmax = lmoy;

        else if (calculeFct(valeur,index,lmin,pc)*calculeFct(valeur,index,lmoy,pc)>0) //Si racine dans moiti� sup�rieure
            lmin = lmoy;
    }
    return(lmoy); // Racine retourn�e

}
/*---------------------------------------------------------------------
 Fonction: calculeFct
 Valeur de retour:
 Retourne la valeur de g(L)
 Param�tres :
 valeur : R�f�rence au tableau de structure contenant l�ensemble des valeurs.
 index : Pointeur � la variable contenant l�index de l��l�ment du tableau utilis� pour tracer le graphique.
 l : Variable contenant la valeur de l�inductance.
 pc : Variable contenant la valeur du pourcentage de la charge.
 Description: Cette fonction r�alise une cha�ne de calcule pour trouver le r�sultat de l��quation g(L)=exp(-Rtd/2L) � pc.
------------------------------------------------------------------------*/
double calculeFct(STRUCT valeur[], int *index, double l, double pc)
{
    // D�claration des variables
    double gl;
    // Initialisation des variables
    gl = (-valeur[*index].r*valeur[*index].td)/(2*l);
    gl = exp(gl);
    gl = gl - pc ;
    return(gl);
}

/*---------------------------------------------------------------------
 Fonction: affichevaleur
 Param�tres :
 valeur : R�f�rence au tableau de structure contenant l�ensemble des valeurs.
 index : Pointeur � la variable contenant l�index de l��l�ment du tableau utilis� pour tracer le graphique.
 Description: Cette fonction affiche les valeurs donn�es par l�utilisateur.
------------------------------------------------------------------------*/
void affichevaleur(STRUCT valeur[],int *index)
{
    //Affiche valeurs
    printf("Ceci est un graphique repr�sentant l'inductance L en fonction de pc pour : \n");
    printf("R = %.10f",valeur[*index].r);
    printf("     C = %.10f",valeur[*index].c);
    printf("     pcmin = %.3f",valeur[*index].pcmin);
    printf("     pcmax = %.3f",valeur[*index].pcmax);
    printf("     td = %.10f \n",valeur[*index].td);
}

/*-----------------------------------------------------------------------
Fonction: creebin
Param�tres :
valeur : R�f�rence au tableau de structure contenant l�ensemble des valeurs.
efp : Pointeur au fichier � ouvrir.
Description:  Cette fonction permet de cr�er un fichier binaire vide dans le cas o� celui-ci n�existe pas.
------------------------------------------------------------------------*/
void creebin(STRUCT valeur[],FILE *efp)
{
    // D�claration des variables
    int ix;
    // Initialisation des variables
    for(ix=0; ix < 5; ix = ix+1)
    {
        valeur[ix].contenu = 0 ; // Assigner �l�ment du tableau comme vide
    }
    efp = fopen(FICHIER_BIN, "wb");
    fwrite(valeur,sizeof(STRUCT),5,efp);
    fclose(efp);
}

/*-------------------------------------------------
 Fonction: pplot()
 valeur : R�f�rence au tableau de structure contenant l�ensemble des valeurs.
 index : Pointeur � la variable contenant l�index de l��l�ment du tableau utilis� pour tracer le graphique.
 x : R�f�rence au tableau des points d�abscisses o� les valeurs de pc vont �tre stock�s.
 y : R�f�rence au tableau des points d�ordonn�s o� les valeurs de L vont �tre stock�s.
Description: Initialise le graphique.  La taille des axes est d�finie par:
             pcmin, pcmax   Axe horixontal
             minFx, maxFx   Axe vertical
             Il est suppos� queles valeurs de la rang�e x sont tri�es en ordre croissant.
             La courbe de f(x) est trac� avec le contenu de chaque rang�e,
             c.-�-d. x  et  y qui sont tous deux des r�f�reces � des tableaux 1D.
-------------------------------------------------*/
void pplot(STRUCT valeur[], int *index, double x[], double y[])
{
    // D�claration des variables
    double minFx, maxFx;  // Valuers minimum et maximum de f(x)
    // Initialisation du graphique
    plsdev("wingcc");  // Choisi l'appareil wingcc - le comilateur de CodeBlocks
    plinit();
    // Configurer les axes et les �tiquettes du graphique
    plwidth(3);   // s�lectionner la largeur de plume
    minFx = valeur[*index].l_min;
    maxFx = valeur[*index].l_max;
    plenv(valeur[*index].pcmin,valeur[*index].pcmax,
          minFx, maxFx, 0, 0);
    plcol0(BLACK);    // Couleur des �ttiquettes
    pllab("pc(secondes)", "L(Henry)", "Graphique de l'inductance L versus pc");
    // Trace la fonction sur le graphique
    plcol0(BLUE);    // Couleur de la trace
    plline(N, x, y);
    plend();
}

/*-----------------------------------------------------------------------
Fonction: demandesauvegarde
Param�tres :
valeur : R�f�rence au tableau de structure contenant l�ensemble des valeurs.
efp : Pointeur au fichier � ouvrir.
Description: La fonction demande � l�utilisateur s�il veut sauvegarder les donn�es ajout�es.
------------------------------------------------------------------------*/
void demandesauvegarde(STRUCT valeur[],FILE *efp)
{
    // D�claration des variables
    int flag;
    // Demande de sauvegarde
    do
    {
        printf("Voulez-vous enregistrer les donn�s entr�es ? R�pondre 1 pour oui et 0 pour non \n");
        scanf("%d",&flag);
    }while(flag!=0 && flag!=1); //Verifie entr�e
    if (flag=1)
    {
        sauvefichier(valeur,efp); //Sauvegarde fichier
    }


}
/*-----------------------------------------------------------------------
Fonction: sauvefichier
Param�tres :
valeur : R�f�rence au tableau de structure contenant l�ensemble des valeurs.
efp : Pointeur au fichier � ouvrir.
Description: Cette fonction permet d��crire le contenu  du tableau dans le fichier.
------------------------------------------------------------------------*/
void sauvefichier(STRUCT valeur[],FILE *efp)
{
    efp = fopen(FICHIER_BIN, "wb");
    rewind(efp);
    fwrite(valeur,sizeof(STRUCT),5,efp);
    fclose(efp);
}
