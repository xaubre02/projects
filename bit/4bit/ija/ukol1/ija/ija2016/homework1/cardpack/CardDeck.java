package ija.ija2016.homework1.cardpack;

import ija.ija2016.homework1.cardpack.Card;
import java.util.ArrayList;

public class CardDeck
{
	private ArrayList<Card> deck;

	public CardDeck(int size)
	{
		this.deck = new ArrayList<Card>(size);
	}

	public static CardDeck createStandardDeck()
	{
		CardDeck stdDeck = new CardDeck(52);
		
		for (int i = 1; i <= 13; i++) { stdDeck.put(new Card(Card.Color.CLUBS,i)); }
        for (int i = 1; i <= 13; i++) { stdDeck.put(new Card(Card.Color.DIAMONDS,i)); }
        for (int i = 1; i <= 13; i++) { stdDeck.put(new Card(Card.Color.HEARTS,i)); }
        for (int i = 1; i <= 13; i++) { stdDeck.put(new Card(Card.Color.SPADES,i)); }

		return stdDeck;
	}

	public int size()
	{
		return deck.size();
	}

	public void put(Card card)
	{
		deck.add(card);
	}

	public Card pop()
	{
		if (deck.isEmpty())
			return null;

		Card top = deck.get( deck.size() - 1 );
		deck.remove( deck.size() - 1 );
		return top;
	}
}