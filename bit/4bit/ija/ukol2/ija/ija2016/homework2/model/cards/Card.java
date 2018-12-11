package ija.ija2016.homework2.model.cards;

/**
 * Rozhraní reprezentující jednu kartu. Karta obsahuje informaci o své hodnotě (1 až 13) a barvě.
 * Tyto informace jsou nastaveny konstruktorem.
 * Hodnota 1 reprezentuje eso (ace), 11 až 13 postupně kluk (jack), královna (queen) a král (king).
 * Barvu definuje výčtový typ Color.
 */
public interface Card {
	/**
	 * @return Hodnota karty.
	 */
	public int value();

	/**
	 * @return Barva karty.
	 */
	public Card.Color color();

	/**
	 * Testuje, zda je karta otočená lícem nahoru.
	 * @return Výsledek testu: true = karta je otočená lícem nahoru.
	 */
	public boolean isTurnedFaceUp();

	/**
	 * Otočí kartu lícem nahoru. Pokud tak už je, nedělá nic.
	 * @return Informace, zda došlo k otočení karty (=true) nebo ne
	 */
	public boolean turnFaceUp();

	/**
	 * Testuje, zda má karta podobnou barvu jako karta zadaná.
	 * Podobnou barvou se myslí černá (piky, kříže) a červená (káry a srdce).
	 * @param c Karta pro porovnání.
	 * @return Informace o shodě barev karet.
	 */
	public boolean similarColorTo(Card c);
	public boolean similarColorTo(Card.Color c);

	/**
	 * Porovná hodnotu karty se zadanou kartou c. Pokud jsou stejné, vrací 0.
	 * Pokud je karta větší než zadaná c, vrací kladný rozdíl hodnot.
	 * @param c Karta, s kterou se porovnává.
	 * @return Vrací rozdíl hodnot karet. 
	 */
	public int compareValue(Card c);

	public enum Color {
		CLUBS ("C"),
		DIAMONDS ("D"),
		HEARTS ("H"),
		SPADES ("S");

		private final String name;       

	    private Color(String s) {
	        name = s;
	    }

	    public static boolean contains (Color color) {
	    	for (Color c : Color.values()){
	    		if (c.equals(color))
	    			return true;
	    	}

	    	return false;
	    }

	    public String toString() {
	       return name;
	    }
	}
}