# Communication with a REST API in C.

	Pentru a putea respecta functionalitatea cerută, mai întâi am împărțit toate
comenzile posibile în câte o funcție (în afară de exit, ce avea o
funcționalitate prea simplă pentru a putea fi pus într-o funcție). Începutul
programului deschide conexiunea și citește de la tastatură comanda primită de la
utilizator și verifică dacă respectivul input se află printre comenzile valide,
dacă nu, se afișează un mesaj corespunzător.

	Funcția registering implementează comanda register. Acesta primește datele 
primite de la utilizator, adică username-ul și parola cu care dorește să se 
înregistreze. Dacă există alt utilizator cu același nume, se va întoarce un 
mesaj de eroare, altfel, contul a fost creat cu succes.

	Funcția login implementează comanda login. Acesta primește credențialele de 
autentificare de la utilizator (username + parolă). Dacă nu au fost introduse 
credențialele corect, atunci se va afișa un mesaj de eroare corespunzător, 
altfel, autentificarea a fost făcută cu succes.

	Funcția enter_library implementează comanda cu același nume. Ea creează un 
request de tip GET cu un cookie primit de la autentificare și va întoarce un 
token de intrare în biibliotecă (care nu se vede, dar este stocat in tokens[0]).
Dacă utilizatorul nu este autentificat, atunci un mesaj de eroare va apărea pe 
consolă.

	Funcția get_books implementează comanda cu același nume. Ea creează un 
request de tip GET cu un cookie primit de la autentificare și un token de 
intrare în bibliotecă. Dacă utilizatorul le deține, atunci i se vor întoarce 
id-ul și titlul tuturor cărților din bibliotecă, atlfel, i se va afișa un mesaj 
de eroare corespunzător.

	Funcția get_book implementează comanda cu același nume. Ea creează tot un 
request de tip GET cu același cookie și token. În plus față de get_books, 
utilizatorul trebuie să introducă id-ul cărții dorite pentru a vizualiza 
detaliile despre ea. Dacă nu introduce un id valid sau nu deține cookie-ul sau 
token-ul valid, atunci se va afișa un mesaj de eroare corespunzător.

	Funcția add_book implementează comanda cu același nume. Utilizatorul este 
nevoit să introducă câteva date despre respectiva carte, iar dacă este 
autentificat și „se află în” bibliotecă, atunci cartea va fi adăugată cu succes.
În caz contrar, se va afișa un mesaj de eroare corespunzător.

	Funcția delete_book implementează comanda cu același nume. Ea creează un 
request de tip DELETE cu aceleași credențiale de care este nevoie. La fel ca la 
get_book, utilizatorul este nevoit să introducă un id valid pentru a elimina o 
carte din bibiliotecă. În cazul în care id-ul, cookie-ul sau token-ul nu sunt 
valide, atunci se afișează un mesaj de eroare corespunzător.

	Funcția logout implementează comanda logout. Ea creează un request de tip 
get în care se cere ieșirea din cont a utilizatorului. Dacă cookie-ul nu este 
valid, atunci se va afișa un mesaj de eroare, altfel, utilizatorul va ieși din 
cont.

	Dacă se ajunge pe penultimul branch de if, atunci comanda este exit și 
atunci utilizatorul iese din program.

	Înainte de a încheia acest README, doresc să menționez următoarele: pentru 
request-urile de tip POST (register, login, add_book) am implementat parsarea 
textului în format json folosind parson.c și parson.h; înainte de a trimite 
mesajul către server, am redeschis conexiunea, deoarece, din păcate, după 30 de 
secunde se închide conexiunea.