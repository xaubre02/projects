/*     Semestralni projekt
---------------------------------
Aplikace: Pasians Klondike
Predmet: IJA
Rok: 2016/2017
Autori: Aubrecht Tomas (xaubre02)
        Bandik Matej   (xbandi01)
-------------------------------*/

package src.Model;

import src.Model.Card;
import java.util.ArrayList;
import java.io.Serializable;

/**
 * Trida reprezentujici balicek karet.
 * Hodnota topCard reprezentuje vrchni kartu balicku. Pokud je balicek prazdny, je topCard roven null.
 */
public class GamePack implements java.io.Serializable
{
	protected ArrayList<Card> pack;
	protected Card topCard;

	/**
	 * Vytvoreni hraciho balicku karet.
	 */
	public GamePack()
	{
		pack = new ArrayList<Card>();
		topCard = null;
	}

	/**
	 * Vytvoreni hraciho balicku karet na zaklade pole karet.
	 * @param array Seznam karet, ze kterych se vytvori novy herni balicek.
	 */
	public GamePack(ArrayList<Card> array)
	{
		pack = array;
		// nastavi vrchni kartu
		if(!pack.isEmpty())
			topCard = pack.get(pack.size() - 1);
	}

	/**
	 * @return Aktualni seznam karet.
	 */
	public ArrayList<Card> getArrayList()
	{
		return pack;
	}

	/**
	 * @return Aktualni pocet karet v balicku.
	 */
	public int size()
	{
		return pack.size();
	}

	/**
	 * Vrati kartu z vrcholu zasobniku (karta zustava na zasobniku).
	 * Pokud je balicek prazdny, vraci null.
	 * @return Karta z vrcholu balicku.
	 */
	public Card top()
	{
		return topCard;
	}

	/**
	 * Vlozi kartu na vrchol balicku a nastavi vrchnu kartu.
	 * @param card Vkladana karta.
	 * @return Úspesnost akce.
	 */
	public boolean put(Card card)
	{
		if(card != null){
			topCard = card;
			return pack.add(card);
		} else {
			return false;
		}
	}

	/**
	 * Odebere kartu z vrcholu balicku. Pokud je balicek prazdny, vraci null.
	 * @return Karta z vrcholu balicku.
	 */
	public Card pop()
	{
		if(pack.isEmpty())
			return null;

		if(pack.size() == 1)
			topCard = null;

		else
			topCard = pack.get(pack.size() - 2);

		return pack.remove(pack.size() - 1);
	}

	public void initPut(Card card)
	{
		put(card);
	}

	private void setTopCard(Card c)
	{
		topCard = c;
	}

	public GamePack pop(int index)
	{
		int size = pack.size();
		GamePack taken = new WorkingPack();
		if(index >= size)
			return null;

		for(int i = index; i < size; i++)
			taken.initPut(pack.remove(index));

		if(pack.isEmpty()){
			topCard = null;
		}

		else {
			pack.get(pack.size() - 1).turnFaceUp();
			topCard = pack.get(pack.size() - 1);
		}
		taken.setTopCard(taken.get(taken.size() - 1));
		return taken;
	}

	/**
	 * Vrati kartu na uvedenem indexu.
	 * Spodni karta je na indexu 0, vrchol je na indexu size()-1.
	 * Pokud je balicek prazdny, nebo index mimo rozsah, vraci null.
	 * @param index Pozice karty v balicku.
	 * @return Karta z vrcholu balicku.
	 */
	public Card get(int index)
	{
		if(pack.isEmpty() || index < 0 || index >= pack.size())
			return null;

		return pack.get(index);
	}

	/**
	 * Test, zda je balicek karet prazdny.
	 * @return Vraci true, pokud je balicek prazdny.
	 */
	public boolean isEmpty()
	{
		return pack.isEmpty();
	}
}