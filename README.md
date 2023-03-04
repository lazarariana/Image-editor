//Copyright Ariana-Maria Lazar-Andrei 312CAb 2022-2023

Define-urile PIXELS si MIN reprezinta capetele intervalului de valori ale
pixelilor, inchis la 0 si deschis la 256.

Fisierul struct.h

Pentru inceput, am creat un struct pentru fiecare tip de fisier: img_ascii
pentru fisiere ascii, respectiv img_bin pentru fisiere binare. Structura
cuprinde atributele necesare pentru a prelucra un fisier, anume: magic word-ul,
ce indica tipul ascii(P2 sau P3) sau binar(P5 sau P6), grayscale (P2, P5) sau rgb
(P3, P6). De asemenea, width si height stocheaza dimensiunile imaginii,
necesare alocarii dinamice a matricei de pixeli. Campul valmax indica valoarea
maxima pe care o poate avea un pixel in matrice. Campul data reprezinta o
matrice alocata dinamic in care vom stoca valorile pixelilor ce construiesc
imaginea. Trebuie sa tinem cont de tipul de fisier, de aceea am creat 2 tipuri
de date, pixel_type_ascii si pixel_type_bin. Pentru pixel_type_ascii, atat
valorile pixelilor grayscale, cat si ale celor rgb, sunt de tip int, iar pentru
pixel_type_bin, de tip unsigned char. Pentru pixelii rgb am creat tipul de date
color_ascii, respectiv color_bin, pentru a stoca impreuna pixelii red, green si
blue. Union-ul img_type reuneste cele 2 tipuri de imagini, pentru a usura
creearea functiilor necesare realizarii comenzilor, avand in vedere ca LOAD nu
specifica tipul de imagine la incarcarea in memorie, iar ambele tipuri de poze
necesita aceeasi zona de memorie ce trebuie alocata.
Structura de date zone retine datele despre selectia curenta in urma comenzilor
SELECT x1 y1 x2 y2, respectiv SELECT ALL. este necesar sa stim originea
selectiei si lungimea, respectiv inaltimea sa. Campul origin_ord reprezinta
ordonata selectiei, de la care se stocheaza height linii, iar campul origin_abs
este abcisa selectiei, de la care se stocheaza width coloane.

Comanda LOAD

Verificam mai intai magic word-ul fisierului, ce indica tipul text/ascii
(P2, P3) sau binar (P5, P6), pentru a stabili ce camp al structului
trebuie accesat in salvarea magic word-ului. imga reprezinta fisierele ascii,
iar imgb fisierele binare. Dupa stabilirea tipului de fisier, putem apela
functia corespunzatoare de citire a fisierului, read_ascii sau read_bin. In
corpul lor se citesc dimensiunile matricei de pixeli si valoarea maxima a
acestora. Utilizam functia skip_comm pentru a ignora liniile ce au ca prim
caracter '#', ce indica faptul ca sunt comentarii, respectiv blankline-urile.
Cazul default vizeaza seturile de date de intrare invalide. Dupa ce am stocat
informatiile despre matrice, alocam dinamic memorie pentru aceasta. Daca
aceasta reuseste, citim din fisier valorile fiecarui element si le stocam in
matrice. In caz contrar, memoria trebuie dezalocata.

Comanda SELECT_ZONE

Tinand cont de citirea cuvantului SELECT ce poate indica atat lansarea in
executie a functiei select_all, cat si a functiei select_zone, citim ceea ce
urmeaza dupa SELECT ca un string, presupunand ca dorim sa apelam SELECT ALL.
Pentru a stabili daca am citit stringul ALL sau un numar, utilizam functia
verif, care verifica daca stringul citit este egal lexicografic cu stringul
ALL. In caz afirmativ, putem apela comanda select_all. Altfel convertim
stringul in numar daca functia digits are rezultat true. Utilizand functia
coordinates stabilim daca am citit 4 coordonate valide pentru a putea apela
select_zone. Actualizam valorile stocate in campurile tipului de data zone:
coordonatele originii, lungimea si inaltimea.

Comanda CROP

Este necesar sa alocam dinamic memorie pentru matricea de pixeli a selectiei ce
dorim sa o cropam. Cum cerinta vizeaza zona de memorie selectata curent, stim
dimensiunile zonei de memorie, cat si coordonatele originii selectiei. Parcurgem
matricea de pixeli stocata in campul data al structului doar pe zona de memorie
selectata, pornind de la coordonatele originii pe inaltime si lungime cu atatea
pozitii cate sunt indicate de campurile width si height. Trebuie tinut cont de
faptul ca zona de memorie alocata pentru matricea auxiliara este indexata de la
0, de aceea indicii liniilor sunt cu selected_zone.orgin_ord mai mici, iar
indicii coloanelor cu selected_zone.orgin_abs mai mici. Dupa ce am copiat toate
elementele necesare, eliberam zona de memorie alocata matricei data, actualizam
dimensiunile si adaugam selectia cropata in locul sau. Actualizam si selectia
completa, deoarece matricea initiala nu mai exista.

Comanda APPLY

