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
import src.Util.GameSet;

/**
 * Trida hledajici nejlepsi mozny tah.
 */
public class Help
{
	public Help(){}

	/**
	 * Najde nejlepsi mozny dalsi tah a vrati textovou reprezentaci daneho tahu.
	 * Postupne prochazi vsechny moznosti od nejvyhodnejsich po ty mene vyhodne.
	 * Pokud dalsi tah neni mozny, informuje o neuspesnem konci hry.
	 * @param set Aktualne rozehrana hra.
	 * @return Řetezec obsahujici informaci o dalsim moznem tahu.
	 */
	public String getHelp(GameSet set)
	{
		// vrchni karta odkladaciho balicku
		Card dropCard = set.dropPack.top();

		// kontrola, zdali se nejaka karta z pracovnich balicku nebo z odkladaciho balicku neda presunout do ciloveho balicku
		for(int x = 0; x < 7; x++)
		{
			// vrchni karta pracovniho balicku
			Card workCard = set.workPack[x].top();

			for(int y = 0; y < 4; y++)
			{
				if(set.targPack[y].isPutPossible(dropCard))
					return "Move card " + dropCard + " to the target pack.";

				// pokud lze kartu vlozit, vrať informaci o danem kroku
				if(set.targPack[y].isPutPossible(workCard))
					return "Move card " + workCard + " to the target pack.";
			}
		}

		// kontrola, zdali se nejaka karta z pracovnich balicku neda presunout do jineho pracovniho balicku
		for(int x = 0; x < 7; x++)
		{
			// spodni karta pracovniho balicku
			Card workCard = set.workPack[x].bottom();

			for(int y = 0; y < 7; y++)
			{
				if( x != y && set.workPack[y].isPutPossible(workCard) && !set.workPack[x].isLastKing())
				{
					if(set.workPack[y].top() == null)
						return "Move cards from " + workCard + " onto the empty working pack.";
					else
						return "Move cards from " + workCard + " onto the " + set.workPack[y].top() + ".";
				}
			}
		}

		// kontrola, zdali se neda vrchni karta z odkladaciho balicku dat na nejaky pracovni balicek
		for(int x = 0; x < 7; x++)
		{
			if(set.workPack[x].isPutPossible(dropCard))
			{
				if(set.workPack[x].top() == null)
					return "Move card " + dropCard + " onto the empty working pack.";
				else
					return "Move card " + dropCard + " onto the " + set.workPack[x].top() + ".";
			}
		}

		// pokud neni hraci nebo odkladaci balicek prazdny
		if(!set.gamePack.isEmpty() || !set.dropPack.isEmpty())
			return "See another card.";

		// pokud zadna z predeslych moznosti neuspela
		return "Game over! There is nothing you could do...";
	}
}