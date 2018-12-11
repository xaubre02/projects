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
 * Trida reprezentujici pracovni balicek karet.
 * Pokud je tento balicek prazdny je mozne do nej vlozit pouze kartu krale libovolne barvy.
 * Obsahuje-li tento balicek jiz nejake karty, vkladana karta nesmi mit podobnou barvu jako ma vrchni karta a zaroveň musi mit hodnotu o 1 mensi nez horni karta.
 */
public class WorkingPack extends GamePack
{
	private Card bottomCard;

	public WorkingPack(){}
	/**
	 * Vytvoreni pracovniho balicku karet na zaklade pole karet.
	 * @param array Seznam karet, ze kterych ma byt vytvoren novy balicek
	 */
	public WorkingPack(ArrayList<Card> array)
	{
		pack = array;
		// nastavi vrchni kartu
		if(!pack.isEmpty())
			topCard = pack.get(pack.size() - 1);
			
		for(Card c : pack)
		{
			if(c.isTurnedFaceUp())
			{
				bottomCard = c;
				break;
			}
		}
	}

	/**
	 * Inicializuje pracovni balicek.
	 */
	public void init()
	{
		topCard = bottomCard = pack.get(pack.size() - 1);
		topCard.turnFaceUp();
	}

	/**
	 * Vlozi kartu do balicku nehlede na jeji hodnotu a barvu (pri inicializaci).
	 * @param card Vkladana karta
	 */
	public void initPut(Card card)
	{
		super.put(card);
	}

	/**
	 * Nastavi vrchni kartu balicku.
	 */
	private void setTopCard(Card c)
	{
		topCard = c;
	}

	/**
	 * Nastavi spodni kartu balicku.
	 */
	private void setBottomCard(Card c)
	{
		bottomCard = c;
	}

	/**
	 * Funkce, ktera navraci spodni kartu balicku.
	 * @return Spodni karta.
	 */
	public Card bottom()
	{
		return bottomCard;
	}

	/**
	 * Vlozi kartu na vrchol balicku. Karta nesmi byt podobne barvy a musi byt o jedna mensi.
	 * @param card Vkladana karta.
	 * @return Úspesnost akce.
	 */
	public boolean put(Card card)
	{
		if(pack.isEmpty())
		{
			if(card.value() == 13)
			{
				bottomCard = topCard = card;
				return pack.add(card);
			}

			return false;
		}

		if(topCard.similarColorTo(card.color()) || topCard.compareValue(card) != 1 )
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
			// pokud je dana karta kral
			if(card.value() == 13) return true;
			else                   return false;
		}

		if(topCard.similarColorTo(card.color()) || topCard.compareValue(card) != 1 )
			 return false;
		else return true;
	}

	/**
	 * Vlozi karty ze zasobniku pack na vrchol zasobniku.
	 * Karty vklada ve stejnem poradi, v jakem jsou uvedeny v zasobniku pack.
	 * @param pack Zasobnik vkladanych karet.
	 * @return Uspesnost akce.
	 */
	public boolean put(WorkingPack pack)
	{
		if(this.pack.isEmpty())
		{
			if(pack.bottom().value() == 13)
			{
				bottomCard = pack.bottom();
				topCard = pack.top();
				return this.pack.addAll(pack.getArrayList());
			}

			return false;
		}
		if(topCard.similarColorTo(pack.bottom().color()) || topCard.compareValue(pack.bottom()) != 1 )
			return false;

		topCard = pack.top();
//		System.out.println(topCard);
		return this.pack.addAll(pack.getArrayList());
	}

	/**
	 * Vlozi karty ze zasobniku pack na vrchol zasobniku nehlede na obsah toho zasobniku.
	 * Karty vklada ve stejnem poradi, v jakem jsou uvedeny v zasobniku pack.
	 * @param pack Zasobnik vkladanych karet.
	 * @return Uspesnost akce.
	 */
	public boolean putMoving(WorkingPack pack)
	{
		if(this.pack.isEmpty())
		{
			bottomCard = pack.bottom();
			topCard = pack.top();
			return this.pack.addAll(pack.getArrayList());
		}

		return false;
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
			bottomCard = topCard = null;

		else
		{
			if(topCard.equals(bottomCard))
				bottomCard = topCard = pack.get(pack.size() - 2);

			else
				topCard = pack.get(pack.size() - 2);
		}

		return pack.remove(pack.size() - 1);
	}

	/**
	 * Metoda odebere ze zasobniku sekvenci karet od zadane karty az po vrchol zasobniku.
	 * Pokud je hledana karta na vrcholu, bude v sekvenci pouze jedna karta.
	 * @param index Pozice dane karty.
	 * @return Zasobnik karet obsahujici odebranou sekvenci. Pokud hledana karta v zasobniku neni, vraci null.
	 */
	public WorkingPack pop(int index)
	{
		int size = pack.size();
		WorkingPack taken = new WorkingPack();
		if(index >= size)
			return null;

		for(int i = index; i < size; i++)
			taken.initPut(pack.remove(index));

		if(pack.isEmpty()){
			bottomCard = topCard = null;
		}

		else {

			pack.get(pack.size() - 1).turnFaceUp();
			topCard = pack.get(pack.size() - 1);
		}
		taken.setTopCard(taken.get(taken.size() - 1));
		taken.setBottomCard(taken.get(0));
		return taken;
	}

	/**
	 * Vraci true, pokud je spodni odkryta karta posledni kartou v pracovnim balicku a jedna se o krale.
	 * @return Vysledek operace.
	 */
	public boolean isLastKing()
	{
		if(pack.isEmpty()) return false;

		if(pack.get(0).value() == 13 && pack.get(0).isTurnedFaceUp()) return true;

		return false;
	}
}