/*     Semestralni projekt
---------------------------------
Aplikace: Pasians Klondike
Predmet: IJA
Rok: 2016/2017
Autori: Aubrecht Tomas (xaubre02)
        Bandik Matej   (xbandi01)
-------------------------------*/

package src.Util;

import src.Model.Card;
import src.Model.GamePack;
import src.Model.TargetPack;
import src.Model.WorkingPack;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collections;

/**
 * Trida reprezentujici stav hry.
 * Trida obsahuje 1 herni balicek, 4 cilove balicky a 7 balicku pracovnich.
 */
public class GameSet implements java.io.Serializable
{
	public int packIndex;
	public GamePack gamePack;
	public GamePack dropPack;
	public WorkingPack movePack;
	public TargetPack[] targPack;
	public WorkingPack[] workPack;

	/**
	 * Vytvoreni nove hry.
	 */
	public GameSet()
	{
		initPacks();
	}

	/**
	 * Inicializuje vsechny balicky ve hre.
	 */
	public void initPacks()
	{
		gamePack = new GamePack();
		dropPack = new GamePack();
		movePack = new WorkingPack();
		targPack = new TargetPack[4];
		workPack = new WorkingPack[7];

		for(int i = 0; i < 4; i++) { targPack[i] = new TargetPack(); }
		for(int i = 0; i < 7; i++) { workPack[i] = new WorkingPack(); }
	}

	/**
	 * Naplni balicky kartama.
	 */
	private void fillPacks()
	{
		// vytvoreni seznamu vsech karet
		ArrayList<Card> tmp = new ArrayList<Card>(52);

		for (int i = 1; i <= 13; i++) { tmp.add(new Card(Card.Color.CLUBS,i)); }
		for (int i = 1; i <= 13; i++) { tmp.add(new Card(Card.Color.DIAMONDS,i)); }
		for (int i = 1; i <= 13; i++) { tmp.add(new Card(Card.Color.HEARTS,i)); }
		for (int i = 1; i <= 13; i++) { tmp.add(new Card(Card.Color.SPADES,i)); }

		// zamichani karet
		Collections.shuffle(tmp);

		// naplneni balicku
		for (int i = 0; i < 24; i++) { gamePack.put(tmp.remove(tmp.size() - 1)); }
		for (int x = 0, n = 0; x < 7; x++, n++)
		{
			for (int y = 0; y <= n; y++)
				workPack[n].initPut(tmp.remove(tmp.size() - 1));

			workPack[n].init();
		}
	}

	/**
	 * Zamicha karty a vytvori novou hru.
	 */
	public void createNewGame()
	{
		initPacks();
		fillPacks();
	}

	public void createExample()
	{
		initPacks();
		for(int i = 1; i <= 13; i++) { targPack[0].put(new Card(Card.Color.SPADES, i)); }
		for(int i = 1; i <= 13; i++) { targPack[1].put(new Card(Card.Color.HEARTS, i)); }
		for(int i = 1; i <= 6; i++) { targPack[2].put(new Card(Card.Color.CLUBS, i)); }
		for(int i = 1; i <= 5; i++) { targPack[3].put(new Card(Card.Color.DIAMONDS, i)); }

		gamePack.put(new Card(Card.Color.CLUBS, 12));
		gamePack.put(new Card(Card.Color.CLUBS, 8));
		gamePack.put(new Card(Card.Color.DIAMONDS, 11));
		gamePack.put(new Card(Card.Color.DIAMONDS, 10));
		gamePack.put(new Card(Card.Color.DIAMONDS, 7));

		workPack[0].initPut(new Card(Card.Color.CLUBS, 13));
		workPack[0].initPut(new Card(Card.Color.CLUBS, 9));
		workPack[0].init();
		
		workPack[1].initPut(new Card(Card.Color.DIAMONDS, 6));
		workPack[1].init();

		workPack[2].initPut(new Card(Card.Color.DIAMONDS, 13));
		workPack[2].initPut(new Card(Card.Color.DIAMONDS, 12));
		workPack[2].init();

		workPack[3].initPut(new Card(Card.Color.DIAMONDS, 9));
		workPack[3].initPut(new Card(Card.Color.DIAMONDS, 8));
		workPack[3].initPut(new Card(Card.Color.CLUBS, 10));
		workPack[3].init();

		workPack[4].initPut(new Card(Card.Color.CLUBS, 7));
		workPack[4].initPut(new Card(Card.Color.CLUBS, 11));
		workPack[4].init();
	}

	/**
	 * Odebere z pracovniho balicku urceneho podle packIndex sekvenci karet pocinajici kartou na indexu cardIndex.
	 * Tyto karty umisti do movePack.
	 * @param packIndex Index pracovniho balicku, ze ktereho se bude odebirat
	 * @param cardIndex Index karty, od ktere se bude odebirat
	 * @return Úspesnost odebrani karet
	 */
	private boolean takeCards(int packIndex, int cardIndex)
	{
		if(!movePack.isEmpty()) return false;

		WorkingPack tmp = workPack[packIndex].pop(cardIndex);
		return movePack.putMoving(tmp);
	}

	/**
	 * Umisti karty z presouvaneho balicku do pracovniho balicku urceneho podle packIndex.
	 * @param packIndex Index pracovniho balicku, do ktereho se bude vkladat
	 * @return Úspesnost vkladabi karet
	 */
	private boolean dropCards(int packIndex)
	{
		if(movePack.isEmpty()) return false;
		// pokud lze vlozit karty do daneho balicku, tak vyprazdni movePack a otoci vrchni kartu v balicku, odkud byly karty odebrany
		if(workPack[packIndex].put(movePack))
		{
			movePack.pop(0);

			if(!workPack[this.packIndex].isEmpty())
				workPack[this.packIndex].top().turnFaceUp();
			return true;
		}
		// jinak vrati karty do puvodniho balicku
		else
		{
			workPack[this.packIndex].put(movePack.pop(0));
			return false;
		}
	}

	/**
	 * Presune sekvenci karet pocinajici kartou na indexu cardIndex ze zdrojoveho pracovniho balicku urceneho podle indexu source
	 * do pracovniho balicku urceneho podle indexu dest.
	 * @param source Index zdrojoveho pracovniho balicku
	 * @param dest Index ciloveho pracovniho balicku
	 * @param cardIndex Index karty, od ktere se bude odebirat
	 * @return Úspesnost presunu karet
	 */
	public boolean moveCards(int source, int dest, int cardIndex)
	{
		this.packIndex = source;

		if (takeCards(source, cardIndex))
		{
			if(dropCards(dest))
				return true;
		}

		return false;
	}
}