/*     Semestralni projekt
---------------------------------
Aplikace: Pasians Klondike
Predmet: IJA
Rok: 2016/2017
Autori: Aubrecht Tomas (xaubre02)
        Bandik Matej   (xbandi01)
-------------------------------*/

package src.Model;

import java.util.Objects;
import java.io.Serializable;

/**
 * Trida reprezentujici jednu kartu. Karta obsahuje informaci o sve hodnote (1 az 13) a barve.
 * Tyto informace jsou nastaveny konstruktorem.
 * Hodnota 1 reprezentuje eso (ace), 11 az 13 postupne kluk (jack), kralovna (queen) a kral (king).
 * Barvu definuje vyctovy typ Color.
 */
public class Card implements java.io.Serializable
{
	private int value;
	private boolean faceUp;
	private Card.Color color;

	/**
	 * Kontroktor pro vytvoreni karty.
	 * @param color Barva karty
	 * @param value Hodnota karty
	 */
	public Card(Card.Color color, int value)
	{
		this.value = value;
		this.color = color;
		this.faceUp = false;
	}

	/**
	 * @return Hodnota karty.
	 */
	public int value()
	{
		return value;
	}

	/**
	 * Testuje, zda je karta otocena licem nahoru.
	 * @return Vysledek testu: true = karta je otocena licem nahoru.
	 */
	public boolean isTurnedFaceUp()
	{
		return faceUp;
	}

	/**
	 * @return Barva karty.
	 */
	public Card.Color color()
	{
		return color;
	}

	/**
	 * Otoci kartu licem nahoru. Pokud tak uz je, nedela nic.
	 * @return Informace, zda doslo k otoceni karty (=true) nebo ne
	 */
	public boolean turnFaceUp()
	{
		if(faceUp)
			return false;

		else
		{
			faceUp = true;
			return true;
		}
	}

	/**
	 * Testuje, zda ma karta podobnou barvu jako zadana barva.
	 * Podobnou barvou se mysli cerna (piky, krize) a cervena (kary a srdce).
	 * @param c Barva pro porovnani.
	 * @return Informace o shode barev karet.
	 */
	public boolean similarColorTo(Card.Color c)
	{
		if( color.equals(Card.Color.CLUBS) || color.equals(Card.Color.SPADES) )
			return ( c.equals(Card.Color.CLUBS) || c.equals(Card.Color.SPADES) );

		else
			return ( c.equals(Card.Color.HEARTS) || c.equals(Card.Color.DIAMONDS) );
	}

	/**
	 * Porovna hodnotu karty se zadanou kartou c. Pokud jsou stejne, vraci 0.
	 * Pokud je karta vetsi nez zadana c, vraci kladny rozdil hodnot.
	 * @param c Karta, s kterou se porovnava.
	 * @return Vraci rozdil hodnot karet. 
	 */
	public int compareValue(Card c)
	{
		return value - c.value();
	}

	/**
	 * Vycet vsech moznych barev karty.
	 */
	public enum Color
	{
		CLUBS ("C"),
		DIAMONDS ("D"),
		HEARTS ("H"),
		SPADES ("S");

		private final String name;       

		private Color(String s)
		{
			name = s;
		}

		public String toString()
		{
			return name;
		}
	}

	@Override
	public String toString()
	{
		String name = Integer.toString(value);
		if(value > 10 || value == 1)
		{
			switch(value)
			{
				case 01:
					name = "A";
					break;

				case 11:
					name = "J";
					break;

				case 12:
					name = "Q";
					break;

				case 13:
					name = "K";
					break;

				default:
					break;
			}
		}
		return name + "(" + color.toString() + ")";
	}

	@Override
	public boolean equals(Object o)
	{
		if (o == this)
			return true;
		if (!(o instanceof Card))
			return false;

		Card card = (Card) o;
		return color == card.color() && value == card.value();
	}

	@Override
	public int hashCode()
	{
		return Objects.hash(color, value);
	}
}