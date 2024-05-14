/*------------------------------------------------------------------
Fichier: EngProjet.c
---------------------------------------------------------------------*/
#include <stdio.h>
#include <gng1106plplot.h>  // Donne des définitions pour utiliser la librarie PLplot
#include <math.h>
#include <float.h>

// Quelques définitions
#define N 60   // Nombre de points pour créer le graphique
#define TOLERANCE 1e-7         // Tolérance de la racine calculé
#define FAUX 0
#define VRAI 1 //
#define FICHIER_BIN  "donnee.bin"

// Définitions de structure
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
 Description: Cette fonction crée un graphique représentant l'inductance
 L en fonction de pc, aprés avoir reçu les valeurs de l'utilsateur qui
 a le choix de les enregistrées et les réutilisées. Fait appel aux différents
 fonctions pour faire les calculs nécessaires.
------------------------------------------------------------------------*/
int main(void)
{
    // Déclarations des variables
    int index; // index du tableau valeur
    STRUCT valeur[5]; // tableau contenant les variable de structure
    double x[N]; // tableau contenant les valeurs de x
    double y[N]; // tableau contenant les valeurs de f(x)
    int save; // Demande de sauvegarde
    FILE *efp;
    // Permettre bonne interprétation des caractères français
    system("chcp 1252 >NUL");
    save = EntreeDonee(valeur,&index,efp);
    creeTbl(valeur,&index,x,y); //Entre les valeurs dans les tableaux x et y
    affichevaleur(valeur,&index);// Affiche les valeurs entrées
    pplot(valeur,&index,x,y); // Trace le graphe
    if (save !=1) // Fait demande de sauvegarde si nouvelles données entrées
        demandesauvegarde(valeur,efp); // Demande de sauvegarder le fichier
    return(0);
}

