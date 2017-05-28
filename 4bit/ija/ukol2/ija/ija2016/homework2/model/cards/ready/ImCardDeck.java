package ija.ija2016.homework2.model.cards.ready;

import ija.ija2016.homework2.model.cards.CardDeck;
import ija.ija2016.homework2.model.cards.Card;
import java.util.ArrayList;

public class ImCardDeck implements CardDeck {
	private ArrayList<Card> deck;
	private Card.Color color;
	private boolean targetPack;
	private int value;

	public ImCardDeck(int size)
	{
		this.targetPack = false;
		this.deck = new ArrayList<Card>(size);
		this.color = null;
	}

	public ImCardDeck(Card.Color color)
	{
		this.targetPack = true;
		this.value = 1;
		this.deck = new ArrayList<Card>();
		this.color = color;
	}

	public static CardDeck createStandardDeck() {
		CardDeck stdDeck = new ImCardDeck(52);
		
		for (int i = 1; i <= 13; i++) { stdDeck.put(new ImCard(Card.Color.CLUBS,i)); }
        for (int i = 1; i <= 13; i++) { stdDeck.put(new ImCard(Card.Color.DIAMONDS,i)); }
        for (int i = 1; i <= 13; i++) { stdDeck.put(new ImCard(Card.Color.HEARTS,i)); }
        for (int i = 1; i <= 13; i++) { stdDeck.put(new ImCard(Card.Color.SPADES,i)); }

		return stdDeck;
	}

	public int size() {
		return deck.size();
	}

	public boolean put(Card card) {
		if (targetPack){
			if ( !card.similarColorTo(new ImCard(this.color, 7)) || card.value() != this.value )
			{
				return false;
			}
			value++;
		}

		return deck.add(card);
	}

	public Card pop() {
		if (deck.isEmpty()) 
			return null;

		int last = deck.size() - 1;
		Card top = deck.get(last);
		deck.remove(last);
		return top;
	}

	public Card get() {
		if (deck.isEmpty()) 
			return null;

		return deck.get( deck.size() - 1);
	}

	public Card get(int index) {
		if (deck.isEmpty() || index < 0 || index >= deck.size()) 
			return null;

		return deck.get(index);
	}

	public boolean isEmpty() {
		return deck.isEmpty();
	}
}