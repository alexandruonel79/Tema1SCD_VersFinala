Onel Alexandru Gabriel 342C5

RULARE:
      Totul este deja generat, nu e nevoie de rpcgen.
      Make build -> 2 fisere executabile: client si server
      Make clean -> sterge fisierele executabile client, server

Am inceput tema prin o buna intelegere a Oauth. Din enunt am inteles partial (schema logica a fost buna),
ulterior mi am clarificat intreband la laborator si postand pe forum.

Primul pas a fost sa realizez interfata, mi-am gandit argumentele functiilor si raspunsurile
lor. Le-am declarat in ordinea logica, pentru a fi mai usor de inteles. Am descris scurt in .x parametrii.

Dupa definirea interfetei, am trecut la citirea fisierelor. Pentru toate functiile auxiliare folosite
in fisierele Oauth_client.c si Oauth_server.c am folosit 2 fisiere denumite helper. Pentru client se
numeste helper_client.c, iar pentru server helper_Oauth.c. Datele citite au fost incapsulate in variabile globale,
definite in .h. Am fost atent sa le izolez, sa nu aibe acces USER la datele globale ale SERVER. De asemenea,
CLIENT nu are acces la nimic de la server.

Am implementat in server procedurile: 
              1) request authorization: Primul pas a fost sa verific daca utilizatorul se afla in baza 
de date. Daca se afla, ii generam tokenul de autorizare. In cazul in care nu era in baza de date, returnam
o eroare: USER_NOT_FOUND
              2) approve request token: Procedura asta este implementata de 'USER', acesta simuleaza citirea
permisiunilor citind dintr-un fisier. In cazul in care nu are nicio permisiune, intoarce rezultatul nesemnat,
cu acelasi token de autentificare, nemodificat si cu eroarea "REQUEST_DENIED". Altfel, creeaza un SignedData in
care salveaza tokenul de autentificare si permisiunile, apoi le semneaza (cripteaza adaugand + 1), si seteaza
campul boolean is_signed la true.
              3) request access token: Procedura primeste user id si datele semnate. Daca nu sunt semnate de USER,
intoarce "REQUEST_DENIED". Altfel, decripteaza datele semnate si verifica daca tokenul de autentificare este valid.
De aici genereaza un token de acces, iar daca request ul a fost cu auto refresh, genereaza si un token de refresh. Pune
valabilitatea si trimite catre CLIENT.
              4) validate delegated action: Procedura se ocupa cu validarea comenzilor in functie de permisiunile 
tokenului de acces. In primul pas verifica daca tokenul de acces nu este gol. Daca este gol afiseaza o eroare: 
"PERMISSION_DENIED". Verifica existenta tokenului, si valabilitatea, afisand mesaje conforme. Ulterior verifica
existenta resursei, iar apoi a permisiunilor stocate in tokens_global pentru acel user.
              5) refresh access token: Procedura este suplimentara, am definit o pentru a putea actualiza tokenul de 
acces, furnizand tokenul de refresh. Motivatia mea este ca este mai curat codul cu o noua procedura decat sa integrez in
request authorization. Functia verifica existenta tokenului, si ulterior genereaza un nou token de acces si de refresh, avand
o noua valabilitate.

Logica din client:
                  In fisierul de Oauth_client am preluat comenzile din fisier si le parcurg cu un for. Am folosit
strcmp pentru comparatie: prima ramura a if ului contine logica pentru REQUEST, iar a doua pentru operatii de READ, DELETE, etc..
                  1) REQUEST: Primul pas este cererea de token de autorizare. Ulterior dau tokenul de autorizare procedurii de approve.
Daca rezultatul se intoarce semnat (boolean este semnat | nu am o eroare). Daca este semnat, continui cu cererea de token de acces. Acum,
in functie daca utilizatorul se afla deja in structura mea de date sau nu, il salvez sau actualizez tokenii.
                  2) READ, MODIFY, DELETE, etc: Primul pas verific daca am deja utilizatorul, daca nu exista deja nu am un token de acces
pentru acesta si trimit o cerere cu un token empty si afisez eroarea. Daca ma aflu deja, scad valabilitatea tokenului si verific daca
este cu refresh automat, pentru a face o cerere de refresh cand expira valabilitatea. Apelez functia validate delegated action si afisez
raspunsul.

Observatii:
In .x am fost atent sa nu trimit si user_id si access token.
In server am evitat ca structurile sa contina user_id si access token, desi era permis.
Am implementat asa sa fiu sigur, ulterior s-a raspuns si clarificat pe forum.

Un video care m-a ajutat
sa inteleg Oauth: https://www.youtube.com/watch?v=ZV5yTm4pT8g&t=98s&ab_channel=ByteByteGo