O comanda valida pentru apply este reprezentata de stringul 'APPLY' separat
pritr-un spatiu de parametrul sau, ce poate fi stringul 'EDGE', 'SHARPEN',
'BLUR', 'GAUSSIAN_BLUR'. Alocam memorie pentru matricea auxiliara in care vom
stoca valorile pixelilor matricei modificate in functie de filtrul aplicat.
Pentru filtrele edge si sharpen am creat functiile edge_sharpen_ascii si
edge_sharpen_bin. Parametrul mat_filter[] indica ce filtru trebuie aplicat
selectiei din fisier, stocand intr-un vector valoriile din matricea filtrului.
Tinem cont ca trebuie aplicata functia clamp pentru a respecta intervalul
corespunzator de valori ale pixelilor (pozitive, mai mici decat 256). Pentru a
eficientiza procesul de calcul incarcat, am utilizat algoritmul lui Lee pentru
a accesa rapid vecinii fiecarui element. Pentru filtrele blur si gaussian_blur
am creat functiile blur_ascii si blur_bin. Parametrul mat_filter[] are acelasi
rol, iar parametrul fr reprezinta constata ce trebuie inmultita cu fiecare
pixel. In functie de filtru, ia valorea lui box_fr sau gauss_fr. Dupa
finalizarea construirii matricei, copiem valorile noi in zona selectata din
matricea data. La final dezalocam matricea auxiliara.

Comanda SAVE

Trebuie tratate cele 4 cazuri de salvare, ce depind atat de tipul de salvare,
cat si de tipul fisierului. Tinem cont de faptul ca atunci cand tipul
fisierului de salvare difera de cel al fisierului initial, magic wordul trebuie
modificat. Folosim fwrite pentru a scrie in fisiere binare si fprintf pentru
text. In primul rand trebuie salvat magic wordul fisierului, iar apoi
dimensiunile matricei de pixeli (mai intai width, apoi height) si ulterior
matricea stocata in campul data.

Comanda HISTOGRAM

Daca parametrul y lipseste, nu este putere a lui 2 sau nu apartine intervalului
[2,255], atunci comanda histogram este invalida. In cazul in care regulile de
mai sus sunt respectate, putem forma vectorul de aparitii al fisierului, doar
daca pixelii sunt de tip grayscale. Fiind dat numarul de bin-uri care trebuie
utilizat, stocam in variabila group numarul de valori care trebuie incluse in
acelasi bin. Stiind ca avem in  total 256 de valori distincte ale pixelilor si
y este o putere a lui 2 intotdeauna, putem fi siguri ca impartirea la group
este exacta si nu exista valori pierdute. Construim in functia build_freq
vectorul de aparitii si actualizam (daca este necesar) valoarea frecventei
maxime din vectorul de fecvente. Dupa ce am construit vectorul, pentru fiecare
bin calculam numarul corespunzator de stele conform formulei date, iar apoi
printam conform formatului dat: numarul de stelute, \t, |, \t, stelutele.

Comanda EQUALIZE

Am reutilizat functia build_freq creata initial pentru comanda HISTOGRAM,
deoarece avem nevoie in continuare de vectorul de aparitii. Cum nu este
specificat faptul ca trebuie sa modificam numarul de bin-uri ale histogramei,
variabila group este egala cu 1 (calculam pentru fiecare bin frecventa). Pentru
fiecare valoare a unui bin, calculam suma de la 0 pana la valoarea pixelului
corespunzatoare binului, conform formulei din enuntul problemei. Pentru a
egaliza imaginea, inlocuim fiecare pixel cu rezultatul rotunjit al formulei
sum_h / area * (PIXELS - 1). 

Comanda EXIT_ALL

Dezaloca memoria alocata matricei de pixeli a fisierului, eliberand mai intai
vectorul de pointeri spre liniile matricei, iar apoi intreaga zona alocata matricei.

Comanda ROTATE

Pentru a realiza comanda rotate, am utilizat functia rotate_90. Deoarece am
observat ca toate valorile admise pentru unghiul de rotire al matricei sunt
multiplii de 90, am prelucrat apelurile functiei rotate_90 pentru a nu incarca
programul. Tinem cont de faptul ca rotirea la 180 de grade are acelasi rezultat
ca rotirea la -180, iar rotirile la 360, -360 si 0 nu au niciun efect asupra
matricei, deci ne concentram doar asupra cazurilor 90, -90, 270, -270. Observam
faptul ca 90 si 270 sunt unghiuri complementare avand in vedere ca o rotatie
completa pe cerc are valoarea de 360 de grade. Asadar, a roti matricea la 270
de grade este echivalent cu rotirea la -90, iar rotirea la 90 de grade
echivalenta cu rotirea la -270 de grade. In variabila cnt stocam catul dintre
valoarea in modul a unghiului si 90, pentru a calcula de cate ori trebuie sa
rotim matricea la 90 de grade. In construirea functiei rotate_90 utilizam
faptul ca pentru a obtine rotirea unei matrice la 90 de grade inversam
coloanele transpusei sale. Comparand indicii elementelor din matricea initiala
cu indicii noilor pozitii ale lor din transpusa prelucrata, am obtinut formula
pe care am utilizat-o in alcatuirea matricei rotite in matricea auxiliara aux.
Dupa ce am pozitionat toate elementele in aux, copiem rezultatul in matricea
data. Tinem cont de faptul dimensiunile width si height isi interschimba
valorile cand alocam memorie pentru aux, iar atunci cand doar secventa
patratica trebuie rotita, restul pixelilor din  matrice raman neschimbati.
