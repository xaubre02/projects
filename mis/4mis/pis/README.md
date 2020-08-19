# PIS
Projekt do předmětu PIS (Pokročilé informační systémy)

# Instalace
1. Stáhnout a nainstalovat Java JDK - https://adoptopenjdk.net (OpenJDK 11 (LTS)) (z vezí 13 mi nefungoval eclipse)
2. Stáhnout Eclipse IDE - https://www.eclipse.org/downloads/packages/ (Eclipse IDE for Enterprise Java Developers)
3. Stáhnout Payara Server - https://www.payara.fish/products/payara-server/ (Payara Server Download)
    1. Stáhnutý archív rozbalit
4. Stáhnout MySQL JDBC Connector - https://downloads.mysql.com/archives/c-j/
    1. Zvolit verzi 5.1.47 (novější verze nefunguje) a případně zvolit Operating System = Platform Independent
    2. Nakopírovat mysql-connector-java-5.1.47.jar do \payara5\glassfish\lib\
5. Stáhnout a nainstalovat XAMPP - https://www.apachefriends.org/index.html
    1. Spustit Apache a MySQL
    2. V phpmyadmin dát root uživateli heslo root
    3. Upravit xampp\phpMyAdmin\config.inc.php, kde nastavit $cfg['Servers'][$i]['auth_type'] = 'cookie'; (phpmyadmin bude vždy vyžadovat heslo)
    4. Vytvořit databázi bakery (prozatím)
6. Otevřít Eclipse IDE
    1. Help -> Eclipse Marketplace... -> Instalovat Payara Tools
    2. Window -> Preference -> Server -> Runtime Enviroment -> Add -> Payara (Vytvořit Payara Server)
    3. Window -> Preference -> Data Management -> Connectivity -> Driver Definitions -> Add (Definovat MySQL Connector)
        1. Name/Type = MySQL JDBC Driver 5.1
        2. JAR List -> Clear All, Add JAR/Zip = lokalizovat JAR MySQL Connectoru (payara5\glassfish\lib\mysql-connector-java-5.1.47.jar)
        3. Properties -> Database Name = bakery, User ID = root, Password = root
    4. Window -> Web Browser -> Default system web browser
    5. Ve spodním okně zvolit záložku server, kliknout pravým tlačítkem myši na Payara server a zvolit možnost Start
        1. Opět kliknout pravým tlačítkem myši na Payara server a zvolit možnost Payara -> View Admin Console (otevře se webová stránka)
        2. Resources -> JDBC -> JDBC Connection Pools -> New
            1. Pool Name = MySQLPool, Resource Type = javax.sql.ConnectionPoolDataSource, Database Driver Vendor = MySQL (zmáčknout tlačítko Next)
            2. Zde definovat políčka: User = root, Password = root, DatabaseName = bakery, URL = jdbc:mysql://localhost:3306/bakery, Url = jdbc:mysql://localhost:3306/bakery (zmáčknout tlačítko Finish)
            3. Otestovat nově vytvořený MySQLPool kliknutím na MySQLPool a nahoře zmáčknout tlačítko Ping (Ping Succeeded)
        3. Resources -> JDBC -> JDBC Resources -> New
            1. JNDI Name = jdbc/mysql, Pool Name = MySQLPool (Zmáčknout tlačítko OK)
      6. Případné stažení ukázkového projektu
            1. Zvolit Git Perspective a zde naklonovat repozitář z URL https://github.com/DIFS-Teaching/jsf-basic a poté stačí naklonovaný repozitář importovat do Eclipse IDE

# Inicializace prázdné databáze
1. Pravým kliknout v Eclipse na projekt.
2. JPA Tools -> Generate Tables from Entities. (Pozor vymaže celou databázi!)

# Vytvoření ukázkové databáze
1. Vytvořit databázi s názvem "bakery" a nastavit kódování(porovnávání) na utf8mb4_czech_ci
2. Použít skript "bakery.sql", který se nachází v tomto repozitář - skript vytvoří tabulky, nastaví integritní omezení a naplní vytvořené tabulky ukázkovými daty

# Build
mvn clean package && docker build -t com.airhacks/pis.bakery .

# RUN
docker rm -f pis.bakery || true && docker run -d -p 8080:8080 -p 4848:4848 --name pis.bakery com.airhacks/pis.bakery

# Instalace a spuštění projektu v Angularu
1) Přejít do adresáře "PIS/angular"
2) Instalovat v angular-cli pomocí příkazu "npm install" (což naistaluje potřebné závislosti a balíky, např. ve složce node-modules)
3) Spustit v angular-cli pomocí příkazu "ng serve"