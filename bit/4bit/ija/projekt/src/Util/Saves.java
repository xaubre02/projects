/*     Semestralni projekt
---------------------------------
Aplikace: Pasians Klondike
Predmet: IJA
Rok: 2016/2017
Autori: Aubrecht Tomas (xaubre02)
        Bandik Matej   (xbandi01)
-------------------------------*/

package src.Util;

import src.Util.GameSet;
import java.io.*;
import java.util.Arrays;
import java.util.ArrayList;

/**
 * Trida zprostredkovavajici ukladani a nacitani stavu hry.
 */
public class Saves
{
	/**
	 * Konstruktor objektu.
	 */
	public Saves(){}

	/**
	 * Funkce najde vsechny ulozene hry ve slozce saves a vrati seznam s jejich jmeny.
	 * @return Seznam vsech ulozenych her
	 */
	public ArrayList getSaves()
	{
		File f = new File("dest-client/saves/");
		ArrayList<String> saves = new ArrayList<String>(Arrays.asList(f.list()));
		saves.add(0, "example");

		return saves;
	}
	
	/**
	 * Pomoci serializace objektu ulozi aktualni stav hry do souboru s urcitym jmenem.
	 * @param set Aktualni stav hry
	 * @param fileName Nazev souboru, kam se bude ukladat
	 * @throws IOException Kontrola prace se soubory
	 */
	public void saveGame(GameSet set, String fileName) throws IOException
	{
		// vytvoreni proudů
		FileOutputStream save = new FileOutputStream("dest-client/saves/" + fileName);
		ObjectOutputStream out = new ObjectOutputStream(save);
		
		// ulozeni objektu
		out.writeObject(set);

		// uvolneni pameti
		out.close();
		save.close();
	}

	/**
	 * Nacte stav hry z ulozeneho souboru obsahujici serializovany objekt a vrati ho jako vysledek.
	 * @param fileName Nazev souboru, ze ktereho se bude nacitat hra
	 * @return Nacteny stav hry nebo null v pripade chyby
	 * @throws IOException Kontrola prace se soubory
	 * @throws ClassNotFoundException Kontrola existence objektu
	 */
	public GameSet loadGame(String fileName) throws IOException, ClassNotFoundException
	{
		// vytvoreni proudů
		FileInputStream save;
		if(fileName == "example")
			save = new FileInputStream("examples/example");		
		else
			save = new FileInputStream("dest-client/saves/" + fileName);
		
		ObjectInputStream out = new ObjectInputStream(save);

		// vytvoreni noveho objektu
		GameSet loaded = (GameSet)out.readObject();
		
		// uvolneni pameti
		out.close();
		save.close();

		// vraceni nacteneho objektu
		return loaded;
	}
}