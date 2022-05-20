Toto je semestrální práce pro předmět APO na desce MZ_APO. 
Tématem je hra Loďe pro dvě desky pomocí TCP protokolu. 
Využíváné periferie jsou LCD displej, otočné voliče, RGB led diody a řádek led diod. 
Ve složce dokumentace je přiložen manuál pro hráče a diagramy s popisem modulů a chodu programu.
Na práci pracovali Kryštof Gärtner a Filip Doležal.

Finální verzi lze stáhnout z gitu: https://gitlab.fel.cvut.cz/gartnkry/battleship-semestral.git
V souboru Makefile změňte IP adresu na IP používané desky.
V souboru tcp.c změňte konstantu SERVER_IP na IP adresu druhé desky (protihráče).
Pro spuštění použijte příkaz make v konzolové řádce a následně make run (případně make clean).

V případě, že by něco nebylo kompletně funkční, 
tak poslední stabilní a funkční verze testovaná přímo ve škole je v commitu b98b1343.