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
import src.Model.GamePack;
import java.util.ArrayList;

/**
 * Trida reprezentujici cilovy balicek karet.
 * Pokud je tento balicek prazdny je mozne do nej vlozit pouze kartu esa libovolne barvy.
 * Obsahuje-li tento balicek jiz nejake karty, vkladana karta musi mit stejnou barvu vrchni karta a hodnotu o 1 vyssi nez ma vrchni karta.
 */
public class TargetPack extends GamePack
{
	public TargetPack(){}
	/**
	 * Vytvoreni ciloveho balicku karet na zaklade pole karet.
	 * @param array Seznam karet, ze kterych se vytvori novy pracovni balicek
	 */
	public TargetPack(ArrayList<Card> array)
	{
		pack = array;
		// nastavi vrchni kartu
		if(!pack.isEmpty())
			topCard = pack.get(pack.size() - 1);
	}

	/**
	 * Vlozi kartu na vrchol balicku. Karta musi byt stejne barvy a musi byt o jedna vetsi.
	 * @param card Vkladana karta.
	 * @return Úspesnost akce.
	 */
	public boolean put(Card card)
	{
		if(pack.isEmpty())
		{
			if(card.value() == 1)
			{
				topCard = card;
				return pack.add(card);
			}

			return false;
		}

		if(!topCard.color().equals(card.color()) || topCard.compareValue(card) != -1 )
			return false;

		topCard = card;
		return pack.add(card);
	}

	/**
	 * Zkontroluje, zdali se dana karta da vlozit na vrchol ciloveho balicku. Karta musi byt stejne barvy a musi byt o jedna vetsi.
	 * @param card Karta, kterou bychom chteli vlozit.
	 * @return Úspesnost akce.
	 */
	public boolean isPutPossible(Card card)
	{
		if(card == null) return false;

		// pokud je balicek prazdny
		if(pack.isEmpty())
		{
			// pokud je karta eso
			if(card.value() == 1) return true;
			else                  return false;
		}

		if(!topCard.color().equals(card.color()) || topCard.compareValue(card) != -1 )
			 return false;
		else return true;
	}
}