/*---------------------------------------------------------------------
 Fonction: EntreeDonee
 Paramètres :
 valeur : Référence au tableau de structure pour stocker les valeurs entrées par l’utilisateur .
 index : Pointeur à la variable contenant l’index de l’élément du tableau utilisé pour tracer le graphique.
 efp : pointeur au fichier d’entrée.
 Valeur de retour: Retourne 1 si le programme ne fait pas un demande de sauvegarder
 les valeurs à l’utilisateur.
 Description: Cette fonction vérifie en premier lieu si un fichier est déjà existant.
  Si le fichier n’existe pas, un nouveau fichier est créé en faisant appel à creebin
  et donner de nouvelles entrées. Sinon la liste du contenu des éléments du tableau
  est affichée (avec un message tel que « disponible » pour les éléments vide).
  L’utilisateur est alors invité à choisir un ensemble de valeurs existantes ou
  d’offrir de nouvelles valeurs. Les valeurs entrées sont vérifiées pour s’assurer
  de leur validité. S’ils ne le sont pas, les valeurs sont rejetées et
  l’utilisateur est invité à fournir de nouvelles valeurs. Ainsi les données
  d’entrée sont assignées à chaque membre de la variable de structure du premier
  élément vide du tableau.

------------------------------------------------------------------------*/
int EntreeDonee(STRUCT valeur[],int *index,FILE *efp)
{
    // Déclaration des variables
    int choix; //choix de l'utilisateur (entrer ou choisir des données)
    int save; // demande de sauvegarde
    int ix;
    int d;
    // Initialisation des variables
    choix = 0;
    save = 0;
    // Vérifie fichier
    verifieFch(efp,&save);
    if (save==-1) // fichier inexistant
    {
        creebin(valeur,efp); // crée fichier
    }

    tblfichier(valeur,efp,index); //Entrée les valeurs du fichier dans le tableau
    for(ix=0; ix<5; ix = ix + 1) //Afficher élement libre ou sauvegardé
    {
        if(valeur[ix].contenu == 0)
        {
            printf("Emplacement %d : Disponible \n",ix+1);
            printf("\n");
        }
        else
        {
            printf("Emplacement %d : Les valeurs sauvegardés \n",ix+1);
            printf("La valeur de la résistance R : %.10f \n",valeur[ix].r);
            printf("La valeur de la capacité C : %.10f \n",valeur[ix].c);
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
            printf("Voulez-vous entrer des nouvelles valeurs (0) ou choisir des valeurs sauvegardées(1) ? Répondre par 0 ou 1 \n");
            scanf("%d",&choix);
        }
        while(choix!= 0 && choix!= 1);
    }

    if(*index==5 && choix==0) // Demande de remplacer si fichier plein
    {
        do
        {
            printf("Dans quel élément voulez-vous remplacer les valeurs ");
            scanf("%d",&d);
        }
        while(d!=1 && d!=2 && d!=3 && d!=4 && d!=5);
        *index = d-1;
    }

    if(choix==0 || *index==0) // Demande d'entrer les valeurs si possible
    {
        do
        {
            printf("Veuillez entrer des valeurs valides. Sinon veuillez réssayer \n");
            do
            {
                printf("Veuillez entrer une valeur de la résistance R : ");
                scanf("%lf",&valeur[*index].r);
            }
            while(valeur[*index].r <= 0); //Vérifie entrée

            do
            {
                printf("Veuillez entrer une valeur de la capacité C : ");
                scanf("%lf",&valeur[*index].c);
            }
            while(valeur[*index].c <= 0); //Vérifie entrée

            do
            {
                printf("Veuillez entrer une valeur de pc minimal : ");
                scanf("%lf",&valeur[*index].pcmin);
            }
            while(valeur[*index].pcmin <= 0 || valeur[*index].pcmin >= 1); //Vérifie entrée

            do
            {
                printf("Veuillez entrer une valeur de pc maximal : ");
                scanf("%lf",&valeur[*index].pcmax); //Vérifie entrée

            }
            while(valeur[*index].pcmax >= 1 || valeur[*index].pcmax <= valeur[*index].pcmin);

            do
            {
                printf("Veuillez entrer une valeur du temps de dissipation: ");
                scanf("%lf",&valeur[*index].td);
            }
            while(valeur[*index].td <= 0); //Vérifie entrée

        }
        while(limiteFct(valeur,index)==FAUX); // Vérifie validité des entrées.
        valeur[*index].contenu = VRAI ; // Element plein
    }

    if (choix==1) // Recevoir choix d'élement déjà sauvegardé
    {
        do
        {
            printf("Choisissez l'élément contenant les valeurs désirées ");
            scanf("%d",&d);
            d = d-1 ;
        }
        while(d>4 || d<0 ||valeur[d].contenu==0);
        *index = d ; // Assigne index de l'élement choisis
        save = 1 ;
    }

    return(save);
}
/*---------------------------------------------------------------------
 Fonction: verifieFch
 Paramètres :
 efp : Pointeur au fichier à ouvrir.
 choix : Pointeur à la variable choix déterminant le choix de l’utilisateur.
 Description: Cette fonction est responsable de vérifier si un fichier existe ou pas.
------------------------------------------------------------------------*/
void verifieFch(FILE *efp,int *save)
{
    efp = fopen(FICHIER_BIN, "rb");
    if(efp == NULL)
    {
        printf("Aucun fichier binaire %s existant \n", FICHIER_BIN);
        printf("Un nouveau fichier sera créé \n \n", FICHIER_BIN);
        *save = -1 ; // Affecte -1 pour créer fichier
    }
    fclose(efp);
}
/*---------------------------------------------------------------------
 Fonction: tblfichier
 Paramètres :
 valeur : Référence au tableau de structure contenant l’ensemble des valeurs.
 efp : Pointeur au fichier à ouvrir.
 index : Pointeur à la variable contenant l’index de l’élément du tableau utilisé pour tracer le graphique.
 Description: Cette fonction est responsable de lire le contenu du fichier
 et l’affecter au tableau de structure.
------------------------------------------------------------------------*/
void tblfichier(STRUCT valeur[], FILE *efp, int *index)
{
    // Déclaration et initialisation des variables
    int ix=0;
    int flag=VRAI;
    efp = fopen(FICHIER_BIN, "rb");
    fread(valeur,sizeof(STRUCT),5,efp);
    fclose(efp);
    *index = 5;
    while(flag==VRAI && ix<5) //Assigne index au premier élément vide du tableau
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
 Retourne VRAI si les valeurs sont valides et FAUX s’ils ne le sont pas.
 Paramètres :
 valeur : Référence au tableau de structure contenant l’ensemble des valeurs.
 index : Pointeur à la variable contenant l’index de l’élément du tableau utilisé pour tracer le graphique.
 Description: Vérifie la validité des valeurs entrées en calculant une limite inférieur de l’inducteur.
------------------------------------------------------------------------*/
int limiteFct(STRUCT valeur[], int *index)
{
    // Déclaration et initialisation des variables
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
 Paramètres :
 valeur : Référence au tableau de structure contenant l’ensemble des valeurs.
 index : Pointeur à la variable contenant l’index de l’élément du tableau utilisé pour tracer le graphique.
 x : Référence au tableau des points d’abscisses où les valeurs de pc vont être stockés.
 y : Référence au tableau des points d’ordonnés où les valeurs de L vont être stockés.
 Description: Cette fonction affecte les valeurs de pc au tableau x et les valeurs de L au tableau y.
------------------------------------------------------------------------*/
void creeTbl(STRUCT valeur[], int *index, double x[], double y[])
{
    // Déclaration des variables
    double pc;
    double inc;
    int ix;
    // Initialisation des variables
    pc = valeur[*index].pcmin;
    inc = (valeur[*index].pcmax - valeur[*index].pcmin)/N ; //valeur de l'incrémentation
    x[0] = pc; // 1er élément du tableau x
    y[0] = valeur[*index].l_min; // 1er élément du tableau y
    x[N-1]= valeur[*index].pcmax; // Dernier élément du tableau x
    y[N-1]= valeur[*index].l_max; // Dernier élément du talbeau y
    for(ix=1; ix < N-1; ix = ix + 1)
    {
        pc = pc + inc; // Incrémentation
        x[ix] = pc;
        y[ix] = trouveRacine(valeur,index,pc); //Calcul de l par bissection
    }
}

/*---------------------------------------------------------------------
 Fonction: trouveRacine
 Valeur de retour:
 Retourne la valeur de la racine lmoy.
 Paramètres :
 valeur : Référence au tableau de structure contenant l’ensemble des valeurs.
 index : Pointeur à la variable contenant l’index de l’élément du tableau utilisé pour tracer le graphique.
 pc : Variable contenant la valeur du pourcentage de la charge.
 Description: Utilise la méthode de bissection afin de trouver la racine de l’équation g(L)=exp(-Rtd/2L) – pc.
------------------------------------------------------------------------*/
double trouveRacine(STRUCT valeur[], int *index, double pc)
{
    // Déclaration des variables
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

        if (fabs(calculeFct(valeur,index,lmin,pc)*calculeFct(valeur,index,lmoy,pc))<= TOLERANCE) //Si racine trouvée
            flag = VRAI;

        else if (calculeFct(valeur,index,lmin,pc)*calculeFct(valeur,index,lmoy,pc)<0) //Si racine dans moitié inférieure
            lmax = lmoy;

        else if (calculeFct(valeur,index,lmin,pc)*calculeFct(valeur,index,lmoy,pc)>0) //Si racine dans moitié supérieure
            lmin = lmoy;
    }
    return(lmoy); // Racine retournée

}
/*---------------------------------------------------------------------
 Fonction: calculeFct
 Valeur de retour:
 Retourne la valeur de g(L)
 Paramètres :
 valeur : Référence au tableau de structure contenant l’ensemble des valeurs.
 index : Pointeur à la variable contenant l’index de l’élément du tableau utilisé pour tracer le graphique.
 l : Variable contenant la valeur de l’inductance.
 pc : Variable contenant la valeur du pourcentage de la charge.
 Description: Cette fonction réalise une chaîne de calcule pour trouver le résultat de l’équation g(L)=exp(-Rtd/2L) – pc.
------------------------------------------------------------------------*/
double calculeFct(STRUCT valeur[], int *index, double l, double pc)
{
    // Déclaration des variables
    double gl;
    // Initialisation des variables
    gl = (-valeur[*index].r*valeur[*index].td)/(2*l);
    gl = exp(gl);
    gl = gl - pc ;
    return(gl);
}

/*---------------------------------------------------------------------
 Fonction: affichevaleur
 Paramètres :
 valeur : Référence au tableau de structure contenant l’ensemble des valeurs.
 index : Pointeur à la variable contenant l’index de l’élément du tableau utilisé pour tracer le graphique.
 Description: Cette fonction affiche les valeurs données par l’utilisateur.
------------------------------------------------------------------------*/
void affichevaleur(STRUCT valeur[],int *index)
{
    //Affiche valeurs
    printf("Ceci est un graphique représentant l'inductance L en fonction de pc pour : \n");
    printf("R = %.10f",valeur[*index].r);
    printf("     C = %.10f",valeur[*index].c);
    printf("     pcmin = %.3f",valeur[*index].pcmin);
    printf("     pcmax = %.3f",valeur[*index].pcmax);
    printf("     td = %.10f \n",valeur[*index].td);
}

/*-----------------------------------------------------------------------
Fonction: creebin
Paramètres :
valeur : Référence au tableau de structure contenant l’ensemble des valeurs.
efp : Pointeur au fichier à ouvrir.
Description:  Cette fonction permet de créer un fichier binaire vide dans le cas où celui-ci n’existe pas.
------------------------------------------------------------------------*/
void creebin(STRUCT valeur[],FILE *efp)
{
    // Déclaration des variables
    int ix;
    // Initialisation des variables
    for(ix=0; ix < 5; ix = ix+1)
    {
        valeur[ix].contenu = 0 ; // Assigner élément du tableau comme vide
    }
    efp = fopen(FICHIER_BIN, "wb");
    fwrite(valeur,sizeof(STRUCT),5,efp);
    fclose(efp);
}

/*-------------------------------------------------
 Fonction: pplot()
 valeur : Référence au tableau de structure contenant l’ensemble des valeurs.
 index : Pointeur à la variable contenant l’index de l’élément du tableau utilisé pour tracer le graphique.
 x : Référence au tableau des points d’abscisses où les valeurs de pc vont être stockés.
 y : Référence au tableau des points d’ordonnés où les valeurs de L vont être stockés.
Description: Initialise le graphique.  La taille des axes est définie par:
             pcmin, pcmax   Axe horixontal
             minFx, maxFx   Axe vertical
             Il est supposé queles valeurs de la rangée x sont triées en ordre croissant.
             La courbe de f(x) est tracé avec le contenu de chaque rangée,
             c.-à-d. x  et  y qui sont tous deux des référeces à des tableaux 1D.
-------------------------------------------------*/
void pplot(STRUCT valeur[], int *index, double x[], double y[])
{
    // Déclaration des variables
    double minFx, maxFx;  // Valuers minimum et maximum de f(x)
    // Initialisation du graphique
    plsdev("wingcc");  // Choisi l'appareil wingcc - le comilateur de CodeBlocks
    plinit();
    // Configurer les axes et les étiquettes du graphique
    plwidth(3);   // sélectionner la largeur de plume
    minFx = valeur[*index].l_min;
    maxFx = valeur[*index].l_max;
    plenv(valeur[*index].pcmin,valeur[*index].pcmax,
          minFx, maxFx, 0, 0);
    plcol0(BLACK);    // Couleur des éttiquettes
    pllab("pc(secondes)", "L(Henry)", "Graphique de l'inductance L versus pc");
    // Trace la fonction sur le graphique
    plcol0(BLUE);    // Couleur de la trace
    plline(N, x, y);
    plend();
}

/*-----------------------------------------------------------------------
Fonction: demandesauvegarde
Paramètres :
valeur : Référence au tableau de structure contenant l’ensemble des valeurs.
efp : Pointeur au fichier à ouvrir.
Description: La fonction demande à l’utilisateur s’il veut sauvegarder les données ajoutées.
------------------------------------------------------------------------*/
void demandesauvegarde(STRUCT valeur[],FILE *efp)
{
    // Déclaration des variables
    int flag;
    // Demande de sauvegarde
    do
    {
        printf("Voulez-vous enregistrer les donnés entrées ? Répondre 1 pour oui et 0 pour non \n");
        scanf("%d",&flag);
    }while(flag!=0 && flag!=1); //Verifie entrée
    if (flag=1)
    {
        sauvefichier(valeur,efp); //Sauvegarde fichier
    }


}
/*-----------------------------------------------------------------------
Fonction: sauvefichier
Paramètres :
valeur : Référence au tableau de structure contenant l’ensemble des valeurs.
efp : Pointeur au fichier à ouvrir.
Description: Cette fonction permet d’écrire le contenu  du tableau dans le fichier.
------------------------------------------------------------------------*/
void sauvefichier(STRUCT valeur[],FILE *efp)
{
    efp = fopen(FICHIER_BIN, "wb");
    rewind(efp);
    fwrite(valeur,sizeof(STRUCT),5,efp);
    fclose(efp);
}
